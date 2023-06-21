const int trigPin = 5;
const int echoPin = 6;
long pulseDuration; // sound wave travel time
int pulseDistance;

int SAFETY_MARGIN = 10; // cm

void obstacleAvoidanceControl(void *parameter) {
    while(1) {
        if (pulseDistance < 10) {
            // maintain a distance of 10cm from an obstacle in front
            goBackward();
            handleRelease();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setupUltrasonicSensor() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    // xTaskCreate(
    //     obstacleAvoidanceControl,   // Task function
    //     "obstacleAvoidanceControl", // Task name
    //     1000,                     // Stack size (bytes)
    //     NULL,                      // Task parameters
    //     2,                         // Task priority, higher number = higher priority
    //     NULL                       // Task handle, allows you to interact with task from other tasks
    // );
}

// update the distance (in cm) of an object in front of the robot
void loopUltrasonicSensor() {
    // make sure trig pin is clear
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    // set trig pin to high state for 10 microseconds 
    // to generate ultrasound wave
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // HIGH means the pulseIn function will wait for the echo pin to go HIGH
    // i.e. when we receive the reflected ultrasound wave
    pulseDuration = pulseIn(echoPin, HIGH);
    pulseDistance = pulseDuration * 0.034 / 2; // distance in cm

    // Serial.print("Distance: ");
    // Serial.println(ultrasonicDistance);
}