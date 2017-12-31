#include <ESP8266WiFi.h>
//#include <ESP8266WiFiAP.h>
//#include <ESP8266WiFiGeneric.h>
//#include <ESP8266WiFiMulti.h>
//#include <ESP8266WiFiScan.h>
//#include <ESP8266WiFiSTA.h>
//#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
//#include <WiFiClientSecure.h>
//#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <dummy.h>


#include <SPI.h>

const char* ssid     = "DOOMFORTRESS_24";
const char* password = "kimokimo84";

const byte TEST1 = B00010000; 
const byte TEST2 = B00000000;
const byte ZEROES = B00000000;
const byte RESETBYTE = B00000001;
const int CS = 15; 
const int EN = 16;
const int SEL1 = 5;
const int SEL2 = 4;
const int SEL3 = 0;
const int SEL4 = 2;

short currentScreen[29];
int currTime = 0;

uint8_t defarray[9][4] = 
{
  {0,0,0,0}, //0 not used
  {1,0,0,0}, //1
  {0,1,0,0}, //2
  {1,1,0,0}, //3
  {0,0,1,0}, //4
  {1,0,1,0}, //5
  {0,1,1,0}, //6
  {1,1,1,0}, //7
  {0,0,0,1}, //8
};

byte hpdefarray[9] = 
{
  B11000011, //reset
  B00100000,
  B00010000, //2
  B00110000, //3
  B00001000, //4
  B00101000, //5
  B00011000, //6
  B00111000, //7
  B00000100, //8

  
};

struct indexEntry
{
  int chip;
  byte highfirstcommand; //to send high
  byte highsecondcommand; //to send high
  byte lowfirstcommand;  //to send low
  byte lowsecondcommand; //to send low
  
};

indexEntry rowData[17] =
{
  rowData[0] =  {1, B00000001, B11100000, B00000001, B00000000}, //0 not used
  rowData[1] =  {3, B00000000, B00000100, B00000000, B00000010},
  rowData[2] =  {1, B00010000, B11000000, B00001000, B11000000},
  rowData[3] =  {3, B00000000, B11100001, B10000000, B00000000},
  rowData[4] =  {1, B01000000, B11100000, B00100000, B00000000},
  rowData[5] =  {3, B01000000, B11100000, B00100000, B00000000},
  rowData[6] =  {1, B00000000, B11100001, B10000000, B00000000},
  rowData[7] =  {3, B00010000, B11100000, B00001000, B00000000},
  rowData[8] =  {1, B00000000, B11100100, B00000000, B00000010},
  rowData[9] =  {2, B00000000, B00010000, B00000000, B00001000},
  rowData[10] = {1, B00000100, B00000000, B00000010, B00000000},
  rowData[11] = {2, B00000100, B00000000, B00000010, B00000000},
  rowData[12] = {1, B00000000, B00010000, B00000000, B00001000},
  rowData[13] = {2, B00000000, B00000100, B00000000, B00000010},
  rowData[14] = {2, B00010000, B00000000, B00001000, B00000000},
  rowData[15] = {2, B00000000, B00000001, B10000000, B00000000},
  rowData[16] = {2, B01000000, B00000000, B00100000, B00000000},
 
    
};

indexEntry colData[29] =
{
  colData[0] =  {4, B00000001, B00000000, B00000001, B00000000}, //0 not used
  
  colData[1] =  {4, B01000000, B11100000, B00100000, B00000000},
  colData[2] =  {4, B00010000, B11000000, B00001000, B11000000},
  colData[3] =  {4, B00000000, B00000100, B00000000, B00000010},
  colData[4] =  {4, B00000000, B00000001, B10000000, B00000000},
  colData[5] =  {4, B00000000, B00010000, B00000000, B00001000},
  colData[6] =  {4, B00000100, B00000000, B00000010, B00000000},
  
  colData[7] =  {5, B01000000, B00000000, B00100000, B00000000},
  colData[8] =  {5, B00010000, B00000000, B00001000, B00000000},
  colData[9] =  {5, B00000000, B00000100, B00000000, B00000010},
  colData[10] = {5, B00000000, B00000001, B10000000, B00000000},
  colData[11] = {5, B00000000, B00010000, B00000000, B00001000},
  colData[12] = {5, B00000100, B00000000, B00000010, B00000000},

  colData[13] = {6, B01000000, B00000000, B00100000, B00000000},
  colData[14] = {6, B00010000, B00000000, B00001000, B00000000},
  colData[15] = {6, B00000000, B00000100, B00000000, B00000010},
  colData[16] = {6, B00000000, B00000001, B10000000, B00000000},
  colData[17] = {6, B00000000, B00010000, B00000000, B00001000},
  colData[18] = {6, B00000100, B00000000, B00000010, B00000000},

  colData[19] = {7, B01000000, B00000000, B00100000, B00000000},
  colData[20] = {7, B00010000, B00000000, B00001000, B00000000},
  colData[21] = {7, B00000000, B00000100, B00000000, B00000010},
  colData[22] = {7, B00000000, B00000001, B10000000, B00000000},
  colData[23] = {7, B00000000, B00010000, B00000000, B00001000},
  colData[24] = {7, B00000100, B00000000, B00000010, B00000000},

  colData[25] = {8, B01000000, B00000000, B00100000, B00000000},
  colData[26] = {8, B00010000, B00000000, B00001000, B00000000},
  colData[27] = {8, B00000000, B00000100, B00000000, B00000010},
  colData[28] = {8, B00000000, B00000001, B10000000, B00000000},
};

