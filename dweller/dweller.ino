#include <SPI.h>
#include <MFRC522.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>  // Wire library - used for I2C communication

#include <RGBLed.h>
#include <Buzzer.h>
#include <Lid.h>
#include <LDR.h>

#define RED_RGB 6
#define GREEN_RGB 3
#define BLUE_RGB 5
#define BUZZER 4
#define LID A0
#define LDR0 A1
#define LDR1 A2
#define LDR2 A3
#define LDR_SENSITIVITY 650
#define RST_PIN  9
#define SS_PIN 10
#define CE 7
#define CSN 8

RGBLed led(RED_RGB, GREEN_RGB, BLUE_RGB);
Buzzer buzzer(BUZZER);
LDR ldr(LDR0, LDR1, LDR2, LDR_SENSITIVITY);
Lid lid(LID, 200);

int ADXL345 = 0x53; 
float X, Y, Z;  
double init_acc;

bool isUnlocked = false;
bool isOpened = false;
bool isAlarmOff = false;
int boardID = 1;
String transmitType = "PING";
int alarmTime = 20;

RF24 radio(CE, CSN);
const byte address[6] = "00001"; 

MFRC522 rfid(SS_PIN, RST_PIN);
byte ID[4] = {154, 248, 194, 21};


void setup() {
  Serial.begin(9600);
  SPI.begin();
  
  rfid.PCD_Init();
  Wire.begin();
  Wire.beginTransmission(ADXL345);
  Wire.write(0x2D);
  Wire.write(8);
  Wire.endTransmission();
  delay(10);

  // buzzer.openningMusic();

  led.ledColor(255, 255, 255);
  while(!isUnlocked)
    readNfc();

  init_acc = readAccelerometer();
}


void loop() {
  readNfc();
  
  if(isAlarmOff) 
  {
    alarm();
  }
  else
  {
    transmitData();
    if(!isUnlocked)
    {
      if(isMoved())
      {
        alarmTurnedOff("ACCELEROMETER TRIGGERED");
      }
      else if(ldr.isTriggered())
      {
        alarmTurnedOff("LDR TRIGGERED");
      }
      else if(lid.isTriggered())
      {
        alarmTurnedOff("LID TRIGGERED");
      }
    }
    else
    {
      if(lid.isTriggered())
      {
        Serial.println("triggered");
        isOpened = true;
        led.ledColor(0, 120, 255);
        delay(125);
        led.ledColor(0, 0, 0);
        delay(100);
      }
      else if(!lid.isTriggered() && isOpened)
      {
        buzzer.lockSound();
        led.ledColor(0, 0, 0);
        isUnlocked = false;
        isOpened = false;
        transmitType = "PING";
        init_acc = readAccelerometer();
      }
    }
  }
}


void readNfc() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (rfid.uid.uidByte[0] == ID[0] && rfid.uid.uidByte[1] == ID[1] && rfid.uid.uidByte[2] == ID[2] && rfid.uid.uidByte[3] == ID[3])
    {
      led.ledColor(0, 255, 0);
      isUnlocked = true;
      buzzer.unlockSound();
      delay(300);
      isAlarmOff = false;
      Serial.println("unlocked");
    } 
    else 
    {
      isUnlocked = false;
    }
  }
  rfid.PICC_HaltA();
}


bool alarmTurnedOff(String alarm) {
  isAlarmOff = true;
  transmitType = alarm;
}

void transmitData() {
  String text = getTransmitData();
  Serial.println(text);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
  radio.write(&text, sizeof(text));
}

void alarm() {
  transmitData();
  
  led.ledColor(255, 0, 0);
  buzzer.alarmSound(0.25);
  led.ledColor(0, 0, 0);
  buzzer.alarmSound(0.25);
  
  if(alarmTime == 0)
  {
    init_acc = readAccelerometer();
    alarmTime = 20;
  }
  alarmTime = alarmTime - 1;
}


String getTransmitData() {
  readAccelerometer();
  ldr.isTriggered();

  String isOpen;
  String hasLight;
  String isMoving;
  
  if(lid.isTriggered()) 
  {
    isOpen = "true";
  }
  else
  {
    isOpen = "false";
  }

  if(ldr.isTriggered()) 
  {
    hasLight = "true";
  }
  else
  {
    hasLight = "false";
  }

  if(isMoved()) 
  {
    isMoving = "true";
  }
  else
  {
    isMoving = "false";
  }
  
  String json = "{\n\t\"device_id\": " + String(boardID) + ",\n\t\"transmit_type\": " + "\"" + transmitType + "\",\n\t\"is_open\": " + isOpen + ",\n\t\"is_moving\": " + isMoving + ",\n\t\"has_light\": " + hasLight + ",\n\t\"sensor\": {\n\t\t\"ldr\": [" + String(ldr.ldr0_value) + ", " + String(ldr.ldr1_value) + ", " + String(ldr.ldr2_value) + "],\n\t\t\"accelerometer\": [" + String(X) + ", " + String(Y) + ", " + String(Z) + "]\n\t}\n}";
  
  return json;
  
}


bool isMoved()
{
    double acc = readAccelerometer();
    double threshold = 0.05;
    
    if(acc > init_acc + threshold)
    {
      // Serial.println("true");
      return true;
    }
    return false;

}


double readAccelerometer()
{
    Wire.beginTransmission(ADXL345);
    Wire.write(0x32);
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345, 6, true);
    X = ( Wire.read()| Wire.read() << 8); // X-axis value
    X = X/256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
    Y = ( Wire.read()| Wire.read() << 8); // Y-axis value
    Y = Y/256;
    Z = ( Wire.read()| Wire.read() << 8); // Z-axis value
    Z = Z/256;

    double X_ = (X * X);
    double Y_ = (Y * Y);
    double Z_ = (Z * Z);
    double acc = sqrt(X_ + Y_ + Z_);
              
    Wire.endTransmission();

    return acc;
}
