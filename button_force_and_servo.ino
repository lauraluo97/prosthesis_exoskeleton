#include <VarSpeedServo.h>

// Servo
VarSpeedServo servo_1;
int servo_1_pin = 10;  // digital pin 10 (PWM)
int first_check = 180;
int second_check = 0;
int turnSpeed = 15; // the turn speed, see "VarSpeedServo Details" for information
int currAng;  // holds current angle measurement
int minPW = 500;   // 20 kg-cm Digital Servo: 500 - 2500
int maxPW = 2500;  // Analog Hobby Servo: 1500 - 1900 

// Force Sensor
int fsrPin = 0; // the FSR and ~10K pulldown are connected to a0
int fsrReading; // the analog reading from the FSR resistor divider
int graspThresh = 200; // the analog reading after which the pressure is strong enough to grasp something: the higher, the stronger

// Button
int button_pin = 2;

// VarSpeedServo Details
  // slowmove(newpos, speed)
  // 0 = fastest speed
  // 1 = slowest speed
  // 2 - 255 = slow to fast, somewhere in between, there is usually a cap

void setup() {
  servo_1.attach(servo_1_pin, minPW, maxPW);
  pinMode(button_pin,INPUT);
  Serial.begin(9600);
  
  // Start at angle 0
  servo_1.slowmove(0,0); 
  currAng = servo_1.read();
  while(currAng > first_check){
    Serial.println("Setting Up");
    currAng = servo_1.read();
  }
  Serial.println(currAng);
  
}

void loop() {
  // CHECK for GRAB COMMAND
  while(!grabRelease()){
    // Do Nothing
    Serial.println("Resting");
  }
  
  // STARTING GRASP
  servo_1.slowmove(first_check, turnSpeed);
  currAng = servo_1.read();
  // Continue turning UNLESS we are grasping
  while(currAng < first_check){
    if(isGrasp()){                             // If met with resistance, interrupt old angle with most current angle and
      servo_1.slowmove(currAng, turnSpeed);    // break out of loop
      break;
    }
    Serial.println("Grasping");
    // Serial.print(currAng);
    currAng = servo_1.read();
  } // If nothing held, stop at angle defined by "first_check"

  // CHECK for RELEASE COMMAND
  while(!grabRelease()){  // Hold here until button is pressed again
    // Do Nothing
    Serial.println("Holding");
  }
  
  // RELEASING
  servo_1.slowmove(second_check, turnSpeed);
  currAng = servo_1.read();
  // Continue opening
  while(currAng > second_check){
    Serial.println("Opening");
    // Serial.print(currAng);
    currAng = servo_1.read();
  }  // Stop at angle defined by "second_check"
  
}


// Checks if we have grasped an item at a specified force threshold
bool isGrasp(){
  fsrReading = analogRead(fsrPin);
  Serial.println("Force Reading:");
  Serial.print(fsrReading);
  if(fsrReading > graspThresh){ return true;}
  return false;
}

// Checks if button to grab is clicked
// Positive logic
bool grabRelease(){
  if(digitalRead(button_pin) == HIGH){ return true;}
  return false;
}





