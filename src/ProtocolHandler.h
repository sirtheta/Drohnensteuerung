#include "arduino.h"
#include "PID_controller.h"

#ifndef protocolhandler_h
#define protocolhandler_h

class ProtocolHandler
{
private:
  void sendPIDToSerial(float _fval, String _strAxis, String _strPidParam);

public:    
    void setterPidValue(String _strPIDParam, float _fValue, PIDController _pidCtrl);
    void getterPidValue(String _strAxis, String _strPIDParam, PIDController _pidCtrl);
};
#endif