void selectSlave(int chipno)
{
   digitalWrite(SEL1, defarray[chipno][0]);
   digitalWrite(SEL2, defarray[chipno][1]);
   digitalWrite(SEL3, defarray[chipno][2]);
   digitalWrite(SEL4, defarray[chipno][3]);   
}

/*void testMux(int targetaddress)
{
  Serial.print("Testing Address:");
  Serial.println(targetaddress);
  selectSlave(targetaddress);
  for (int i=0; i<20; i++){
    digitalWrite(CS, HIGH);
    delay(1);
    digitalWrite(CS, LOW);
    delay(1);
  }
  digitalWrite(CS, HIGH); //returns CS back to init
}*/

void hpsendStuff(int slaveNo1, byte first, byte second, int slaveNo2, byte third, byte fourth)
{
   
   byte firstbuffer = 0;
   byte secondbuffer = 0;
   byte thirdbuffer = 0;
   byte fourthbuffer = 0;

   SPI.beginTransaction(SPISettings(1000000, LSBFIRST, SPI_MODE1)); 
   
   selectSlave(slaveNo1);
   digitalWrite(CS, LOW);
   firstbuffer = SPI.transfer(first);
   secondbuffer = SPI.transfer(second);

   digitalWrite(CS, HIGH); //pulls cs high
   selectSlave(slaveNo2);
   digitalWrite(CS, LOW);   //pulls cs low   hpselectSlave(slaveNo2); //sets all the addresses on the mux
   thirdbuffer = SPI.transfer(third);
   fourthbuffer = SPI.transfer(fourth);
   digitalWrite(CS, HIGH); //pulls cs high
   
   SPI.endTransaction();
 
   Serial.print("OutputSlave_");
   Serial.print(slaveNo1);
   Serial.print(": ");
   Serial.print(firstbuffer, HEX);
   Serial.print("_");
   Serial.println(secondbuffer, HEX);
   Serial.print("OutputSlave_");
   Serial.print(slaveNo2);
   Serial.print(": ");
   Serial.print(thirdbuffer, HEX);
   Serial.print("_");
   Serial.println(fourthbuffer, HEX);

}

void sendStuff(int slaveNo, byte first, byte second)
{
   
   byte firstbuffer = B00000000;
   byte secondbuffer = B00000000;

   SPI.beginTransaction(SPISettings(100, LSBFIRST, SPI_MODE1)); 
   selectSlave(slaveNo); //sets all the addresses on the mux
   digitalWrite(CS, LOW); //wakes up the chip in question by telling the mux to wakeup
   firstbuffer = SPI.transfer(first);
   secondbuffer = SPI.transfer(second);  
   digitalWrite(CS, HIGH); //resets chip select to turn back off
   SPI.endTransaction();

   Serial.print("Output: ");
   Serial.print(firstbuffer, HEX);
   Serial.print("_");
   Serial.println(secondbuffer, HEX);
}

void resDrivers()
{
  for(int i=1; i<9; i++)
  {
    sendStuff(i, RESETBYTE, ZEROES);
    sendStuff(i, RESETBYTE, ZEROES);
  } 
}

void resChip(int chip)
{
  sendStuff(chip, RESETBYTE, ZEROES);
}

