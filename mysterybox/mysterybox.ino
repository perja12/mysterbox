#include <Servo.h>

Servo servo;

// The top secret pin codes
const int code_for_day[] = { 
  11,   // 1
  1210, // 2
  1064, // 3
  155,  // 4 
  15, // 5
  16, // 6 
  17, // 7 
  18, // 8 
  19, // 9
  20, // 10
  21, // 11
  22, // 12
  23, // 13
  24, // 14
  25, // 15
  26, // 16
  27, // 17
  28, // 18
  29, // 19
  30, // 20
  31, // 21
  32, // 22
  33, // 23
  34, // 24
  35, // 25
  36, // 26
  37, // 27
  38, // 28
  39, // 29
  40, // 30
  41  // 31
};

// Pins
const int servoPin = 1;
const int ledPin = 5;
const int outsideLedRed = 2;
const int outsideLedGreen = 3;

// Values for the servo
const int valLocked = 72;
const int valUnlocked = 106;

// The scratch bank to use
const int bank = 1;

static bool connected = false;
static bool locked = false;

void setup() {
  Bean.enableConfigSave(false);
  Bean.enableWakeOnConnect(true);
  Bean.setAdvertisingInterval(500);
  pinMode(ledPin, OUTPUT);
  pinMode(outsideLedRed, OUTPUT);
  pinMode(outsideLedGreen, OUTPUT);

  blinkOutsideLed(3, outsideLedGreen);

  // Lock the box when powering on
  lock(true);
}

void loop() {
  Bean.setLed(0, 0, 255);
  connected = Bean.getConnectionState();
  
  if (!connected) {
    // Turn off led and go to sleep if there is no active connection.
    digitalWrite(ledPin, LOW);
    Bean.sleep(0xffffffff);
  } else {
    // There is an active connection: 
    // 1) Read from scratch 1 (0 = lock, pin=unlock)
    // 2) Sleep 100 and then goto 1 until a disconnect.
    digitalWrite(ledPin, HIGH);
    long state = Bean.readScratchNumber(bank);
    
  
    if (state == 0 && !locked) {
      lock(true);
      blinkOutsideLed(1, outsideLedGreen);
    } else if (locked) {
      byte day = state >> 24 ;
      state = state & 0x00FFFFFF;

      // Get the pin code for the selected day.
      int pinCode = 1337;
      if (day > -1 && day < sizeof(code_for_day) / sizeof( int )) {
        pinCode = code_for_day[day - 1]; 
      }
      
      if (state == pinCode) {
        lock(false);
        blinkOutsideLed(3, outsideLedGreen);
        // Hide the pin code so that the next client can't read it
        //Bean.setScratchNumber(bank, 1);
      } else if (state != 1) {
          blinkOutsideLed(3, outsideLedRed);
          //Bean.setScratchNumber(bank, 1);
      }
    }
  }
  Bean.setScratchNumber(bank, 1);
  Bean.sleep(100);
}

void lock(bool lock) {
  servo.attach(servoPin);
  servo.write(lock ? valLocked : valUnlocked);
  Bean.setLed(lock ? 255 : 0, lock ? 0 : 255, 0);
  delay(500);
  locked = lock;
  servo.detach();
}

void blinkOutsideLed(int numBlinks, int pin) {
  for (int i = 0; i < numBlinks; i++) {
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
    delay(200);
  }
}

