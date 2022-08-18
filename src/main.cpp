/*
  INSERT MEANINGFULL COMMENT HERE :)
*/

#include <Arduino_LSM9DS1.h>
#include <vector_type.h>


vec3_t anglespeedVect = {0,0,0}; //anglespeed readings of the gyro
vec3_t accelVect = {0,0,0}; //acceleration readings
vec3_t accelAngl = {0,0,0}; //calculated acceleration angles

vec3_t startVect = {0,0,0}; //acceleration vector at start
vec3_t currentAngles = {0,0,0}; // current orientation vector

vec3_t gyroVect = {0,0,0}; //gyro vector (gets rotated based on anglespeed)
vec3_t gyroAngl = {0,0,0}; //gyro vector angles
vec3_t gyroDirAngl = {90,90,0}; //gyro angles calculated directly by anglespeed
float corrValX, corrValY, corrValZ; //correction values for correcting the readings

float thresholdVect = 0.05;
float thresholdGyro = 0.5;
float smoothing = 10;

//PID-Loop variables {x,y,z} https://softwareengineering.stackexchange.com/questions/186124/programming-pid-loops-in-c
vec3_t proportionals  = {0.1f,0.1f,0.1f};
vec3_t integrals      = {0.1f,0.1f,0.1f};
vec3_t derivatives    = {0.1f,0.1f,0.1f};

vec3_t errors         = {0.0f,0.0f,0.0f};
vec3_t integralErrors = {0.0f,0.0f,0.0f};

vec3_t timeStamp = {0.0f,0.0f,0.0f};
//PID Ends

float pi = 3.141592;

long lastTime; //used to calculate the rotation


vec3_t isTurning()
{
  vec3_t isTurning = {0, 0, 0};
  if (abs(anglespeedVect.x) > thresholdGyro)
  {
    isTurning.x = true;
  }
  if (abs(anglespeedVect.y) > thresholdGyro)
  {
    isTurning.y = true;
  }
  if (abs(anglespeedVect.z) > thresholdGyro)
  {
    isTurning.z = true;
  }
  return isTurning;
}

void calculateCurrentAnglesViaPIDLoop()
{
  delay(10);

  float dTime = (millis() - timeStamp.x)/1000;
  
  float error =  accelAngl.x - currentAngles.x;
  float dError = error - errors.x;
  integralErrors.x += error;
  currentAngles.x += proportionals.x * error + (integrals.x * integralErrors.x * dTime) + (derivatives.x * dError * dTime);
  errors.x = error;

  error =  accelAngl.y - currentAngles.y;
  dError = error - errors.y;
  integralErrors.y += error;
  currentAngles.y += proportionals.y * error + (integrals.y * integralErrors.y * dTime) + (derivatives.y * dError * dTime);
  errors.y = error;

  error =  accelAngl.z - currentAngles.z;
  dError = error - errors.z;
  integralErrors.z += error;
  currentAngles.z += proportionals.z * error + (integrals.z * integralErrors.z * dTime) + (derivatives.z * dError * dTime);
  errors.z = error;

  timeStamp.x = millis();
}

void calculateCurrentAngles() 
{
  //if current acceleration vector magnitude is within the threshold range of the start vector
  //=> this means that the device is not beeing accelerated
  if (accelVect.mag() < startVect.mag() + thresholdVect && accelVect.mag() > startVect.mag() - thresholdVect)
  {
    vec3_t tempVect = isTurning();

    //if gyroscope is showing a rotation in x
    if (tempVect.x == true)
    {
      currentAngles.x += round(accelAngl.x - currentAngles.x) * (1 / smoothing);
    }
    if (tempVect.y == true)
    {
      currentAngles.y += round(accelAngl.y - currentAngles.y) * (1 / smoothing);
    }
    if (tempVect.z == true)
    {
      currentAngles.z += round(accelAngl.z - currentAngles.z) * (1 / smoothing);
    }
  } 
}


//Method to calculate the correction-values for each axis
void inititalizeGyroscope(int _sampleSize)
{
  corrValX = 0;
  corrValY = 0;
  corrValZ = 0;

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

}

