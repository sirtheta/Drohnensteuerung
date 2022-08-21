//https://softwareengineering.stackexchange.com/questions/186124/programming-pid-loops-in-c


#include "arduino.h"

#ifndef pid_controller_h
#define pid_controller_h

class PIDcontroller
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
    PIDcontroller(float _p, float _i, float _d, float* _pOutput);
    void processError(float _error, float _dTime);
};

PIDcontroller::PIDcontroller(float _p, float _i, float _d, float* _pOutput)
{
    p               = _p;
    i               = _i;
    d               = _d;
    pOutput         = _pOutput;
    error           = 0;
    integralError   = 0;
}

void PIDcontroller::processError(float _error, float _dTime)
{
    integralError += _error;

    *pOutput = p * _error + (i * integralError * _dTime) + (d * (_error - error) / _dTime);
    
    error = _error;
}

#endif
