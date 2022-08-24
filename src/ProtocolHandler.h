#include "arduino.h"
#include "PID_controller.h"

#ifndef protocolhandler_h
#define protocolhandler_h

class ProtocolHandler
{
  private:
    void sendPIDToSerial(float _fval);

  public:
    ProtocolHandler();

    char cmdTerminator ;
    char paramSeparator;
    char chrSetPid[5];
    char chrReadPid[5];
    char chrTransferPid[5];
    
    static void setterPidValues(String _strPIDParam, float _fValue, PIDController _pidCtrl);
    static void getterPidValues(String _strAxis, String _strPIDParam, PIDController _pidCtrl);
};
#endif