#include <Arduino_LSM9DS1.h>
#include <vector_type.h>
#include <PID_controller.h>
#include <ProtocolHandler.h>
#include <GlobalDefines.h>

// max length of incoming message
#define MAX_MESSAGE_LENGTH 20

bool DEBUG = false;

vec3_t anglespeedVect         = {0,0,0}; //anglespeed readings of the gyro
vec3_t accelVect              = {0,0,0}; //acceleration readings
vec3_t accelEulerAnglesVect   = {0,0,0}; //calculated acceleration angles

vec3_t startVect              = {0,0,0}; //acceleration vector at start
vec3_t currentEulerAnglesVect = {0,0,0}; // current orientation vector

// threshold values for acceleration and gyro values
float thresholdVect = 0.03;
float thresholdGyro = 0.5;

float timeStamp     = 0;
// initalise parameter position, used for incoming messages
short paramPosition = 0;

//initial PID Values
// can be overwritten via protocol
float fPidP = 0.3;
float fPidI = 0.03;
float fPidD = 0.003;

// values for PID controllers
float xCorrection;
float yCorrection;
float zCorrection;

//PID CONTROLLERS
PIDController xController(fPidP, fPidI, fPidD, &xCorrection);
PIDController yController(fPidP, fPidI, fPidD, &yCorrection);
PIDController zController(fPidP, fPidI, fPidD, &zCorrection);

//Protocol Handler
ProtocolHandler protocolHandler;

// Protocol data
String strCommand;
String strAxis;
String strPidParam;
float fValue;

// param enum for protocol switchcase
enum Param 
{
  command = 1,
  axis = 2,
  pidParam = 3,
  value = 4
};

// function to calculate the current angles
void calculateCurrentAngles()
{
  delay(5);

  //flag if sensor is beeing accelerated
  bool accelerating = !(accelVect.mag() < startVect.mag() + thresholdVect && accelVect.mag() > startVect.mag() - thresholdVect);
  //calculate time delta
  float dTime = (millis() - timeStamp)/1000;

  //calculate errors
  float xError = accelerating ? anglespeedVect.x * dTime : accelEulerAnglesVect.x - currentEulerAnglesVect.x;
  float yError = accelerating ? anglespeedVect.y * dTime : accelEulerAnglesVect.y - currentEulerAnglesVect.y;
  float zError = anglespeedVect.y * dTime;

  //running PID loops for correcting the current angles
  xController.processError(xError, dTime);
  yController.processError(yError, dTime);
  zController.processError(zError, dTime);

  //applying corrections to current angles
  currentEulerAnglesVect.x += xCorrection;
  currentEulerAnglesVect.y += yCorrection;
  currentEulerAnglesVect.z += zCorrection;

  timeStamp = millis();
}

//Function to calculate the correction-values for each axis
void inititalizeGyroscope(int _sampleSize)
{
  float corrValX = 0;
  float corrValY = 0;
  float corrValZ = 0;

  for (int i = 0; i < _sampleSize; i++)
  { 
    Serial.println("Gyroscope calibrating.. step: " + String(i) + "/" + String(_sampleSize));
    delay(2);
    float xRaw, yRaw, zRaw;
    IMU.readRawGyro(xRaw,yRaw, zRaw);
    corrValX += xRaw;
    corrValY += yRaw;
    corrValZ += zRaw;
  }

  corrValX = corrValX / _sampleSize;
  corrValY = corrValY / _sampleSize;
  corrValZ = corrValZ / _sampleSize;

  IMU.setGyroOffset(corrValX,corrValY,corrValZ);
}


//Function to read the gyro and subtract the correction-values
void readCorrectedGyro()
{
  if (IMU.gyroAvailable()) 
  {
    IMU.readGyro(anglespeedVect.x,anglespeedVect.y,anglespeedVect.z);
  }
}

// read current acceleration
void readAccelerometer()
{
  if (IMU.accelAvailable())
  {
    IMU.readAccel(accelVect.x,accelVect.y,accelVect.z);
  }
}

//calculate X,Y eulerangles of a given vector (in reference of axis) => we will have to change this
void calculateAngelsOfVector(vec3_t _vectorIn, vec3_t* _anglesOut)
{
 _anglesOut->x = (acosf(_vectorIn.x / _vectorIn.mag())) * 180 / PI; //(*180/pi) => convert rad in degrees
 _anglesOut->y = (acosf(_vectorIn.y / _vectorIn.mag())) * 180 / PI; //(*180/pi) => convert rad in degrees
 _anglesOut->z = 0; //Calculation of Z rotation is not possible 
}

