//https://softwareengineering.stackexchange.com/questions/186124/programming-pid-loops-in-c


#include "PID_controller.h"

PIDController::PIDController(float _p, float _i, float _d, float* _pOutput)
{
    p               = _p;
    i               = _i;
    d               = _d;
    pOutput         = _pOutput;
    error           = 0;
    integralError   = 0;
}

void PIDController::setPIDS(float _p, float _i, float _d)
{
  p = _p;
  i = _i;
  d = _d;
}

void PIDController::processError(float _error, float _dTime)
{
    integralError += _error;

    *pOutput = p * _error + (i * integralError * _dTime) + (d * (_error - error) / _dTime);
    
    error = _error;
}
