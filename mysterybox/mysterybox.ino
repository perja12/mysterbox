#include <Servo.h>

// Sketch for LightBlue Bean: https://punchthrough.com/bean

Servo servo;

// The top secret pin code
const uint32_t pin_code = 43215;

// Pins
const int servo_pin = 1;
const int led_pin = 5;

// Values for the servo
const int val_locked = 72;
const int val_unlocked = 106;

// The scratch banks to use
const int pin_bank = 1;
const int result_bank = 2;

static bool connected = false;
static bool locked = false;

const int lock_error = 0;
const int lock_success = 1;

int attempts = 0;
unsigned long stop_ignore = 0;
long old_value = 0;

void setup() {
    Bean.enableConfigSave(false);
    Bean.enableWakeOnConnect(true);
    Bean.setAdvertisingInterval(250);
    pinMode(led_pin, OUTPUT);

    // Lock the box when powering on
    lock(true);
}

bool isBruteForceTimeoutActive() {
    unsigned long now = millis();
    Bean.setScratchNumber(3, now);
    Bean.setScratchNumber(4, stop_ignore);

    if (millis() < stop_ignore) {
        return true;
    } else {
        return false;
    }
}

void checkBruteForce() {
    if (attempts > 0 && (attempts % 10 == 0)) {
        stop_ignore = (unsigned long)(millis() + 5000);
    } else {
        stop_ignore = 0;
    }
}

void loop() {
    Bean.setLed(0, 0, 255);
    connected = getConnectionState();

    if (connected) {
        // Turn the led around the power button on
        digitalWrite(led_pin, HIGH);

        // Check if the scratch value has changed
        long value = Bean.readScratchNumber(pin_bank);
        if (value != old_value) {
            Bean.setLed(0, 255, 0);
            delay(200);
            attempts++;

            old_value = value;
            // There is a new value - first check if the brute force timeout is active
            if (isBruteForceTimeoutActive()) {
                Bean.setLed(255, 0, 0);
                Bean.sleep(100);
                return;
            }

            checkBruteForce();

            // If not we check if this is an attempt to unlock or lock
            if (locked && value != 0) { // UNLOCK
                if (value == pin_code) {
                    lock(false);
                    // Hide the pin code so that the next client can't read it
                    Bean.setScratchNumber(pin_bank, -1);
                    attempts = 0;
                }
            } else if (!locked && value == 0) { // LOCK
                lock(true);
            }
        }
    } else {
        // We are not connected - turn off led and go to sleep if there is no active connection.
        digitalWrite(led_pin, LOW);
        Bean.sleep(0xffffffff);
    }
    Bean.sleep(100);
}

void lock(bool lock) {
    servo.attach(servo_pin);
    servo.write(lock ? val_locked : val_unlocked);
    for (int i = 0; i < 5; i++) {
        Bean.setLed(lock ? 255 : 0, lock ? 0 : 255, 0);
        delay(100);
        Bean.setLed(0, 0, 0);
        delay(100);
    }
    locked = lock;
    servo.detach();
    Bean.setScratchNumber(result_bank, (locked ? 1 : 0));
}

// Workaround for bug in firmware.
// See: http://beantalk.punchthrough.com/t/wrong-result-from-bean-getconnectionstate/1089/7
bool getConnectionState() {
    int count = 0;
    bool connected = Bean.getConnectionState();
    while (!connected && (count++ < 2)) {
        delay(5);
        connected = Bean.getConnectionState();
    }
    return connected;
}
