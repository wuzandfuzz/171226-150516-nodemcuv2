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
#include <TimeLib.h>


const char* ssid     = "DOOMFORTRESS_24";
const char* password = "kimokimo84";
bool connected = 0;
unsigned int localPort = 2390;
/* Don't hardwire the IP address or we won't get the benefits of the pool.
*  Lookup the IP address for the host name instead */
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP udp; // A UDP instance to let us send and receive packets over UDP

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

uint16_t currentScreen[29];
uint16_t bufferScreen[29];
unsigned long lastretrieved = 0;
unsigned long currTime = 0;

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
  rowData[0] =  {1, B00000001, B00000000, B00000001, B00000000}, //0 not used
  rowData[1] =  {3, B00000000, B00000100, B00000000, B00000010},
  rowData[2] =  {1, B00010000, B00000000, B00001000, B00000000},
  rowData[3] =  {3, B00000000, B00000001, B10000000, B00000000},
  rowData[4] =  {1, B01000000, B00000000, B00100000, B00000000},
  rowData[5] =  {3, B01000000, B00000000, B00100000, B00000000},
  rowData[6] =  {1, B00000000, B00000001, B10000000, B00000000},
  rowData[7] =  {3, B00010000, B00000000, B00001000, B00000000},
  rowData[8] =  {1, B00000000, B00000100, B00000000, B00000010},
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
  
  colData[1] =  {4, B01000000, B00000000, B00100000, B00000000},
  colData[2] =  {4, B00010000, B00000000, B00001000, B11000000},
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
int countOnes(int number)
{
  int count=0;
     if ((number - 1)%10 == 0){
      count = count + 1;
     } 
  
     if (number - (number%10)==10){
      count = count +1;
     }  
  return count;
}
void resChip(int chip)
{
  sendStuff(chip, RESETBYTE, ZEROES);
}

