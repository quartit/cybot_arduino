
#include <SpeedyStepper.h>

#define MOTOR_X_ENABLE_PIN 14
#define MOTOR_X_STEP_PIN 15
#define MOTOR_X_DIR_PIN 16

#define MOTOR_Y_ENABLE_PIN 18
#define MOTOR_Y_STEP_PIN 19
#define MOTOR_Y_DIR_PIN 20


SpeedyStepper stepperX;
SpeedyStepper stepperY;


int accelerationX = 500;
int accelerationY = 2000;
int speedX = 6000;
int speedY = 3000;


// Declare stepper commands
bool runToPos = false;
bool runConst = false;

// These strings hold the serial input and an Int parser, respectively
// For serial interface
String buffer;
String intbuffer;

void serial() {
  // Wait for a serial command
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n') {
      if (buffer.startsWith("id")) {
        Serial.println("id:stepper-test"); // so other devices can figure out what is connected
      }
      else if (buffer.startsWith("help")) {
        // Help function
        // Display action
        Serial.println("AVAILABLE COMMANDS:");
        Serial.println("move X****/X : moves X to specified position");
        Serial.println("accelX**** / accelY**** sets acceleration");
        Serial.println("maxspeedX**** / maxspeedY**** : sets max speed");
        Serial.println("help : displays this menu again");
        Serial.println("values : displays stepper values");
      }


      else if (buffer.startsWith("move")) {
        // Moves to a relative position
        intbuffer = buffer;

        String sx = intbuffer.substring(intbuffer.indexOf("X") + 1, intbuffer.indexOf("/X") + 1);
        String xy = intbuffer.substring(intbuffer.indexOf("Y") + 1, intbuffer.indexOf("/Y") + 1);
        int destX = sx.toInt();
        int destY = xy.toInt();
        //int dest = intbuffer.toInt();
        moveXYWithNoCoordination(-destX, -destY, speedX, accelerationX,  speedY, accelerationY);
        runToPos = true;
        runConst = false;
        // Display action
        Serial.println("Moving X: ");
        Serial.println((int)destX);
        Serial.println("Moving Y: ");
        Serial.println((int)destY);
      }
      else if (buffer.startsWith("accelX")) {
        // Set acceleration
        intbuffer = buffer;
        intbuffer.remove(0, 6);
        int acc = intbuffer.toInt();
        accelerationX = acc;
        // Display action
        Serial.println("Accel: ");
        Serial.println((int)acc);
      }
      else if (buffer.startsWith("accelY")) {
        // Set acceleration
        intbuffer = buffer;
        intbuffer.remove(0, 6);
        int acc = intbuffer.toInt();
        accelerationY = acc;
        // Display action
        Serial.println("Accel: ");
        Serial.println((int)acc);
      }
      else if (buffer.startsWith("values")) {
        Serial.print("accelX: ");
        Serial.print( accelerationX );
        Serial.print( " accelY: " );
        Serial.println(accelerationY);
        Serial.print("maxspeedX: " );
        Serial.print(speedX);
        Serial.print(" maxspeedY: ");
        Serial.println(speedY);
      }
      else if (buffer.startsWith("maxspeedX")) {
        // Set max speed
        intbuffer = buffer;
        intbuffer.remove(0, 9);
        int mspd = intbuffer.toInt();
        speedX = mspd;
        // Display action
        Serial.println("Max Speed: ");
        Serial.println((int)mspd);
      }
      else if (buffer.startsWith("maxspeedY")) {
        // Set max speed
        intbuffer = buffer;
        intbuffer.remove(0, 9);
        int mspd = intbuffer.toInt();
        speedY = mspd;
        // Display action
        Serial.println("Max Speed: ");
        Serial.println((int)mspd);
      }
      else {
        Serial.println("Unknown cmd.");
      }
      buffer = "";
    } else buffer += c;
  }
}

