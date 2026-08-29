
// ==========================================
// TRIAL 1 - SENSOR TO MOTOR TEST
//
// Sensor Array 1 = LEFT
// Sensor Array 2 = RIGHT
//
// LEFT  BLACK + RIGHT BLACK -> BOTH FORWARD
// LEFT  WHITE + RIGHT BLACK -> LEFT MOTOR STOP
// LEFT  BLACK + RIGHT WHITE -> RIGHT MOTOR STOP
// LEFT  WHITE + RIGHT WHITE -> BOTH STOP
// ==========================================


// ==========================================
// CD74HC4067 PINS
// ==========================================

#define MUX_SIG A4

#define S0 A0
#define S1 A1
#define S2 A2
#define S3 A3


// ==========================================
// TB6612FNG PINS
// ==========================================

// LEFT MOTOR
#define PWMA 5
#define AIN1 10
#define AIN2 11

// RIGHT MOTOR
#define PWMB 6
#define BIN1 8
#define BIN2 7

// STANDBY
#define STBY 9


// ==========================================
// ENCODER PINS
// Currently NOT being used
// ==========================================

// LEFT ENCODER
#define LEFT_C1 2
#define LEFT_C2 4

// RIGHT ENCODER
#define RIGHT_C1 3
#define RIGHT_C2 12


// ==========================================
// CALIBRATION
// ==========================================

// Sensor value >= threshold = BLACK
// Sensor value < threshold  = WHITE

int BLACK_THRESHOLD = 500;


// Motor speed
int MOTOR_SPEED = 150;


// ==========================================
// SELECT MUX CHANNEL
// ==========================================

void selectChannel(int channel)
{
  digitalWrite(S0, channel & 0x01);
  digitalWrite(S1, (channel >> 1) & 0x01);
  digitalWrite(S2, (channel >> 2) & 0x01);
  digitalWrite(S3, (channel >> 3) & 0x01);
}


// ==========================================
// READ ONE SENSOR
// ==========================================

int readSensor(int channel)
{
  selectChannel(channel);

  delayMicroseconds(10);

  return analogRead(MUX_SIG);
}


// ==========================================
// LEFT MOTOR FORWARD
// ==========================================

void leftMotorForward()
{
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  analogWrite(PWMA, MOTOR_SPEED);
}


// ==========================================
// RIGHT MOTOR FORWARD
// ==========================================

void rightMotorForward()
{
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMB, MOTOR_SPEED);
}


// ==========================================
// STOP LEFT MOTOR
// ==========================================

void stopLeftMotor()
{
  analogWrite(PWMA, 0);

  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
}


// ==========================================
// STOP RIGHT MOTOR
// ==========================================

void stopRightMotor()
{
  analogWrite(PWMB, 0);

  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}


// ==========================================
// STOP BOTH MOTORS
// ==========================================

void stopMotors()
{
  stopLeftMotor();
  stopRightMotor();
}


// ==========================================
// SETUP
// ==========================================

void setup()
{
  Serial.begin(9600);


  // ---------- MUX ----------
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(MUX_SIG, INPUT);


  // ---------- TB6612 ----------
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);


  // ---------- ENCODERS ----------
  // Not used in this trial
  pinMode(LEFT_C1, INPUT);
  pinMode(LEFT_C2, INPUT);

  pinMode(RIGHT_C1, INPUT);
  pinMode(RIGHT_C2, INPUT);


  // Enable TB6612
  digitalWrite(STBY, HIGH);


  // Start motors OFF
  stopMotors();


  // ---------- SERIAL ----------
  Serial.println("==========================================");
  Serial.println("       SENSOR + MOTOR TRIAL 1");
  Serial.println("==========================================");

  Serial.print("Black Threshold: ");
  Serial.println(BLACK_THRESHOLD);

  Serial.print("Motor PWM: ");
  Serial.println(MOTOR_SPEED);

  Serial.println("------------------------------------------");
}


// ==========================================
// MAIN LOOP
// ==========================================

void loop()
{
  bool leftBlack = false;
  bool rightBlack = false;


  Serial.println();
  Serial.println("============= SENSOR READINGS =============");


  // ========================================
  // READ LEFT SENSOR ARRAY
  // Channels 0-7
  // ========================================

  Serial.println();
  Serial.println("--- LEFT SENSOR ARRAY ---");

  for (int i = 0; i < 8; i++)
  {
    int value = readSensor(i);


    Serial.print("L");
    Serial.print(i + 1);

    Serial.print(" | Value: ");
    Serial.print(value);

    Serial.print(" | ");


    if (value >= BLACK_THRESHOLD)
    {
      Serial.println("BLACK");

      leftBlack = true;
    }
    else
    {
      Serial.println("WHITE");
    }
  }


  // ========================================
  // READ RIGHT SENSOR ARRAY
  // Channels 8-15
  // ========================================

  Serial.println();
  Serial.println("--- RIGHT SENSOR ARRAY ---");

  for (int i = 8; i < 16; i++)
  {
    int value = readSensor(i);


    Serial.print("R");
    Serial.print(i - 7);

    Serial.print(" | Value: ");
    Serial.print(value);

    Serial.print(" | ");


    if (value >= BLACK_THRESHOLD)
    {
      Serial.println("BLACK");

      rightBlack = true;
    }
    else
    {
      Serial.println("WHITE");
    }
  }


  // ========================================
  // SHOW ARRAY RESULTS
  // ========================================

  Serial.println();
  Serial.println("------------- ARRAY RESULT -------------");


  Serial.print("LEFT ARRAY: ");

  if (leftBlack)
  {
    Serial.println("BLACK");
  }
  else
  {
    Serial.println("WHITE");
  }


  Serial.print("RIGHT ARRAY: ");

  if (rightBlack)
  {
    Serial.println("BLACK");
  }
  else
  {
    Serial.println("WHITE");
  }


  // ========================================
  // MOTOR DECISION
  // ========================================

  Serial.println();
  Serial.println("------------- MOTOR ACTION -------------");


  // ----------------------------------------
  // BOTH BLACK
  // ----------------------------------------

  if (leftBlack && rightBlack)
  {
    Serial.println("LEFT  = BLACK");
    Serial.println("RIGHT = BLACK");
    Serial.println("ACTION: BOTH MOTORS FORWARD");


    leftMotorForward();
    rightMotorForward();
  }


  // ----------------------------------------
  // LEFT WHITE, RIGHT BLACK
  // ----------------------------------------

  else if (!leftBlack && rightBlack)
  {
    Serial.println("LEFT  = WHITE");
    Serial.println("RIGHT = BLACK");
    Serial.println("ACTION: LEFT MOTOR STOP");
    Serial.println("        RIGHT MOTOR FORWARD");


    stopLeftMotor();
    rightMotorForward();
  }


  // ----------------------------------------
  // LEFT BLACK, RIGHT WHITE
  // ----------------------------------------

  else if (leftBlack && !rightBlack)
  {
    Serial.println("LEFT  = BLACK");
    Serial.println("RIGHT = WHITE");
    Serial.println("ACTION: LEFT MOTOR FORWARD");
    Serial.println("        RIGHT MOTOR STOP");


    leftMotorForward();
    stopRightMotor();
  }


  // ----------------------------------------
  // BOTH WHITE
  // ----------------------------------------

  else
  {
    Serial.println("LEFT  = WHITE");
    Serial.println("RIGHT = WHITE");
    Serial.println("ACTION: BOTH MOTORS STOP");


    stopMotors();
  }


  Serial.println("==========================================");


  delay(100);
