/*

  Arduino LSM9DS1 - Gyroscope Application


  This example reads the gyroscope values from the LSM9DS1 sensor 

  and prints them to the Serial Monitor or Serial Plotter, as a directional detection of 

  an axis' angular velocity.

https://www.youtube.com/watch?v=U7lf_E79j7Q
  The circuit:

  - Arduino Nano 33 BLE Sense


  Created by Riccardo Rizzo


  Modified by Benjamin Dannegård

  30 Nov 2020


  This example code is in the public domain.

*/


#include <Arduino_LSM9DS1.h>


float x, y, z;
float actX;
float corrValX;
float tick = 10;
long lastTime;

int plusThreshold = 30, minusThreshold = -30;


void setup() {

  Serial.begin(9600);

  while (!Serial);

  Serial.println("Started");


  if (!IMU.begin()) {

    Serial.println("Failed to initialize IMU!");

    while (1);
  }

  Serial.print("Gyroscope sample rate = ");
  IMU.setGyroODR(5);

  Serial.print(IMU.getGyroODR());

  Serial.println(" Hz");

  Serial.println();

  Serial.println("Gyroscope in degrees/second");

  actX = 0;
  lastTime = millis();

  for (int i = 0; i < 500; i++)
  {
    float xRaw, yRaw, zRaw;
    IMU.readRawGyro(xRaw,yRaw, zRaw);
    corrValX = corrValX +  xRaw;
  }
  Serial.println(corrValX);
  corrValX = corrValX / 500;
  IMU.setGyroOffset(corrValX, 0, 0);
}

void loop() {  

  if (IMU.gyroscopeAvailable()) {

    IMU.readGyro(x,y,z);
  }
  long currenTime = millis();
  long deltaT = currenTime- lastTime;
  float xChange = (x/1000)*deltaT;
  lastTime = currenTime;
  actX = actX + xChange;
  Serial.println(actX);

}