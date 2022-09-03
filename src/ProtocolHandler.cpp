#include "ProtocolHandler.h"
#include "GlobalDefines.h"

/***********************************************
 Protocol Definiton to send and receive PID Values
***********************************************
 setPidValues:
 string must start with: "PIDS"
 folowing by the axis Parameter X,Y or Z
 folowing with the P, I or D and the value to set
 example to set PID:
  PIDS|X|P|0.3;
 example to get PID:
  PIDR|X|P;
example of requested transfered data when requested via PIDR:
  PIDT|X|P|0.3;
example of Dataframe:
  DATT|X|Y|Z|X|Y|Z
  DATT|90.05|90.2|3.5|0.1|0.01|1.05
***********************************************/

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

//sends a float array
void ProtocolHandler::sendDataFrame(float _iData[], int _iDataLength)
{
  String frame = chrDataTransfer;

  for(int i =0; i < _iDataLength ;i++)
  {
    frame = frame + paramSeparator + String(_iData[i]);
  }

  Serial.println(frame);
}