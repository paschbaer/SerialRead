#include <Arduino.h>
#include <SoftwareSerial.h>

//EMH eHZ-K
SoftwareSerial portOne(D7, D8);

//Iskra MT681
SoftwareSerial portTwo(D5, D6);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);   

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\r\n");

  portOne.begin(9600);

  portTwo.begin(9600);
}

void loop()
{
  char buff[512];

  Serial.println("\r\nListening on port one");
  portOne.listen();
  int availOne = portOne.available();
  while (availOne > 0)
  {
    Serial.print(availOne);
    Serial.println(" byte(s) of data available on port one:");
    int i = 0;
    for (; availOne > 0; availOne--)
    {
      sprintf(buff + (3 * i++), "%.02X ", portOne.read());
    }
    Serial.println(buff);

    delay(100);
    availOne = portOne.available();
  }

  delay(100);

  /*delay(500);

  Serial.println("\r\nListening on port two");
  portTwo.listen();
  int availTwo = portTwo.available();
  while (availTwo > 0)
  {
    Serial.print(availTwo);
    Serial.println(" byte(s) of data available on port two:");
    int i = 0;
    for (; availTwo > 0; availTwo--)
    {
      sprintf(buff + (3 * i++), "%.02X ", portTwo.read());
    }
    Serial.println(buff);
  }

  delay(500);*/

}