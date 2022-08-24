//https://softwareengineering.stackexchange.com/questions/186124/programming-pid-loops-in-c


#include "arduino.h"

#ifndef pid_controller_h
#define pid_controller_h

class PIDController
{
private:
    //proportional factor
    float p; 
    //integral factor
    float i; 
    //derivative factor
    float d; 

    //last error
    float error;
    //integral error
    float integralError; 

    float* pOutput;
public:
    PIDController(float _p, float _i, float _d, float* _pOutput);
    void processError(float _error, float _dTime);
    void setPIDS(float _p, float _i, float _d);
};

#endif