//copies the accelerationvector on startup as startvector
void initializeStartVector()
{
  IMU.readAccel(startVect.x, startVect.y, startVect.z);
  calculateAngelsOfVector(startVect, &currentEulerAnglesVect);
}

void setup() 
{
  Serial.begin(9600);

  while (!Serial);

  Serial.println("Started");

  if (!IMU.begin()) { Serial.println("Failed to initialize IMU!"); while (1); }

  IMU.setGyroODR(5);

  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.getGyroODR());
  Serial.println(" Hz");

  timeStamp = millis();

  inititalizeGyroscope(500);
  initializeStartVector();
}

/***********************************************
 Protocol Definiton to send and receive PID Values
***********************************************
 setPidValues:
 string must start with: "PIDS"
 folowing by the axis Parameter X,Y or Z
 folowing with the P, I or D and the value to set
 example:
  PIDS|X|P|0.3;
 example to get PID:
  PIDR|X|P;
example of requested transfered data:
  PIDT|X|P|0.3;
example for angle Frame:
  ANGT|X|90.00;
example for angle Frame:
  MOVT|X|0.5;
***********************************************/
// sets the received PID value via the setter method of the protocol handler
void setPidValue()
{
  if (strAxis == "X")
  {
    protocolHandler.setterPidValue(strPidParam, fValue, &xController);
  }
  else if (strAxis == "Y")
  {
    protocolHandler.setterPidValue(strPidParam, fValue, &yController);
  }
  else if (strAxis == "Z")
  {
    protocolHandler.setterPidValue(strPidParam, fValue, &zController);
  }
}

// gets the current PID value from received command and sends it 
// via the getter method of protocol handler
void getPidValue()
{
  if (strAxis == "X")
  {
    protocolHandler.getterPidValue(strAxis, strPidParam, &xController);
  }
  else if (strAxis == "Y")
  {
    protocolHandler.getterPidValue(strAxis, strPidParam, &yController);
  }
  else if (strAxis == "Z")
  {
    protocolHandler.getterPidValue(strAxis, strPidParam, &zController);
  }
}

// execute the incoming command depending on received command
void executeIncomingCommand()
{
  if (strCommand == chrSetPid)
  {
    setPidValue();
  }
  else if (strCommand == chrReadPid)
  {
    getPidValue();
  }  
}

void loop() 
{ 
  readCorrectedGyro(); //read gyro => outputs angularvelocity of each axis
  readAccelerometer(); //read accelerometer => outputs the current acceleration as 3d vector
  calculateAngelsOfVector(accelVect, &accelEulerAnglesVect); //calculates the angles of the acceleretionvector X,Y

  calculateCurrentAngles();

  // Check to see if anything is available in the serial receive buffer
  while (Serial.available() > 0)
  {
    // Create a place to hold the incoming message
    static char message[MAX_MESSAGE_LENGTH];
    static unsigned int message_pos = 0;

    // Read the next available byte in the serial receive buffer
    char inByte = Serial.read();

    // Message coming check for pipe
    if (inByte != paramSeparator && inByte != cmdTerminator && message_pos < MAX_MESSAGE_LENGTH - 1)
    {
      // Add the incoming byte to message
      message[message_pos] = inByte;
      message_pos++;
    }
    else
    {
      // adds terminator to message 
      message[message_pos] = '\0';
      paramPosition++;
      //delay(500);
      // Reset for the next message
      message_pos = 0;

      // fill the incoming message into the correct string for later execution
      switch (paramPosition)
      {
      case command:
        strCommand = String(message);
        break;
      case axis:
        strAxis = String(message);
        break;
      case pidParam:
        strPidParam = String(message);
        break;
      case value:
        fValue = String(message).toFloat();
        break;
      default:
        break;
      }

      if (inByte == cmdTerminator)
      {
        paramPosition = 0; // reset parameter position
        message_pos = 0; // reset message position, read for new message
        executeIncomingCommand(); // execute the incoming command
        //delay(1000); // do wen need a delay here??
      }
    }
  }

  //float array to send:
  float data[]
  {
    currentEulerAnglesVect.x,
    currentEulerAnglesVect.y,
    currentEulerAnglesVect.z,
    accelVect.x,
    accelVect.y,
    accelVect.z
  };
  //sending float array as dataframe
  protocolHandler.sendDataFrame(data,6);

}