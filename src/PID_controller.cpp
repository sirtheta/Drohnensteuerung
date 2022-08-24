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

void PIDController::setPID_P(float _p)
{
  p = _p;
}
void PIDController::setPID_I(float _i)
{
  i = _i;
}
void PIDController::setPID_D(float _d)
{
  d = _d;
}

float PIDController::getPID_P()
{
  return p;
}
float PIDController::getPID_I()
{
  return i;
}
float PIDController::getPID_D()
{
  return d;
}

void PIDController::processError(float _error, float _dTime)
{
  integralError += _error;

  *pOutput = p * _error + (i * integralError * _dTime) + (d * (_error - error) / _dTime);
  
  error = _error;
}
