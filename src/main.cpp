/*
  INSERT MEANINGFULL COMMENT HERE :)
*/

#include <Arduino_LSM9DS1.h>
#include <vector_type.h>
#include <PID_controller.h>

#define MAX_MESSAGE_LENGTH 20

bool DEBUG = false;

vec3_t anglespeedVect = {0,0,0}; //anglespeed readings of the gyro
vec3_t accelVect = {0,0,0}; //acceleration readings
vec3_t accelEulerAnglesVect = {0,0,0}; //calculated acceleration angles

vec3_t startVect = {0,0,0}; //acceleration vector at start
vec3_t currentEulerAnglesVect = {0,0,0}; // current orientation vector

float thresholdVect = 0.03;
float thresholdGyro = 0.5;

float timeStamp = 0;

short paramPosition = 0;

//PID Values
float pidPX = 0.3;
float pidIX = 0.03;
float pidDX = 0.003;

float pidPY = 0.3;
float pidIY = 0.03;
float pidDY = 0.003;

float pidPZ = 0.3;
float pidIZ = 0.03;
float pidDZ = 0.003;

//PID CONTROLLERS
float xCorrection;
PIDController xController(pidPX, pidIX, pidDX, &xCorrection);
float yCorrection;
PIDController yController(pidPY, pidIY, pidDY, &yCorrection);
float zCorrection;
PIDController zController(pidPZ, pidIY, pidDY, &zCorrection);

String strCommand;
String strAxis;
String strPidParam;
float fValue;

char cmdTerminator = ';';
char paramSeparator = '|';

char chrSetPid[5] = "PIDS";
char chrReadPid[5] = "PIDR";

enum Param 
{
  command = 1,
  axis = 2,
  pidParam = 3,
  value = 4
};

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


//used to desplay a vector as readable string
String vectorToString(vec3_t _vector)
{
  return "| X:" + String(_vector.x) + "| Y:" + String(_vector.y) + "| Z:" + String(_vector.z);
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

void sendFrame()
{
  float x     = currentEulerAnglesVect.x;
  float y     = currentEulerAnglesVect.y;
  float z     = currentEulerAnglesVect.z;
  float moveX = accelVect.x;
  float moveY = accelVect.y;

  Serial.println(String(x) + ":" + String(y) + ":" + String(z) +":" + String(moveX) + ":" + String(moveY));
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
***********************************************/

// this is a shit... :(
void setPidValue()
{
  if (strAxis == "X")
  {
    if (strPidParam == "P")
    {
      xController.setPID_D(fValue);
    }
    else if (strPidParam == "I")
    {
      xController.setPID_I(fValue);
    }
    else if (strPidParam == "D")
    {
      xController.setPID_D(fValue);
    }
  }
  else if (strAxis == "Y")
  {
    if (strPidParam == "P")
    {
      yController.setPID_D(fValue);
    }
    else if (strPidParam == "I")
    {
      yController.setPID_I(fValue);
    }
    else if (strPidParam == "D")
    {
      yController.setPID_D(fValue);
    }
  }
  else if (strAxis == "Z")
  {
    if (strPidParam == "P")
    {
      zController.setPID_D(fValue);
    }
    else if (strPidParam == "I")
    {
      zController.setPID_I(fValue);
    }
    else if (strPidParam == "D")
    {
      zController.setPID_D(fValue);
    }
  }
}

void executeIncomingCommand()
{
  if (strCommand == chrSetPid) 
  {
    setPidValue();
  }
  else if (strCommand == chrReadPid)
  {

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
      message[message_pos] = '\0';
      paramPosition++;
      //delay(500);
      // Reset for the next message
      message_pos = 0;

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
        paramPosition = 0;
        message_pos = 0;
        executeIncomingCommand();
      }
    }
  }

  if (DEBUG)
  {
    Serial.println("CURRENT VECT ANGLES: " + vectorToString(currentEulerAnglesVect) + " ANGLESPEED: " + vectorToString(anglespeedVect));
  }
  else
  {
    //sendFrame();
  }  
}