//method to rotate a given vector by some angles x,y,z
void rotateVector(vec3_t* _vect, vec3_t _ang)
{

  /*
  ROTATING AROUND Z-AXIS:
    |cos θ   −sin θ    0| |x|   |x cos θ − y  sin θ|   |x'|
    |sin θ    cos θ    0| |y| = |x sin θ + y  cos θ| = |y'|
    |  0       0       1| |z|   |        z         |   |z'|

  ROTATING AROUND Y-AXIS:
    | cos θ    0   sin θ| |x|   | x cos θ + z sin θ|   |x'|
    |   0      1       0| |y| = |         y        | = |y'|
    |−sin θ    0   cos θ| |z|   |−x sin θ + z cos θ|   |z'|

  ROTATING AROUND X-AXIS:
    |1     0           0| |x|   |        x         |   |x'|
    |0   cos θ    −sin θ| |y| = |y cos θ − z  sin θ| = |y'|
    |0   sin θ     cos θ| |z|   |y sin θ + z  cos θ|   |z'|

  ref: https://stackoverflow.com/questions/14607640/rotating-a-vector-in-3d-space
  */
 float sinZ = sinf(_ang.z);
 float cosZ = cosf(_ang.z);

 float sinY = sinf(_ang.y);
 float cosY = cosf(_ang.y);

 float sinX = sinf(_ang.x);
 float cosX = cosf(_ang.x);

  //rotate around Z-Axis:
  /*X'*/ _vect->x =  _vect->x * cosZ - _vect->y * sinZ;
  /*Y'*/ _vect->y =  _vect->x * sinZ + _vect->y * cosZ;
  /*Z'*/

  //rotate around Y-Axis:
  /*X'*/ _vect->x =  _vect->x * cosY + _vect->z * sinY;
  /*Y'*/
  /*Z'*/ _vect->z = -_vect->x * sinY + _vect->z * cosY;

  //rotate around X-Axis:
  /*X'*/
  /*Y'*/ _vect->y =  _vect->y * cosX - _vect->z * sinX;
  /*Z'*/ _vect->z =  _vect->y * sinX + _vect->z * cosX;
}


//Function to read thr accelerometer
void readAccelerometer()
{
  if(IMU.accelAvailable())
  {
    IMU.readAccel(accelVect.x, accelVect.y, accelVect.z);
  }
}

//Function to read the gyro and subtract the correction-values
void readCorrectedGyro()
{
  if (IMU.gyroscopeAvailable()) 
  {
    IMU.readGyro(anglespeedVect.x,anglespeedVect.y,anglespeedVect.z);
    anglespeedVect.x -= corrValX;
    anglespeedVect.y -= corrValY;
    anglespeedVect.z -= corrValZ;
  }
}

//calculate X,Y eulerangles of a given vector (in reference of axis) => we will have to change this
void calculateAngelsOfVector(vec3_t _vectorIn, vec3_t* _anglesOut)
{
 _anglesOut->x = (acosf(_vectorIn.x / _vectorIn.mag())) * 180 / pi; //(*180/pi) => convert rad in degrees
 _anglesOut->y = (acosf(_vectorIn.y / _vectorIn.mag())) * 180 / pi; //(*180/pi) => convert rad in degrees
 _anglesOut->z = 0; //Calculation of Z rotation is not possible 

}

//Function to calculate the actual angles (time * speed) not accurate
void calculateGyroAxisAngles()
{
  long currenTime = millis();
  long deltaT = currenTime- lastTime;
  float xChange = (anglespeedVect.x / 1000) * deltaT;
  float yChange = (anglespeedVect.y / 1000) * deltaT;
  float zChange = (anglespeedVect.z / 1000) * deltaT;
  lastTime = currenTime;
  gyroDirAngl.x = gyroDirAngl.x + xChange;
  gyroDirAngl.y = gyroDirAngl.y + yChange;
  gyroDirAngl.z = gyroDirAngl.z + zChange;

  vec3_t angleChange {xChange * pi / 180,yChange * pi / 180,zChange * pi / 180};
  rotateVector(&gyroVect, angleChange);
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
  // startVect = startVect.norm();
  gyroVect = startVect;
  calculateAngelsOfVector(startVect, &currentAngles);
}

void setup() 
{

  Serial.begin(9600);

  while (!Serial);

  Serial.println("Started");

  if (!IMU.begin()) 
  {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  IMU.setGyroODR(5);

  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.getGyroODR());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Gyroscope in degrees/second");

  lastTime = millis();
  timeStamp = {lastTime, lastTime, lastTime};

  inititalizeGyroscope(1000);
  initializeStartVector(); 
}

void loop() 
{ 
  readCorrectedGyro(); //read gyro => outputs angularvelocity of each axis
  readAccelerometer(); //read accelerometer => outputs the current acceleration as 3d vector
  calculateAngelsOfVector(accelVect, &accelAngl); //calculates the angles of the acceleretionvector X,Y
  //calculateCurrentAngles(); //calculates the current angles based on som filtering of the accelerationvector)
  calculateCurrentAnglesViaPIDLoop();
  Serial.println("CURRENT VECT ANGLES: " + vectorToString(currentAngles) + " ANGLESPEED: " + vectorToString(anglespeedVect));
}