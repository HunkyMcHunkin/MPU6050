#include<Wire.h>
#include "MPU6050.h"
#include "I2Cdev.h"

MPU6050 accelgyro;

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t ax, ay, az, gx, gy, gz;

float gyro_x_cal, gyro_y_cal, gyro_z_cal;
float gyro_z_total;

float rotations = 0;

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);

  for(int i = 0; i < 2000; i++){
    if(i%100==0)Serial.print('.');
    accelgyro.getRotation(&gx, &gy, &gz);
    gyro_x_cal += gx;
    gyro_y_cal += gy;
    gyro_z_cal += gz;
    delay(3);
  }
  
  gyro_x_cal /= 2000;
  gyro_y_cal /= 2000;
  gyro_z_cal /= 2000;
  Serial.println();

}
void loop(){
  accelgyro.getAcceleration(&ax, &ay, &az);
  accelgyro.getRotation(&gx, &gy, &gz);

  gx -= gyro_x_cal;
  gy -= gyro_y_cal;
  gz -= gyro_z_cal;
  
  /*
  Serial.print("AcX = "); Serial.print(ax);
  Serial.print(" | AcY = "); Serial.print(ay);
  Serial.print(" | AcZ = "); Serial.print(az);
  */
  Serial.print(" | GyX = "); Serial.print(gx/65.5);
  Serial.print(" | GyY = "); Serial.print(gy/65.5);
  Serial.print(" | GyZ = "); Serial.println(gz/65.5);

  gyro_z_total +=gz/(20*65.5);
  while(360<=gyro_z_total){
    rotations += 1;
    gyro_z_total -= 360;
  }
  
  while(gyro_z_total<=-360){
    rotations -= 1;
    gyro_z_total += 360;
  }


  Serial.print("[");
  Serial.print(cos(gyro_z_total));
  Serial.print(",");
  Serial.print(sin(gyro_z_total));
  Serial.println("]");
  Serial.print("Antall rotasjoner: ");
  Serial.println(rotations+gyro_z_total/360);
  
  delay(50);
}
