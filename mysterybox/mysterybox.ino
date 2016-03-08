#include <Servo.h>

Servo servo;

// The top secret pin code
const uint32_t pinCode = 43215; 

// Pins
const int servoPin = 1;
const int ledPin = 5;

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
    } else if (state == pinCode && locked) {
      lock(false);
      // Hide the pin code so that the next client can't read it
      Bean.setScratchNumber(bank, 1);
    }
  }
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

