// Pin definitions
const int pirPin = 2; // PIR sensor connected to digital pin 2
const int potPin = A0; // Potentiometer for adjusting light on duration connected to analog pin A0
const int relayPin = 3; // Relay module connected to digital pin 3
const int photoresistorPin = A1; // Photoresistor connected to analog pin A1
const int triggerPin = 4; // Ultrasonic sensor trigger pin connected to digital pin 4
const int echoPin = 5; // Ultrasonic sensor echo pin connected to digital pin 5

// Constants
const int lightThresholdPot = A2; // Potentiometer for adjusting light threshold connected to analog pin A2
const int distanceThresholdPot = A3; // Potentiometer for adjusting distance threshold connected to analog pin A3
const unsigned long debounceDelay = 200; // Debounce delay in milliseconds

// States
enum State {LIGHT_OFF, LIGHT_ON}; // State machine: LIGHT_OFF and LIGHT_ON states
State currentState = LIGHT_OFF; // Initialize the state machine with the LIGHT_OFF state

// Variables
unsigned long lastDebounceTime = 0; // Variable to store the last time the light was turned on

void setup() {
  pinMode(pirPin, INPUT); // Set PIR sensor pin as input
  pinMode(potPin, INPUT); // Set potentiometer for light on duration pin as input
  pinMode(relayPin, OUTPUT); // Set relay module pin as output
  pinMode(photoresistorPin, INPUT); // Set photoresistor pin as input
  pinMode(triggerPin, OUTPUT); // Set ultrasonic sensor trigger pin as output
  pinMode(echoPin, INPUT); // Set ultrasonic sensor echo pin as input
  
  digitalWrite(relayPin, LOW); // Initialize relay module in the off state
  Serial.begin(9600); // Start serial communication at 9600 baud rate
}

void loop() {
  int pirState = digitalRead(pirPin); // Read PIR sensor state
  int lightIntensity = analogRead(photoresistorPin); // Read light intensity from photoresistor
  int potValue = analogRead(potPin); // Read light on duration from potentiometer
  int lightThreshold = analogRead(lightThresholdPot); // Read light threshold from potentiometer
  int distanceThreshold = map(analogRead(distanceThresholdPot), 0, 1023, 10, 200); // Read and map distance threshold from potentiometer

  long distance = readDistance(); // Read distance from ultrasonic sensor
  
  // Debug messages
  Serial.print("PIR State: ");
  Serial.println(pirState);
  Serial.print("Light Intensity: ");
  Serial.println(lightIntensity);
  Serial.print("Pot Value: ");
  Serial.println(potValue);
  Serial.print("Light Threshold: ");
  Serial.println(lightThreshold);
  Serial.print("Distance Threshold: ");
  Serial.println(distanceThreshold);
  Serial.print("Distance: ");
  Serial.println(distance);
  
  // State machine switch case
  switch (currentState) {
    case LIGHT_OFF: // In the LIGHT_OFF state
      // If motion detected, light intensity is below the threshold, and distance is below the threshold
      if (pirState == HIGH && lightIntensity < lightThreshold && distance < distanceThreshold) {
        currentState = LIGHT_ON; // Change to the LIGHT_ON state
        lastDebounceTime = millis(); // Update the last debounce time
      }
      break;

    case LIGHT_ON: // In the LIGHT_ON state
      digitalWrite(relayPin, HIGH); // Turn the relay module on, enabling the light
      // If the light has been on for longer than the potentiometer-specified duration
      if (millis() - lastDebounceTime > potValue) {
        currentState = LIGHT_OFF; // Change to the LIGHT_OFF state
        digitalWrite(relayPin, LOW); // Turn the relay module off, disabling the light
      }
      break;
  }
  delay(100);
}

// Function to read the distance from the ultrasonic sensor
long readDistance() {
  digitalWrite(triggerPin, LOW); // Ensure the trigger pin starts in the LOW state
  delayMicroseconds(2); // Wait for 2 microseconds to stabilize the trigger pin
  digitalWrite(triggerPin, HIGH); // Set the trigger pin to HIGH to send a 10-microsecond pulse
  delayMicroseconds(10); // Wait for 10 microseconds while the pulse is sent
  digitalWrite(triggerPin, LOW); // Set the trigger pin back to LOW
  
  // Measure the duration of the echo pin's HIGH state, which is proportional to the distance
  long duration = pulseIn(echoPin, HIGH);
  
  // Convert the duration to distance in centimeters (0.0344 cm per microsecond, divided by 2 for round trip)
  long distance = duration * 0.0344 / 2;
  
  return distance; // Return the calculated distance
}

