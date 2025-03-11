#include <Stepper.h>

const int stepsPerRevolution = 2048;
const int rpm = 15;
const int buttonPin = 13;
int direction = 1; // Initial direction (1 for clockwise, -1 for counterclockwise)
const unsigned long minChangeInterval = 3000; // Minimum time between direction changes (in milliseconds)

unsigned long totalCycleTime = 122000;
unsigned long stopInterval = 500; // Stop the motor every 3 seconds
const unsigned long stopDuration = 1000; // Stop duration (1 second)
//const unsigned long autoChangeTime = totalCycleTime - (totalCycleTime / stopInterval) * stopDuration;
const unsigned long autoChangeTime = 122000;
Stepper stepper1(stepsPerRevolution, 2, 4, 3, 5);
Stepper stepper2(stepsPerRevolution, 9, 11, 10, 12);

unsigned long lastChangeTime = 0;
unsigned long lastButtonPressTime = 0;
unsigned long lastStopTime = 0;
bool isStopping = false;
bool startSignalReceived = false;
bool manstartSignalReceived = false;
void setup() {
  stepper1.setSpeed(rpm);
  stepper2.setSpeed(10);
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);

  Serial.println("Stepper Motors Initialized");
  delay(1000);
  
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  // Check for start or stop signals from the Raspberry Pi
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove any extra spaces or newlines
    if (input == "start") {
      startSignalReceived = true;
      Serial.println("Start signal received, beginning motor movement");
      Serial.println("newJob");
      Serial.println("vid_start");
      lastChangeTime = millis(); // Initialize the lastChangeTime
      lastStopTime = millis(); // Initialize the lastStopTime
    } else if (input == "stop") {
      startSignalReceived = false;
      manstartSignalReceived = false;
      Serial.println("Stop signal received, motors stopping");
      return; // Exit the loop to stop motor operations
    }
    else if(input == "medium"){
      stopInterval = 2000;
      totalCycleTime = 150000;
      }
    else if(input == "high"){
        stopInterval = 1000;
        totalCycleTime = 450000;
        }
    else if(input == "low"){
        stopInterval = 3000;
        
        totalCycleTime = 300000;}

    else if (input == "reverse") {
          buttonState = HIGH ;
    }
    else if(input == "start_manual")
    { manstartSignalReceived = HIGH ;
      Serial.println("Manual Start signal received, beginning motor movement");
      Serial.println("newJob");
      Serial.println("vid_start");
      

    }
    else if(input == "move_up")
    {
      int stepsPerSecond = (stepsPerRevolution * rpm) / 60;

    // Move the stepper
      stepper1.step(-1 * stepsPerSecond/2);
    }
    else if(input == "move_down")
    {
      int stepsPerSecond = (stepsPerRevolution * rpm) / 60;

    // Move the stepper
      stepper1.step(1 * stepsPerSecond/2);
    }
    
  }
  
  // Wait for the "start" signal before running the motor logic
  if (!startSignalReceived) {
    return; // Exit the loop until the "start" signal is received
  }

  

  // Button press handling
  if (buttonState == HIGH && millis() - lastButtonPressTime > minChangeInterval) {
    direction *= -1; // Reverse direction
    lastChangeTime = millis(); // Reset the change timer
    lastButtonPressTime = millis(); // Reset the button press timer
    Serial.println("Button pressed, direction changed");
  }

  // Automatic direction change
  if (millis() - lastChangeTime > autoChangeTime) {
    direction *= -1;
    lastChangeTime = millis(); // Reset the change timer
    Serial.println("Automatic direction change");
  }

  // Stop the motor every 3 seconds for 1 second
  if (millis() - lastStopTime > 500) {
    isStopping = true;
    lastStopTime = millis();
    Serial.println("Motor stopped for 1 second");
    
    // Stepper 2 rotates 90 degrees
    rotateStepper2();
   
    delay(500); // Wait for 1 second

    // Stepper 2 rotates another 90 degrees
    rotateStepper2();
    Serial.println("Stepper 2 rotation completed");
    //Serial.println("click");
    delay(500);
  }

  if (isStopping) {
    // Keep motor stopped for the stop duration
    if (millis() - lastStopTime >= stopDuration) {
      isStopping = false;
      lastStopTime = millis();
      Serial.println("Motor resumed");
    }
  } else {
    // Calculate steps for one second
    int stepsPerSecond = (stepsPerRevolution * rpm) / 60;

    // Move the stepper
    stepper1.step(direction * stepsPerSecond);
  }

  // Logging
  Serial.print("Direction: ");
  Serial.print(direction);
  Serial.print(", Time since last change: ");
  Serial.print(millis() - lastChangeTime); 
  Serial.println(" ms");
}

void rotateStepper2() {
  int stepsFor90Degrees = stepsPerRevolution ; // 2048 steps for full revolution -> 90 degrees = 2048/4
  stepper2.step(stepsFor90Degrees);
  Serial.println("Stepper 2 rotated 360 degrees");
}