void testwriteBit(int col, int row, int highlow) //fix performance here
{
  if (col > 0 && col<29 && row > 0 && row <29) { 
  int fliptime = 200;  //100us for safety, as low as 50us works
  int capRecoverDel = 5000; //100us to let the caps recover

  digitalWrite(EN, HIGH); //setup
  delay(2);  //this is really the rate limiter

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
  else {
    Serial.print("Error in ColRow Call");
    Serial.print("Col: ");
    Serial.print(col);
    Serial.print(" Row: ");
    Serial.println(row);
  }
}

void serialscreenWrite(uint16_t scrFrame[])
{
  uint16_t buffer = 0;
  const uint16_t bitmaskArray[17] = 
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
      Serial.print(" "); //just for formatting
    }
    Serial.print(":");
    for (int j=1; j<29; j++){
      buffer = bitmaskArray[i]&scrFrame[j];
      if (buffer == bitmaskArray[i]){
        Serial.print("#");
        }
      else{
        Serial.print(".");
      }
    }
  Serial.print('\n');
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

void resetScreen(){
 for (int i=1; i<29; i++){
  for (int j=16; j>0; j--){
      testwriteBit(i, j, 0);
    }
  }
  delay(20);
}

void writeScreen(uint16_t writeFrame[]){
  uint16_t buffer;
  const uint16_t bitmaskArray[17] = 
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
  for (int i=1; i<29; i++){
    for (int j=16; j>0; j--){
      buffer = bitmaskArray[j]&writeFrame[i];
      if (bitmaskArray[j] == buffer){
        testwriteBit(i,j,1);
        }
      else{
        testwriteBit(i,j,0);
      }
    }
  }
  memcpy(currentScreen, writeFrame, sizeof(currentScreen));
  Serial.println("finished write to panel");
}

void writeDifference(uint16_t writeFrame[]){
  uint16_t buffer;
  const uint16_t bitmaskArray[17] = 
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
  for (int i=1; i<29; i++){
    for (int j=16; j>0; j--){
      buffer = bitmaskArray[j]&writeFrame[i];
      if (bitmaskArray[j] == buffer){
        testwriteBit(i,j,1);
        }
      else{
        testwriteBit(i,j,0);
      }
    }
  }
  memcpy(currentScreen, writeFrame, sizeof(currentScreen));
  Serial.println("finished write to panel");
}


int placeChar(int request, int startPos){
  int chEntry[12][3] {
    {0, 0, 6}, //0, {characterid, startpoint, number of bytes in char of interest}
    {1, 6, 3}, //1
    {2, 9, 6}, //2
    {3, 15, 6},
    {4, 21, 6},
    {5, 27, 6},
    {6, 33, 6},
    {7, 39, 6},
    {8, 45, 6},
    {9, 51, 6},
    {10, 57,1}, //blank
    {11, 58,1}, //:
  };

  const uint16_t bmpFile[59]{ 
    0b0011111000000000, //0, starts at [0]
    0b0111111100000000,
    0b0100000100000000,
    0b0100000100000000,
    0b0111111100000000,
    0b0011111000000000, //0, ends at [5]

    0b0010000000000000, //1
    0b0111111100000000,
    0b0111111100000000,

    0b0011001100000000, //2
    0b0110011100000000,
    0b0100111100000000,
    0b0100100100000000,
    0b0111100100000000,
    0b0011000100000000,

    0b0010001000000000, //3
    0b0110001100000000,
    0b0100100100000000,
    0b0100100100000000,
    0b0111111100000000,
    0b0011011000000000,

    0b0000010000000000,
    0b0000110000000000,
    0b0001010000000000,
    0b0111111100000000,
    0b0111111100000000,
    0b0000010000000000,

    0b0111100100000000,
    0b0111100100000000,
    0b0100100100000000,
    0b0100100100000000,
    0b0100111100000000,
    0b0100011000000000,

    0b0001111000000000,
    0b0011111100000000,
    0b0110100100000000,
    0b0100100100000000,
    0b0100111100000000,
    0b0000011000000000,

    0b0100000000000000,
    0b0100000000000000,
    0b0100011100000000,
    0b0101111100000000,
    0b0111100000000000,
    0b0110000000000000,

    0b0011011000000000,
    0b0111111100000000,
    0b0100100100000000,
    0b0100100100000000,
    0b0111111100000000,
    0b0011011000000000,

    0b0011000000000000,
    0b0111100100000000,
    0b0100100100000000,
    0b0100101100000000,
    0b0111111000000000,
    0b0011110000000000,

    0b0000000000000000, //blank, 10

    0b0010001000000000, //:, 11
  };

  for (int i = 0; i < chEntry[request][2]; i++){
    int buffercoord = startPos + i;
    int bmpcoord = i + chEntry[request][1];
    bufferScreen[buffercoord] = bmpFile[bmpcoord];
  }
  return (startPos + chEntry[request][2]);
}

void composeScreenTime(int hourIn, int minuteIn){
  
  if (hourIn > 12){
    hourIn = 0;
  } 

  int scrPointer = 1;
  if (hourIn>9){
    scrPointer = placeChar(1,2);
    scrPointer = placeChar(10,scrPointer);
    scrPointer = placeChar((hourIn%10), scrPointer);
    scrPointer = placeChar(10,scrPointer);
  }
  else {
    scrPointer = placeChar(hourIn, 2);
    scrPointer = placeChar(10,scrPointer);
  }
  scrPointer = placeChar(11,scrPointer);
  scrPointer = placeChar(10,scrPointer);

  if (minuteIn<10){
    scrPointer = placeChar(0,scrPointer);
    scrPointer = placeChar(10,scrPointer);
    scrPointer = placeChar(minuteIn, scrPointer);
  }
  else{
    scrPointer = placeChar(((minuteIn-(minuteIn%10))/10), scrPointer);
    scrPointer = placeChar(10,scrPointer);
    scrPointer = placeChar((minuteIn%10), scrPointer);
  }

  for (int i = scrPointer; i<29; i++){
    scrPointer = placeChar(10, scrPointer);
  }
}

void getTime()
//gets time and converts to PST
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(5000);
  
  //tries to connect for appx 30 sec. 
  for (int i=1; i<10; i++) {
    if (WiFi.status() == WL_CONNECTED){
      connected = 1;
      Serial.println("");
      Serial.println("WiFi connected");  
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
    }
    else {
      delay(500);
      Serial.print(".");
    }
  }
  if (connected == 1){
    //now gets time if connected.  if couldn't connect, continues business as usual
    Serial.println("Starting UDP");
    udp.begin(localPort);
    Serial.print("Local port: ");
    Serial.println(udp.localPort());
    //get a random server from the pool
    WiFi.hostByName(ntpServerName, timeServerIP); 
    sendNTPpacket(timeServerIP); // send an NTP packet to a time server
    // wait to see if a reply is available
    delay(1000);

    int cb = udp.parsePacket();
    if (!cb) {
      Serial.println("no packet yet");
    }
    else {
      Serial.print("packet received, length=");
      Serial.println(cb);
      // We've received a packet, read the data from it
      udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
      //the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, esxtract the two words:

      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      Serial.print("Seconds since Jan 1 1900 = " );
      Serial.println(secsSince1900);

      // now convert NTP time into everyday time:
      Serial.print("Unix time = ");
      // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
      const unsigned long seventyYears = 2208988800UL;
      // subtract seventy years:
      unsigned long epoch = secsSince1900 - seventyYears;
      // print Unix time:
      Serial.println(epoch);
      setTime(epoch);
      adjustTime(-28800);  //subtracts 8 hr to get to PST
      Serial.print(hour());
      Serial.print(":");
      Serial.print(minute());
      /*
      //print the hour, minute and second:
      Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
      Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
      Serial.print(':');
      if ( ((epoch % 3600) / 60) < 10 ) {
        // In the first 10 minutes of each hour, we'll want a leading '0'
        Serial.print('0');
      }
      Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
      Serial.print(':');
      if ( (epoch % 60) < 10 ) {
        // In the first 10 seconds of each minute, we'll want a leading '0'
        Serial.print('0');
      }
      Serial.println(epoch % 60); // print the second
      */

    }
    // wait ten seconds before asking for the time again
    //delay(10000);
  }
  else {
    Serial.println("No Connection Available, stop");
  }
  WiFi.mode(WIFI_OFF);
}
  
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void setup()
{
  int wiFlag=0;
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

  for (int i = 0; i<29; i++) //resets the current screen state buffer
  {
    currentScreen[i] = 0b0000000000000000;
  }
  //more boring stuff that works
  {  
    getTime();
    Serial.print(hour());
    Serial.print(":");
    Serial.print(minute());
    Serial.print(":");
    Serial.print(second()); 
  }

  Serial.println("Writing Current Screen");
  serialscreenWrite(currentScreen);
  composeScreenTime(hourFormat12(), minute());
  Serial.println("Writing Buffer");
  serialscreenWrite(bufferScreen);
  writeScreen(bufferScreen);

}

void loop()
{
  

  composeScreenTime(hourFormat12(), minute());
  int writeFlag = 0;
  
  for (int i = 1; i<29; i++){
    if (bufferScreen[i] != currentScreen[i]){
      writeFlag = 1;
    }
  }
  
  if (writeFlag == 1){
    /*resetScreen();
    writeScreen(bufferScreen);
    */
    writeDifference(bufferScreen);
  }
  else {
  }

 
}
