#include "arduino.h"
#include "PID_controller.h"

#ifndef protocolhandler_h
#define protocolhandler_h

class ProtocolHandler
{
private:
  void sendPIDToSerial(float _fval, String _strAxis, String _strPidParam);

public:    
    void setterPidValue(String, float, PIDController*);
    void getterPidValue(String, String, PIDController*);
};
#endif