#include <Arduino.h>
#include <SoftwareSerial.h>
#include "sml.h"

//EMH eHZ-K
SoftwareSerial portOne(D7, D8);

//Iskra MT681
SoftwareSerial portTwo(D5, D6);

sml_states_t curStatePortOne;

#define MAX_STR_MANUF 5
unsigned char manuf[MAX_STR_MANUF];
double T1Wh = -2;
double SumWh = -2;

typedef struct
{
  const unsigned char OBIS[6];
  void (*Handler)();
} OBISHandler;

void Manufacturer()
{
  smlOBISManufacturer(manuf, MAX_STR_MANUF);
}

void PowerT1()
{
  smlOBISWh(T1Wh);
}

void PowerSum()
{
  smlOBISWh(SumWh);
}

OBISHandler OBISHandlers[] = 
{
    {{0x81, 0x81, 0xc7, 0x82, 0x03, 0xff}, &Manufacturer}, /* 129-129:199.130.3*255 */
    {{0x01, 0x00, 0x01, 0x08, 0x01, 0xff}, &PowerT1},      /*   1-  0:  1.  8.1*255 (T1) */
    {{0x01, 0x00, 0x01, 0x08, 0x00, 0xff}, &PowerSum},     /*   1-  0:  1.  8.0*255 (T1 + T2) */
    {{0}, 0}
};

void HandleByte (unsigned char currentChar, sml_states_t& curState)
{
  unsigned int iHandler = 0;
  curState = smlState(currentChar);

  if (curState == SML_START) 
  {//reset local vars
      manuf[0] = 0; 
      T1Wh = -3; 
      SumWh = -3;
  }
  else if (curState == SML_LISTEND)
  {//check handlers on last received list
    for (iHandler = 0; OBISHandlers[iHandler].Handler != 0 && !(smlOBISCheck(OBISHandlers[iHandler].OBIS)); iHandler++);

    if (OBISHandlers[iHandler].Handler != 0)
    {
      OBISHandlers[iHandler].Handler();
    }
  }
  else if (curState == SML_FINAL)
  {
    Serial.println(">>> Successfully received a complete message!");

    /*char buff[256];
    sprintf(buff, "manu: %s, T1Wh: %.2f, SumWh: %.2f", manuf, T1Wh, SumWh);
    Serial.println(buff);*/
    printf(">>> Manufacturer.............: %s\n", manuf);
    printf(">>> Power T1    (1-0:1.8.1)..: %.3f kWh\n", T1Wh);
    printf(">>> Power T1+T2 (1-0:1.8.0)..: %.3f kWh\n\n", SumWh);

  }
  else if (curState == SML_CHECKSUM_ERROR)
  {
    Serial.println(">>> Checksum error.");
  }
  else if (curState == SML_UNEXPECTED)
  {
    Serial.println(">>> Unexpected byte!");
  }
  
}

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
  while (portOne.available() > 0)
  {
    HandleByte(portOne.read(), curStatePortOne);
  }
}