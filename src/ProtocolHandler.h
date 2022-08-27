#include "arduino.h"
#include "PID_controller.h"

#ifndef protocolhandler_h
#define protocolhandler_h

class ProtocolHandler
{
private:
  void sendPIDToSerial(float _fval, String _strAxis, String _strPidParam);

public:    
  void setterPidValue(String param, float value, PIDController* controller);
  void getterPidValue(String axis, String param, PIDController* controller);
  void sendFrame(float, String axis, String cmd);
  void sendDataFrame(float[], int);
};
#endif