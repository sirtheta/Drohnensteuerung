#include <Arduino_LSM9DS1.h>
#include <vector_type.h>
#include <PID_controller.h>
#include <ProtocolHandler.h>
#include <GlobalDefines.h>

// Max length of incoming message
#define MAX_MESSAGE_LENGTH 20

bool DEBUG = false;
bool verticalMode = true; // Set to true if you're using the 3D-Printed joystick

vec3_t anglespeedVect         = {0,0,0}; // Anglespeed readings of the gyro
vec3_t accelVect              = {0,0,0}; // Acceleration readings
vec3_t accelEulerAnglesVect   = {0,0,0}; // Calculated acceleration angles

vec3_t startVect              = {0,0,0}; // Acceleration vector at start
vec3_t currentEulerAnglesVect = {0,0,0}; // Current orientation vector

// Threshold values for acceleration and gyro values
float thresholdVect = 0.03;
float thresholdGyro = 0.5;

float timeStamp     = 0;
// Initalise parameter position, used for incoming messages
short paramPosition = 0;

// Initial PID Values
// can be overwritten via protocol
float fPidP = 0.3;
float fPidI = 0.03;
float fPidD = 0.003;

// values for PID controllers
float xCorrection;
float yCorrection;
float zCorrection;

// PID CONTROLLERS
PIDController xController(fPidP, fPidI, fPidD, &xCorrection);
PIDController yController(fPidP, fPidI, fPidD, &yCorrection);
PIDController zController(fPidP, fPidI, fPidD, &zCorrection);

// Protocol Handler
ProtocolHandler protocolHandler;

// Protocol data
String strCommand;
String strAxis;
String strPidParam;
float fValue;

// Param enum for protocol switchcase
enum Param 
{
  command = 1,
  axis = 2,
  pidParam = 3,
  value = 4
};

// Function to calculate the current angles
void calculateCurrentAngles()
{
  delay(5);

  // Flag if sensor is beeing accelerated
  bool accelerating = !(accelVect.mag() < startVect.mag() + thresholdVect && accelVect.mag() > startVect.mag() - thresholdVect);
  // Calculate time delta
  float dTime = (millis() - timeStamp)/1000;

  // Calculate errors
  float xError = accelerating ? anglespeedVect.x * dTime : accelEulerAnglesVect.x - currentEulerAnglesVect.x;
  float yError = accelerating ? anglespeedVect.y * dTime : accelEulerAnglesVect.y - currentEulerAnglesVect.y;
  float zError = anglespeedVect.y * dTime;

  // Running PID loops for correcting the current angles
  xController.processError(xError, dTime);
  yController.processError(yError, dTime);
  zController.processError(zError, dTime);

  // Applying corrections to current angles
  currentEulerAnglesVect.x += xCorrection;
  currentEulerAnglesVect.y += yCorrection;
  currentEulerAnglesVect.z += zCorrection;

  timeStamp = millis();
}

// Function to calculate the correction-values for each axis
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

// Function to read the gyro and subtract the correction-values
void readCorrectedGyro()
{
  if (IMU.gyroAvailable()) 
  {
     // Vertical mode switches and inverses axis accordingly
    if(verticalMode)
    {
      IMU.readGyro(anglespeedVect.z,anglespeedVect.x,anglespeedVect.y);
      anglespeedVect = -anglespeedVect;
    }
    else
    {
      IMU.readGyro(anglespeedVect.x,anglespeedVect.y,anglespeedVect.z);
    }        
  }
}

// Read current acceleration
void readAccelerometer()
{
  if (IMU.accelAvailable())
  {
     // Vertical mode switches and inverses axis accordingly
    if(verticalMode)
    {
      IMU.readAccel(accelVect.z,accelVect.x,accelVect.y);
      accelVect = -accelVect;
    }
    else
    {
      IMU.readAccel(accelVect.x,accelVect.y,accelVect.z);
    }    
  }
}

// Calculate X,Y eulerangles of a given vector (in reference of axis)
void calculateAngelsOfVector(vec3_t _vectorIn, vec3_t* _anglesOut)
{
  _anglesOut->x = (acosf(_vectorIn.x / _vectorIn.mag())) * 180 / PI; //(*180/pi) => convert rad in degrees
  _anglesOut->y = (acosf(_vectorIn.y / _vectorIn.mag())) * 180 / PI; //(*180/pi) => convert rad in degrees
  _anglesOut->z = 0; //Calculation of Z rotation is not possible 
}

// Copies the accelerationvector on startup as startvector
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

// Sets the received PID value via the setter method of the protocol handler
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

// Gets the current PID value from received command and sends it 
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

// Execute the incoming command depending on received command
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
  readCorrectedGyro(); // Read gyro => outputs angularvelocity of each axis
  readAccelerometer(); // Read accelerometer => outputs the current acceleration as 3d vector
  calculateAngelsOfVector(accelVect, &accelEulerAnglesVect); // Calculates the angles of the acceleretionvector X,Y

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
      // Adds terminator to message 
      message[message_pos] = '\0';
      // Ready for next parameter
      paramPosition++;

      // Reset for the next message
      message_pos = 0;

      // Fill the incoming message into the correct string for later execution
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
        paramPosition = 0;        // Reset parameter position
        message_pos = 0;          // Reset message position, read for new message
        executeIncomingCommand(); // Execute the incoming command
      }
    }
  }

  // Float array to send:
  float data[]
  {
    currentEulerAnglesVect.x,
    currentEulerAnglesVect.y,
    currentEulerAnglesVect.z,
    accelVect.x,
    accelVect.y,
    accelVect.z
  };
  // Sending float array as dataframe
  protocolHandler.sendDataFrame(data,6);

}