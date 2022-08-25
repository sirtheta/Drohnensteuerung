#include "ProtocolHandler.h"
#include "GlobalDefines.h"


// function to set the incoming PID value
void ProtocolHandler::setterPidValue(String _strPidParam, float _fValue, PIDController* _pidCtrl)
{
  if (_strPidParam == "P")
  {
    _pidCtrl->setPID_P(_fValue);
  }
  else if (_strPidParam == "I")
  {
    _pidCtrl->setPID_I(_fValue);
  }
  else if (_strPidParam == "D")
  {
    _pidCtrl->setPID_D(_fValue);
  }
}

// function to get the requested PID value
void ProtocolHandler::getterPidValue(String _strAxis, String _strPidParam, PIDController* _pidCtrl)
{
  if (_strPidParam == "P")
  {
    sendPIDToSerial(_pidCtrl->getPID_P(), _strAxis, _strPidParam);
  }
  else if (_strPidParam == "I")
  {
    sendPIDToSerial(_pidCtrl->getPID_I(), _strAxis, _strPidParam);
  }
  else if (_strPidParam == "D")
  {
    sendPIDToSerial(_pidCtrl->getPID_D(), _strAxis, _strPidParam);
  }
}

// Send the requested PID Value via Serial
void ProtocolHandler::sendPIDToSerial(float _fVal, String _strAxis, String _strPidParam)
{
  int iVal = _fVal * 10000; // transfer float to int because serial (string) will only print two digits after comma
  Serial.println(String(chrTransferPid) + 
                String(paramSeparator) + 
                _strAxis + 
                String(paramSeparator) + 
                _strPidParam + 
                String(paramSeparator) + 
                iVal + 
                String(cmdTerminator));
}

// sends the frame with the inputs via serial
void ProtocolHandler::sendFrame(float fValue, String strAxis, String strCommand)
{
  Serial.println(strCommand + 
                 paramSeparator + 
                 strAxis + 
                 paramSeparator +
                 fValue + 
                 cmdTerminator);
}