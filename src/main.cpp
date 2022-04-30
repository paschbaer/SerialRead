#include <Arduino.h>
#include <SoftwareSerial.h>
#include "SmlReader.h"

//EMH eHZ-K
SoftwareSerial portOne(D7, D8);

//Iskra MT681
SoftwareSerial portTwo(D5, D6);

sml_states_t curStatePortOne;
sml_states_t curStatePortTwo;

#define MAX_STR_MANUF 5
unsigned char manuf[MAX_STR_MANUF];
double T1Wh = -2;
double T2Wh = -2;
double SumWh = -2;
double CurW = -2;

typedef struct
{
  const unsigned char OBIS[6];
  void (*Handler)();
} OBISHandler;



OBISHandler OBISHandlers[] = 
{
    {{0x81, 0x81, 0xc7, 0x82, 0x03, 0xff}, &Manufacturer}, /* 129-129:199.130.3*255 */
    {{0x01, 0x00, 0x01, 0x08, 0x01, 0xff}, &PowerT1},      /*   1-  0:  1.  8.1*255 (T1) */
    {{0x01, 0x00, 0x01, 0x08, 0x02, 0xff}, &PowerT2},      /*   1-  0:  2.  8.1*255 (T2) */
    {{0x01, 0x00, 0x01, 0x08, 0x00, 0xff}, &PowerSum},     /*   1-  0:  1.  8.0*255 (T1 + T2) */
    {{0x01, 0x00, 0x10, 0x00, 0x07, 0xff}, &EnergyCur},    /*   1-  0:  10.  0.7*255 (T1 + T2) */
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
      T2Wh = -3;
      SumWh = -3;
      CurW = -3;
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
    printf(">>> Power T2    (1-0:1.8.2)..: %.3f kWh\n", T2Wh);
    printf(">>> Power T1+T2 (1-0:1.8.0)..: %.3f kWh\n", SumWh);
    printf(">>> Energy      (1-0:10.7.0)..: %.3f kW\n\n", CurW);

  }
  else if (curState == SML_CHECKSUM_ERROR)
  {
    Serial.println(">>> Checksum error.");
  }
  else if (curState == SML_UNEXPECTED)
  {
    //Serial.println(">>> Unexpected byte!");
    char buff[4];
    sprintf(buff, "%02X ", currentChar);
    Serial.print(buff);
  }
  
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);   

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\r\n");

  portOne.begin(9600);

  //portTwo.begin(9600);
}

void loop()
{
  bool bDelay = true;

  portOne.listen();
  Serial.println("\r\nportOne >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  while (portOne.available() > 0)
  {
    HandleByte(portOne.read(), curStatePortOne);
    bDelay = false;
  }

  /*portTwo.listen();
  Serial.println("\r\nportTwo >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  while (portTwo.available() > 0)
  {
    HandleByte(portTwo.read(), curStatePortTwo);
    bDelay = false;
  }*/

  if (bDelay)
    delay(500);
}