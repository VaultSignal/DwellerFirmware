#include <SPI.h>
#include <MFRC522.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RGBLed.h>
#include <Buzzer.h>
#include <Lid.h>
#include <LDR.h>

#include <Wire.h>  // Wire library - used for I2C communication

int ADXL345 = 0x53; // The ADXL345 sensor I2C address

float X, Y, Z;  // Outputs

double init_acc;
double MAX_VALUE = 2.0;
double MIN_VALUE = 0.0;
double RANGE = 0.1;

#define RED_RGB 6
#define GREEN_RGB 3
#define BLUE_RGB 5
#define BUZZER 4

#define LID A0
#define LDR0 A1
#define LDR1 A2
#define LDR2 A3
#define LDR_SENSITIVITY 990
#define RST_PIN  9
#define SS_PIN 10
#define CE 7
#define CSN 8

RGBLed led(RED_RGB, GREEN_RGB, BLUE_RGB);
Buzzer buzzer(BUZZER);

LDR ldr(LDR0, LDR1, LDR2, 1000);
Lid lid(LID, 200);
// Accelerometer accelerometer;

RF24 radio(CE, CSN); // CE, CSN
const byte address[6] = "00001"; // the address the the module

bool isUnlocked = false;
bool isOpened = false;
bool isAlarmOff = false;
String alarm = "";
int boardID = 1;

MFRC522 rfid(SS_PIN, RST_PIN);
byte ID[4] = {154, 248, 194, 21};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(10);

  init_acc = readAccelerometer();
  //Serial.println(init_acc);
  
  led.ledColor(255, 255, 255);
}

void loop() {  
  isMoved(); 
  // Read NFC card
  // Serial.println(accelerometer.readAccelerometer()[1]);
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    if (rfid.uid.uidByte[0] == ID[0] && rfid.uid.uidByte[1] == ID[1] && rfid.uid.uidByte[2] == ID[2] && rfid.uid.uidByte[3] == ID[3])
    {
      led.ledColor(0, 255, 0);
      isUnlocked = true;
      buzzer.unlockSound(); 
      led.ledColor(0, 255, 0);
      isAlarmOff = false;
      Serial.println("unlocked");
    } 
    else 
    {
      isUnlocked = false;
    }
  }

  
  if(isAlarmOff) 
  {
    alarmTurnedOff();
  }
  else
  {
    if(isMoved())
    {
      if(!isUnlocked) {
        isAlarmOff = true;
        alarm = "ACCELEROMETER";
      }
      Serial.println("moved");
    }
    else if(ldr.isTriggered())
    {
      if(!isUnlocked)
      {
        isAlarmOff = true;
        alarm = "LDR";
      }
      Serial.println("light");
    }
    else if(lid.isTriggered())
    {
      if(isUnlocked)
      {
        isOpened = true;
      }
      else
      {
        isAlarmOff = true;
        alarm = "LID";
      }
      Serial.println("lid opened");
    }
    else // Lock
    {
      if(isUnlocked && isOpened) 
      {
        isUnlocked = false;
        isOpened == false;
      }
    }
  }
  // RFID
  rfid.PICC_HaltA();
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
    // Serial.println("a:" + String(acc) + " " + String(X) + " " + String(Y) + " " + String(Z));
              
    Wire.endTransmission();

    return acc;
}

bool isMoved()
{
    double acc = readAccelerometer();
    double threshold = 0.05;
    
    if(acc > init_acc + threshold)
    {
      return true;
    }
    return false;
}

void alarmTurnedOff() {
  led.ledColor(255, 0, 0);
  buzzer.alarmSound(1);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
  String text = getTransmitData(); // you can customize this text to your wish
  radio.write(&text, sizeof(text));
}

/*
 * {
 *    "id": 1,
 *    "alarm": "LID",
 *    "sensor": {
 *      "lid": false,
 *      "ldr": [400, 350, 380],
 *      "accelerometer": [-0.08, 0.12, 0.02]
 *    }
 *  }
 */
String getTransmitData() {
  readAccelerometer();
  ldr.isTriggered();

  String lid_str;
  
  if(lid.isTriggered()) 
  {
    lid_str = "true";
  }
  else
  {
    lid_str = "false";
  }

  return "{ \"id\":" + String(boardID) + ", \"alar\": " + alarm + ", \"sensor\": { \"lid\": " + lid_str + ", \"ldr\": [" + String(ldr.ldr0_value) + ", " + String(ldr.ldr1_value) + ", " + String(ldr.ldr2_value) + "], \"accelerometer\": [" + String(X) + ", " + String(Y) + ", " + String(Z) + "] } }";
  
}
