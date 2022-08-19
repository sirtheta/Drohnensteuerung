/*
  INSERT MEANINGFULL COMMENT HERE :)
*/

#include <Arduino_LSM9DS1.h>
#include <vector_type.h>
#include <PID_controller.h>


vec3_t anglespeedVect = {0,0,0}; //anglespeed readings of the gyro
vec3_t accelVect = {0,0,0}; //acceleration readings
vec3_t accelAngl = {0,0,0}; //calculated acceleration angles

vec3_t startVect = {0,0,0}; //acceleration vector at start
vec3_t currentAngles = {0,0,0}; // current orientation vector


float thresholdVect = 0.03;
float thresholdGyro = 0.5;

float timeStamp = 0;

//PID CONTROLLERS
float xCorrection;
PIDcontroller xController(0.3, 0.03, 0.003, &xCorrection);
float yCorrection;
PIDcontroller yController(0.3, 0.03, 0.003, &yCorrection);
float zCorrection;
PIDcontroller zController(0.3, 0.03, 0.003, &zCorrection);


void calculateCurrentAngles()
{
  delay(5);

  //flag if sensor is beeing accelerated
  bool accelerating = !(accelVect.mag() < startVect.mag() + thresholdVect && accelVect.mag() > startVect.mag() - thresholdVect);
  float dTime = (millis() - timeStamp)/1000;
  float error =0;

  //running PID loops for correcting the current angles
  error = accelerating ? anglespeedVect.x * dTime : accelAngl.x - currentAngles.x;
  xController.processError(error, dTime);
  error = accelerating ? anglespeedVect.y * dTime : accelAngl.y - currentAngles.y;
  yController.processError(error, dTime);
  error =  anglespeedVect.y * dTime;
  zController.processError(anglespeedVect.z * dTime,dTime);


  //applying corrections to current angles
  currentAngles.x += xCorrection;
  currentAngles.y += yCorrection;
  currentAngles.z += zCorrection;

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
  if (IMU.gyroscopeAvailable()) 
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
  calculateAngelsOfVector(startVect, &currentAngles);
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

void loop() 
{ 
  readCorrectedGyro(); //read gyro => outputs angularvelocity of each axis
  readAccelerometer(); //read accelerometer => outputs the current acceleration as 3d vector
  calculateAngelsOfVector(accelVect, &accelAngl); //calculates the angles of the acceleretionvector X,Y

  calculateCurrentAngles();

  Serial.println("CURRENT VECT ANGLES: " + vectorToString(currentAngles) + " ANGLESPEED: " + vectorToString(anglespeedVect));
}