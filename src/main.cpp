/*

  Arduino LSM9DS1 - Gyroscope Application


  This example reads the gyroscope values from the LSM9DS1 sensor 

  and prints them to the Serial Monitor or Serial Plotter, as a directional detection of 

  an axis' angular velocity.

https://www.youtube.com/watch?v=U7lf_E79j7Q
  The circuit:

  - Arduino Nano 33 BLE Sense


  Created by Riccardo Rizzo


  Modified by Benjamin Danneg�rd

  30 Nov 2020


  This example code is in the public domain.

*/


#include <Arduino_LSM9DS1.h>


float gX, gY, gZ; //anglespeed readings of the gyro
float aX, aY, aZ; //acceleration
float actX, actY, actZ; //actual angles
float corrValX, corrValY, corrValZ; //correction values for correcting the readings

long lastTime;


//Method to calculate the correction-values for each axis
void inititalizeGyroscope()
{
  corrValX = 0;
  corrValY = 0;
  corrValZ = 0;

  for (int i = 0; i < 1000; i++)
  { 
    float xRaw, yRaw, zRaw;
    IMU.readRawGyro(xRaw,yRaw, zRaw);
    corrValX += xRaw;
    corrValY += yRaw;
    corrValZ += zRaw;
  }
  corrValX = corrValX / 1000;
  corrValY = corrValY / 1000;
  corrValZ = corrValZ / 1000;

}

void readAccelerometer()
{
 IMU.readAccel(aX,aY,aZ);
}

//Method to read the gyro and subtract the correction-values
void readCorrectedGyro()
{
  if (IMU.gyroscopeAvailable()) 
  {
    IMU.readGyro(gX,gY,gZ);
    gX -= corrValX;
    gY -= corrValY;
    gZ -= corrValZ;
  }
}

//Method to calculate the actual angles (time * speed) not accurate
void calculateGyroAxisAngles()
{
  long currenTime = millis();
  long deltaT = currenTime- lastTime;
  float xChange = (gX/1000)*deltaT;
  float yChange = (gY/1000)*deltaT;
  float zChange = (gZ/1000)*deltaT;
  lastTime = currenTime;
  actX = actX + xChange;
  actY = actY + yChange;
  actZ = actZ + zChange;
}

//string to display the actual angles
String gyroAxisAnglesText()
{
  return "GYRO ANGLES:  X" + String(actX) + "X Y" + String(actY) + "Y Z" + String(actZ) + "Z";
}

//string to display the acceleration
String accelerationText()
{
  return "ACCELERATION:  X" + String(aX) + "X Y" + String(aY) + "Y Z" + String(aZ) + "Z";
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

  actX = 0;
  actY = 0;
  actZ = 0;
  lastTime = millis();

  inititalizeGyroscope(); 
}

void loop() 
{  
  readCorrectedGyro();
  readAccelerometer();
  calculateGyroAxisAngles();
  Serial.println( gyroAxisAnglesText() + " ||||| " + accelerationText());
 
}