void testwriteBit(int col, int row, int highlow) //fix performance here
{
  int fliptime = 100;  //100us for safety, as low as 50us works
  int capRecoverDel = 100; //100us to let the caps recover

  digitalWrite(EN, HIGH); //setup
  //delay(2);  //this is really the rate limiter

  if (highlow == 1) 
  {
   hpsendStuff(colData[col].chip, colData[col].lowfirstcommand, colData[col].lowsecondcommand, 
               rowData[row].chip, rowData[row].highfirstcommand, rowData[row].highsecondcommand);
   hpsendStuff(colData[col].chip, colData[col].lowfirstcommand, colData[col].lowsecondcommand, 
               rowData[row].chip, rowData[row].highfirstcommand, rowData[row].highsecondcommand);
   delayMicroseconds(fliptime);
  }

  if (highlow == 0) 
  {
   hpsendStuff(rowData[row].chip, rowData[row].lowfirstcommand, rowData[row].lowsecondcommand, 
               colData[col].chip, colData[col].highfirstcommand, colData[col].highsecondcommand);
   hpsendStuff(rowData[row].chip, rowData[row].lowfirstcommand, rowData[row].lowsecondcommand, 
               colData[col].chip, colData[col].highfirstcommand, colData[col].highsecondcommand);
   delayMicroseconds(fliptime);
  }
  
  digitalWrite(EN, LOW);
  delayMicroseconds(capRecoverDel);
}
void serialscreenWrite()
{
  const short bitmaskArray[17] = 
  {
    0b0000000000000000, //not used, a way to blank stuff
    0b0000000000000001,
    0b0000000000000010,
    0b0000000000000100,
    0b0000000000001000,
    0b0000000000010000,
    0b0000000000100000,
    0b0000000001000000,
    0b0000000010000000,
    0b0000000100000000,
    0b0000001000000000,
    0b0000010000000000,
    0b0000100000000000,
    0b0001000000000000,
    0b0010000000000000,
    0b0100000000000000,
    0b1000000000000000,
  };
  Serial.println("Start Panel");
  for (int i=1; i<17; i++){
    Serial.print(i);
    if (i<10){
      Serial.print(" ");
    }
    Serial.print(":");
    for (int j=1; j<29; j++){
      if (bitmaskArray[i]&currentScreen[j] == 0){
        Serial.print(".");
        }
      else{
        Serial.print("#");
      }
    }
  }
}

void testpanelSweep()
{
 for (int i=1; i<29; i++){
  for (int j=16; j>0; j--){
      testwriteBit(i, j, 1);  
    }
  }
  delay(20);
 for (int i=1; i<29; i++){
  for (int j=16; j>0; j--){
      testwriteBit(i, j, 0);
    }
  }
  delay(20);
}

void writeScreen(short writeFrame[29]){
  const short bitmaskArray[17] = 
  {
    0b0000000000000000, //not used, a way to blank stuff
    0b0000000000000001,
    0b0000000000000010,
    0b0000000000000100,
    0b0000000000001000,
    0b0000000000010000,
    0b0000000000100000,
    0b0000000001000000,
    0b0000000010000000,
    0b0000000100000000,
    0b0000001000000000,
    0b0000010000000000,
    0b0000100000000000,
    0b0001000000000000,
    0b0010000000000000,
    0b0100000000000000,
    0b1000000000000000,
  };
  for (int i=1; i<17; i++){
    for (int j=1; j<29; j++){
      if (bitmaskArray[i]&writeFrame[j] == 0){
        testwriteBit(i,j,0);
        }
      else{
        testwriteBit(i,j,1);
      }
    }
  }
  memcpy(writeFrame, currentScreen, sizeof(currentScreen));
}

void setup()
{
    pinMode(CS,   OUTPUT);
    pinMode(EN,   OUTPUT);
    pinMode(SEL1, OUTPUT);
    pinMode(SEL2, OUTPUT);
    pinMode(SEL3, OUTPUT);
    pinMode(SEL4, OUTPUT);

    //initialize
    digitalWrite(EN,   LOW);
    digitalWrite(CS,   HIGH); //once you pull CS Low it allows you to select a chip
    digitalWrite(SEL1, HIGH);
    digitalWrite(SEL2, HIGH);
    digitalWrite(SEL3, HIGH);
    digitalWrite(SEL4, HIGH);

    Serial.begin(115200);
    delay(1000);
    SPI.begin();
    digitalWrite(EN, HIGH);
    delay(1);
    resDrivers();
    digitalWrite(EN, LOW);  

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  for (int i = 0; i<29; i++){
    currentScreen[i] = 0b0000000000000000;
  } 
}

void loop()
{
  writeScreen(currentScreen);

  /*  
  currTime = millis();
  Serial.print("Milliseconds since Boot");
  Serial.println(currTime);
  */
}
