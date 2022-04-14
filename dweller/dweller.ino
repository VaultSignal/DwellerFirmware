#include <SPI.h>
#include <MFRC522.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h> // Wire library - used for I2C communication

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
#define RST_PIN 9
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
byte boardID = 1;
String transmitType = "PING";
int alarmTime = 20;

RF24 radio(CE, CSN);
uint64_t address = 0;

MFRC522 rfid(SS_PIN, RST_PIN);
byte ID[2][4] = {{208, 150, 247, 137}, {241, 173, 251, 137}};

void setup()
{
  Serial.begin(9600);
  SPI.begin();

  rfid.PCD_Init();
  Wire.begin();
  Wire.beginTransmission(ADXL345);
  Wire.write(0x2D);
  Wire.write(8);
  Wire.endTransmission();
  delay(10);

  led.ledColor(255, 255, 255);
  while (!isUnlocked)
    readNfc();

  init_acc = readAccelerometer();
}

void loop()
{
  readNfc();

  if (isAlarmOff)
  {
    alarm();
  }
  else
  {
    transmitData();
    if (!isUnlocked)
    {
      if (isMoved())
      {
        alarmTurnedOff("ACCELEROMETER TRIGGERED");
      }
      else if (ldr.isTriggered())
      {
        alarmTurnedOff("LDR TRIGGERED");
      }
      else if (lid.isTriggered())
      {
        alarmTurnedOff("LID TRIGGERED");
      }
    }
    else
    {
      if (lid.isTriggered())
      {
        Serial.println("triggered");
        isOpened = true;
        led.ledColor(0, 120, 255);
        delay(125);
        led.ledColor(0, 0, 0);
        delay(100);
      }
      else if (!lid.isTriggered() && isOpened)
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

void readNfc()
{
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
  {
    for (int i = 0; i < 2; i++)
    {
      if (rfid.uid.uidByte[0] == ID[i][0] && rfid.uid.uidByte[1] == ID[i][1] && rfid.uid.uidByte[2] == ID[i][2] && rfid.uid.uidByte[3] == ID[i][3])
      {
        led.ledColor(0, 255, 0);
        isUnlocked = true;
        buzzer.unlockSound();
        delay(300);
        isAlarmOff = false;
        Serial.println("unlocked");
        rfid.PICC_HaltA();
        return;
      }
    }
    isUnlocked = false;
    led.ledColor(255, 0, 0);
    buzzer.wrongCard();
    led.ledColor(0, 0, 0);
    rfid.PICC_HaltA();
  }
}

bool alarmTurnedOff(String alarm)
{
  isAlarmOff = true;
  transmitType = alarm;
}

void transmitData()
{
  byte *payload = getTransmitData();
  Serial.println(payload[1]);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
  radio.write(payload, sizeof(payload));
  free(payload);
}

void alarm()
{
  transmitData();

  led.ledColor(255, 0, 0);
  buzzer.alarmSound(0.25);
  led.ledColor(0, 0, 0);
  buzzer.alarmSound(0.25);

  if (alarmTime == 0)
  {
    init_acc = readAccelerometer();
    alarmTime = 20;
  }
  alarmTime = alarmTime - 1;
}

byte *getTransmitData()
{
  readAccelerometer();
  ldr.isTriggered();

  bool isOpen = lid.isTriggered();
  bool hasLight = ldr.isTriggered();
  bool isMoving = isMoved();

  byte openByte = isOpen ? 1 : 0;
  byte movingByte = isMoving ? 1 : 0;
  byte lightByte = hasLight ? 1 : 0;

  // Convert the X, Y and Z accelorometer values
  // to the bytes. A float should take 4 bytes.
  byte *x = reinterpret_cast<byte *>(&X);
  byte *y = reinterpret_cast<byte *>(&Y);
  byte *z = reinterpret_cast<byte *>(&Z);

  // Now convert to LDR values to bytes,
  // An int should take 4 bytes as well.
  byte *ldr_0 = reinterpret_cast<byte *>(&ldr.ldr0_value);
  byte *ldr_1 = reinterpret_cast<byte *>(&ldr.ldr1_value);
  byte *ldr_2 = reinterpret_cast<byte *>(&ldr.ldr2_value);

  byte payload[] = {
      boardID,
      openByte,
      movingByte,
      lightByte,
      x[0],
      x[1],
      x[2],
      x[3],
      y[0],
      y[1],
      y[2],
      y[3],
      z[0],
      z[1],
      z[2],
      z[3],
      ldr_0[0],
      ldr_0[1],
      ldr_0[2],
      ldr_0[3],
      ldr_1[0],
      ldr_1[1],
      ldr_1[2],
      ldr_1[3],
      ldr_2[0],
      ldr_2[1],
      ldr_2[2],
      ldr_2[3],
  };
  byte *payload_ptr = (byte *)malloc(28 * sizeof(byte));
  memcpy(payload_ptr, payload, 28);
  return payload_ptr;
}

bool isMoved()
{
  double acc = readAccelerometer();
  double threshold = 0.05;

  if (acc > init_acc + threshold)
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
  X = (Wire.read() | Wire.read() << 8); // X-axis value
  X = X / 256;                          // For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y = (Wire.read() | Wire.read() << 8); // Y-axis value
  Y = Y / 256;
  Z = (Wire.read() | Wire.read() << 8); // Z-axis value
  Z = Z / 256;

  double X_ = (X * X);
  double Y_ = (Y * Y);
  double Z_ = (Z * Z);
  double acc = sqrt(X_ + Y_ + Z_);

  Wire.endTransmission();

  return acc;
}
