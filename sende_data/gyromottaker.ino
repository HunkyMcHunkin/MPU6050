#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile

RH_ASK driver;
float rotations;

void setup()
{
    Serial.begin(9600);  // Debugging only
    Serial.println("Hello World!");
    if (!driver.init())
         Serial.println("init failed");
}

void loop()
{
    uint8_t buf[5];
    uint8_t buflen = sizeof(buf);
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
      int i;
      buf[strlen(buf)-1] = '\0';
      
      Serial.print("Rotations: ");
      sscanf(buf, "%d", &i);
      rotations = i/1000;
      Serial.println(rotations);
               
    }
}
