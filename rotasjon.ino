/*
Dette er et utkast på hvordan man kan bruke MPU-6050 til å måle rotasjon rundt de forskjellige aksene. 
Jeg har fokusert på Z-aksen, for det er den som er relevant når rotasjonen skjer i xy-planet.
Koden er et utkast det kan bygges videre på.
Her er en video som forklarer noen av prinsippene: https://www.youtube.com/watch?v=4BoIE8YQwM8&list=LLsENxRbqaA0q6FjYJk4QBAw
*/

#include<Wire.h>
#include "MPU6050.h"
#include "I2Cdev.h"

MPU6050 accelgyro;

const int MPU_addr=0x68;  // I2C address of the MPU-6050

//definerer de ulike variablene
int16_t gx, gy, gz;   //rotasjonen rundt hver akse
float gyro_x_cal, gyro_y_cal, gyro_z_cal;   //Kalibreringen for hver akse

float gyro_z_total;   //totalt antall grader vi har beveget oss rundt z-aksen
int rotations = 0;  //gyro_z_total omregnet til antall hele rotasjoner 

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600); 

  //kalibrering: gjennomsnittet av hundre målinger, dette gjennomsnittet skal trekkes fra hver måling i loop()
  Serial.println("Calibrating...");
  for(int i = 0; i < 2000; i++){
    if(i%100==0)Serial.print('.');   //printer en "." for hver hundrede måling, skal bli 20 stk "." tilsammen
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
  //henter målingene fra gyroskopet
  accelgyro.getRotation(&gx, &gy, &gz);
  
  //trwekker fra kalibreringen
  gx -= gyro_x_cal;   
  gy -= gyro_y_cal;
  gz -= gyro_z_cal;
  
  //skriver ut verdiene til serial
  Serial.print(" | GyX = "); Serial.print(gx/65.5);   //vi må dele på 65.5 for å få omgjort rådataen til grader
  Serial.print(" | GyY = "); Serial.print(gy/65.5);   //se vedlagt video for bedre forklaring
  Serial.print(" | GyZ = "); Serial.println(gz/65.5);

  //finner totalt antall grader gyroskopet har beveget seg rundt Z 
  gyro_z_total +=gz/(20*65.5);
  
  //Legger til eller trekker fra 1 på "rotations" om gyroskopet har hatt en hel rotasjon. 
  //Fortegnet avgjøres av hvilken veg gyroskopet roterer. 
  while(360<=gyro_z_total){
    rotations += 1;
    gyro_z_total -= 360;
  }
  
  while(gyro_z_total<=-360){
    rotations -= 1;
    gyro_z_total += 360;
  }
  
  //Skriver ut antall rotasjoner. Legger til et desimaltall som sier hvor langt gyroskopet er på "neste runde".
  Serial.print("Antall rotasjoner: ");
  Serial.println(rotations+gyro_z_total/360);
  
  //Skriver ut vinkelen gyroskopet står i, i forhold til startsposisjonen, på vektorformat (tenk enhetssirkelen).
  Serial.print("[");
  Serial.print(cos(gyro_z_total));
  Serial.print(",");
  Serial.print(sin(gyro_z_total));
  Serial.println("]");
  
  delay(50);
}
