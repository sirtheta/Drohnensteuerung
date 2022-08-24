#include <ProtocolHandler.h>

char cmdTerminator = ';';
char paramSeparator = '|';
char chrSetPid[5] = "PIDS";
char chrReadPid[5] = "PIDR";
char chrTransferPid[5] = "PIDT";

ProtocolHandler::ProtocolHandler() {}


// Send the requested PID Value via Serial
void sendPIDToSerial(float _fVal, String _strAxis, String _strPidParam)
{
  int iVal = _fVal * 10000; // transfer float to int because serial will only print two digits after comma
  Serial.println(String(chrTransferPid) + String(paramSeparator) + _strAxis + String(paramSeparator) + _strPidParam + String(paramSeparator) + iVal + String(cmdTerminator));
}

static void setterPidValue(String _strPidParam, float _fValue, PIDController _pidCtrl)
{
  if (_strPidParam == "P")
  {
    _pidCtrl.setPID_P(_fValue);
  }
  else if (_strPidParam == "I")
  {
    _pidCtrl.setPID_I(_fValue);
  }
  else if (_strPidParam == "D")
  {
    _pidCtrl.setPID_D(_fValue);
  }
}

static void getterPidValues(String _strAxis, String _strPidParam, PIDController _pidCtrl)
{
  if (_strPidParam == "P")
  {
    sendPIDToSerial(_pidCtrl.getPID_P(), _strAxis, _strPidParam);
  }
  else if (_strPidParam == "I")
  {
    sendPIDToSerial(_pidCtrl.getPID_I(), _strAxis, _strPidParam);
  }
  else if (_strPidParam == "D")
  {
    sendPIDToSerial(_pidCtrl.getPID_D(), _strAxis, _strPidParam);
  }
}