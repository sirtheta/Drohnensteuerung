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
    /*set Pid functions*/
    void setPID_P(float _p);
    void setPID_I(float _i);
    void setPID_D(float _d);
    /*get Pid functions*/
    float getPID_P();
    float getPID_I();
    float getPID_D();
};

#endif
