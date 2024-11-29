#include <simlib.h>


const double arrivalMin = 8.0;
const double arrivalMax = 18.0;
const double orderMin = 1.0;
const double orderMax = 5.0;
const double takeOrderOnSite = 2.0;
const double takeOrderToGo = 1.0;
const double eatTimeMin = 15.0;
const double eatTimeMax = 25.0;
const double cleanTimeMin = 1.0;
const double cleanTimeMax = 2.0;
const double onSpotProb = 0.63;


Queue waiterQueue("Order Queue");
Queue cookQueue("Food Queue");

Facility Waiter("Waiter");
Facility Helper("Helper");
Facility Cook("Cook");


class Customer : public Process{
    bool EatOnSpot;

    void Behavior(){
        order_food();
    }


    void ActivateQueue(Queue &q){
        if(q.Empty()){
            return;
        }

        Customer *c = (Customer *) q.GetFirst();
        c->Activate();
    }


    void ProcessFacility(Facility &f, const double start, const double end){
        f.Seize(this);
        Wait(Uniform(start, end));
        f.Release(this);
    }


    void order_food(){
        while(Waiter.Busy() && Helper.Busy()){
            waiterQueue.Insert(this);
            this->Passivate();
        }

        if(!Waiter.Busy()){
            ProcessFacility(Waiter, orderMin, orderMax);
            ActivateQueue(waiterQueue);
        }

        else if(!Helper.Busy()){
            ProcessFacility(Helper, orderMin, orderMax);
            ActivateQueue(waiterQueue);
        }

        wait_for_food();
    }


    void wait_for_food(){
        while(Cook.Busy() && Helper.Busy()){
            cookQueue.Insert(this);
            this->Passivate();
        }

        if(!Cook.Busy()){
            ProcessFacility(Cook, orderMin, orderMax);
            ActivateQueue(cookQueue);
        }

        else if(!Helper.Busy()){
            ProcessFacility(Helper, orderMin, orderMax);
            ActivateQueue(cookQueue);
        }

        decide_where_to_eat();

        take_Order();
    }


    void decide_where_to_eat(){
        if(Random() <= onSpotProb){
            EatOnSpot = true;
        }

        else{
            EatOnSpot = false;
        }
    }


    void take_Order(){
        while(Waiter.Busy() && Helper.Busy()){
            waiterQueue.InsFirst(this);
            this->Passivate();
        }

        if(!Waiter.Busy()){
            Waiter.Seize(this);
            if(EatOnSpot){
                Wait(takeOrderOnSite);
            }
            else{
                Wait(takeOrderToGo);
            }
            Waiter.Release(this);
            ActivateQueue(waiterQueue);
        }

        else if(!Helper.Busy()){
            Helper.Seize(this);
            if(EatOnSpot){
                Wait(takeOrderOnSite);
            }
            else{
                Wait(takeOrderToGo);
            }
            Helper.Release(this);
            ActivateQueue(waiterQueue);
        }

        if(EatOnSpot){
            eat();
        }
        else{
            return;
        }
    }


    void eat(){
        Wait(Uniform(eatTimeMin, eatTimeMax));
        clean();
    }


    void clean(){
        while(Waiter.Busy() && Helper.Busy()){
            waiterQueue.InsFirst(this);
            this->Passivate();
        }

        if(!Waiter.Busy()){
            ProcessFacility(Waiter, cleanTimeMin, cleanTimeMax);
            ActivateQueue(waiterQueue);
        }

        else if(!Helper.Busy()){
            ProcessFacility(Helper, cleanTimeMin, cleanTimeMax);
            ActivateQueue(waiterQueue);
        }
    }
};


class CustomerGenerator : public Event{
    void Behavior(){
        (new Customer)->Activate();
        Activate(Time + Uniform(arrivalMin, arrivalMax));
    }
};


int main(){
    Init(0, 720);

    (new CustomerGenerator)->Activate();

    Run();

    cookQueue.Output();
    waiterQueue.Output();
    Cook.Output();
    Waiter.Output();
    Helper.Output();

    return 0;
}