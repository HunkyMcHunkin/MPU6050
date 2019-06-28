
#include<Wire.h>
#include "MPU6050.h"
#include "I2Cdev.h"
#include <RH_ASK.h>
#include <SPI.h> 

RH_ASK driver;

MPU6050 accelgyro;

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t gx, gy, gz;

float gyro_z_cal;
float gyro_z_total;
float rotations = 0;
int tot_rot;

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);

  if (!driver.init())
         Serial.println("init failed");

  Serial.println("Calibrating...");
  for(int i = 0; i < 2000; i++){
    if(i%100==0)Serial.print('.');
    accelgyro.getRotation(&gx, &gy, &gz);
    gyro_z_cal += gz;
    delay(3);
  }

  gyro_z_cal /= 2000;
  Serial.println();

}
void loop(){

  accelgyro.getRotation(&gx, &gy, &gz);
  gz -= gyro_z_cal;

  Serial.print("GyZ = "); Serial.println(gz/65.5);

  gyro_z_total += gz/(20*65.5);

  while(360<=gyro_z_total){
    rotations += 1;
    gyro_z_total -= 360;
  }  
  while(gyro_z_total<=-360){
    rotations -= 1;
    gyro_z_total += 360;
  }

  Serial.print("Rotations: ");
  Serial.println((rotations + gyro_z_total/360)/0.58);

  char msg[5];
  tot_rot = 1000*(rotations + gyro_z_total/360)/0.58;
  dtostrf(tot_rot, 5, 1, msg);
  driver.send((uint8_t*)msg, strlen(msg));
  driver.waitPacketSent();
  
  delay(50);
}
