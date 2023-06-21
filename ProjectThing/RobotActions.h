#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address 0x60
// https://learn.adafruit.com/adafruit-stepper-dc-motor-featherwing/pinouts#i2c-addressing-1883531
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61);

// Select which 'port' M1, M2, M3 or M4
Adafruit_DCMotor *leftMotor = AFMS.getMotor(2); // 2 and 3
Adafruit_DCMotor *rightMotor = AFMS.getMotor(1); // 1 and 4

void setupMotorShield() {
    if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
        Serial.println("Could not find Motor Shield. Check wiring.");
        while (1);
    }
    Serial.println("Motor Shield found.");
}

void handleRobotMotion(int leftMotorDirection, int rightMotorDirection, int motorSpeed) {
    // Set the speed to start, from 0 (off) to 255 (max speed)
    leftMotor->setSpeed(motorSpeed);
    rightMotor->setSpeed(motorSpeed);

    leftMotor->run(leftMotorDirection);
    rightMotor->run(rightMotorDirection);
}

void handleRelease() {
    // RELEASE stops the motor
    // removes power from the motor and is equivalent to setSpeed(0)
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
}

void goForward() {
    handleRobotMotion(FORWARD, FORWARD, 50);
}
void goBackward() {
    handleRobotMotion(BACKWARD, BACKWARD, 50);
}
void turnLeft() {
    handleRobotMotion(BACKWARD, FORWARD, 50);
}
void turnRight() {
    handleRobotMotion(FORWARD, BACKWARD, 50);
}