//
// move both X & Y motors together in a coordinated way, such that they each
// start and stop at the same time, even if one motor moves a greater distance
//
void moveXYWithCoordination(long stepsX, long stepsY, float speedInStepsPerSecondX, float accelerationInStepsPerSecondPerSecondX, float speedInStepsPerSecondY, float accelerationInStepsPerSecondPerSecondY)
{
  float speedInStepsPerSecond_X;
  float accelerationInStepsPerSecondPerSecond_X;
  float speedInStepsPerSecond_Y;
  float accelerationInStepsPerSecondPerSecond_Y;
  long absStepsX;
  long absStepsY;

  //
  // setup initial speed and acceleration values
  //
  speedInStepsPerSecond_X = speedInStepsPerSecondX;
  accelerationInStepsPerSecondPerSecond_X = accelerationInStepsPerSecondPerSecondX;

  speedInStepsPerSecond_Y = speedInStepsPerSecondY;
  accelerationInStepsPerSecondPerSecond_Y = accelerationInStepsPerSecondPerSecondY;


  //
  // determine how many steps each motor is moving
  //
  if (stepsX >= 0)
    absStepsX = stepsX;
  else
    absStepsX = -stepsX;

  if (stepsY >= 0)
    absStepsY = stepsY;
  else
    absStepsY = -stepsY;


  //
  // determine which motor is traveling the farthest, then slow down the
  // speed rates for the motor moving the shortest distance
  //
  if ((absStepsX > absStepsY) && (stepsX != 0))
  {
    //
    // slow down the motor traveling less far
    //
    float scaler = (float) absStepsY / (float) absStepsX;
    speedInStepsPerSecond_Y = speedInStepsPerSecond_Y * scaler;
    accelerationInStepsPerSecondPerSecond_Y = accelerationInStepsPerSecondPerSecond_Y * scaler ;
  }

  if ((absStepsY > absStepsX) && (stepsY != 0))
  {
    //
    // slow down the motor traveling less far
    //
    float scaler = (float) absStepsX / (float) absStepsY;
    speedInStepsPerSecond_X = speedInStepsPerSecond_X * scaler;
    accelerationInStepsPerSecondPerSecond_X = accelerationInStepsPerSecondPerSecond_X * scaler;
  }


  //
  // setup the motion for the X motor
  //
  stepperX.setSpeedInStepsPerSecond(speedInStepsPerSecond_X);
  stepperX.setAccelerationInStepsPerSecondPerSecond(accelerationInStepsPerSecondPerSecond_X);
  stepperX.setupRelativeMoveInSteps(stepsX);


  //
  // setup the motion for the Y motor
  //
  stepperY.setSpeedInStepsPerSecond(speedInStepsPerSecond_Y);
  stepperY.setAccelerationInStepsPerSecondPerSecond(accelerationInStepsPerSecondPerSecond_Y);
  stepperY.setupRelativeMoveInSteps(stepsY);


  //
  // now execute the moves, looping until both motors have finished
  //
  while ((!stepperX.motionComplete()) || (!stepperY.motionComplete()))
  {
    stepperX.processMovement();
    stepperY.processMovement();
  }
}

void moveXYWithNoCoordination(long stepsX, long stepsY, float speedInStepsPerSecondX, float accelerationInStepsPerSecondPerSecondX, float speedInStepsPerSecondY, float accelerationInStepsPerSecondPerSecondY)
{
  //
  // setup the motion for the X motor
  //
  stepperX.setSpeedInStepsPerSecond(speedInStepsPerSecondX);
  stepperX.setAccelerationInStepsPerSecondPerSecond(accelerationInStepsPerSecondPerSecondX);
  stepperX.setupRelativeMoveInSteps(stepsX);


  //
  // setup the motion for the Y motor
  //
  stepperY.setSpeedInStepsPerSecond(speedInStepsPerSecondY);
  stepperY.setAccelerationInStepsPerSecondPerSecond(accelerationInStepsPerSecondPerSecondY);
  stepperY.setupRelativeMoveInSteps(stepsY);


  //
  // now execute the moves, looping until both motors have finished
  //
  while ((!stepperX.motionComplete()) || (!stepperY.motionComplete()))
  {
    stepperX.processMovement();
    stepperY.processMovement();
  }
}
/*
   SETUP FUNCTION
*/

// Required by Arduino; runs once at reset
void setup() {

  // Open serial connection and print a message
  Serial.begin(9600);
  Serial.println("AVAILABLE COMMANDS:");
  Serial.println("move X****/X : moves X to specified position");
  Serial.println("accelX**** / accelY**** sets acceleration");
  Serial.println("maxspeedX**** / maxspeedY**** : sets max speed");
  Serial.println("help : displays this menu again");
  Serial.println("values : displays stepper values");

  pinMode(MOTOR_X_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_Y_ENABLE_PIN, OUTPUT);


  stepperX.connectToPins(MOTOR_X_STEP_PIN, MOTOR_X_DIR_PIN);
  stepperY.connectToPins(MOTOR_Y_STEP_PIN, MOTOR_Y_DIR_PIN);

}

/*
   LOOP FUNCTION
*/

void loop() {

  serial();

}
