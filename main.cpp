#include "simlib.h"


// Define parameters
Parameter arrivalRateMin(8.0), arrivalRateMax(18.0);
Parameter orderTimeMin(1.0), orderTimeMax(5.0);
Parameter cookTimeMin(6.0), cookTimeMax(8.0);
Parameter serveOnSite(2.0);
Parameter serveToGo(1.0);
Parameter eatTimeMin(15.0), eatTimeMax(25.0);
Parameter cleanTimeMin(1.0), cleanTimeMax(2.0);
Parameter leaveTimeMin(15.0), leaveTimeMax(20.0);

Facility Waiter("Waiter");
Facility Helper("Helper");


class Customer : public Process {
    void Behavior() {
        Print("%f: Customer arrived\n", T.Value());

        double leaveTime = T.Value() + Uniform(leaveTimeMin.Value(), leaveTimeMax.Value());
        Activate(leaveTime);

        // Order food
        while(T.Value() < leaveTime){
            // Attempt to use the waiter
            if(!Waiter.Busy()){ // Waiter is available
                Waiter.Seize(this);
                Print("%f: Customer is ordering with the Waiter\n", T.Value());
                Wait(Uniform(orderTimeMin.Value(), orderTimeMax.Value()));
                Print("%f: Customer finished ordering\n", T.Value());
                Waiter.Release(this);
                return;
            }
            else if(!Helper.Busy()){ // Use Helper if Waiter is busy
                Helper.Seize(this);
                Print("%f: Customer is ordering with the Helper\n", T.Value());
                Wait(Uniform(orderTimeMin.Value(), orderTimeMax.Value()));
                Print("%f: Customer finished ordering\n", T.Value());
                Helper.Release(this);
                return;
            }
            else{
                Passivate();
            }
        }

        // If customer is not served within the time limit, they leave
        Print("%f: Customer waited too long and left\n", T.Value());
    }
};


class Generator : public Event {
    void Behavior() {
        // Generate a new customer
        (new Customer)->Activate();
        // Schedule the next customer
        Activate(T.Value() + Uniform(arrivalRateMin.Value(), arrivalRateMax.Value()));
    }
};


int main(){
    SetOutput("simulation_output.txt");
    Init(0.0, 480.0);

    (new Generator)->Activate(0.0);

    Run();

    return 0;
}