// =============================================
// ESP32 + L298N Line Follower (PID)
// Updated for ESP32 Core 3.x
// =============================================

// --- L298N Pin Definitions ---
#define IN1 5    // Left motor direction A
#define IN2 17   // Left motor direction B
#define IN3 16   // Right motor direction A
#define IN4 4    // R  ight motor direction B
#define ENA 2   // Left motor PWM (must be PWM-capable pin)
#define ENB 18    // Right motor PWM (must be PWM-capable pin)

// --- ESP32 PWM Config (NEW for Core 3.x) ---
#define PWM_FREQ  1000   // 1 kHz
#define PWM_RES   8      // 8-bit (0–255)
// NOTE: ledcSetup() and ledcAttachPin() are removed in Core 3.x
// Use ledcAttach(pin, freq, res) instead

// --- PID Constants ---
#define KP 2
#define KD 5
#define BASE_SPEED      100  // Forward cruise speed
#define M1_MAX_SPEED    120
#define M2_MAX_SPEED    120

// --- QTR Sensor Settings ---
#define NUM_SENSORS 4
const int sensorPins[NUM_SENSORS] = {34, 35, 33, 25};
int sensorValues[NUM_SENSORS];
int lastError = 0;

// --- Function Prototypes ---
void setMotors(int leftSpeed, int rightSpeed);
void setLeftMotor(int speed);
void setRightMotor(int speed);
int  readLinePosition();
void calibrateSensors();
void stopMotors();
void followLine();

void Forward();
void SmallForward();
void Right();
void Left();
void HardRight();
void HardLeft();
void Reverse();

// =============================================

void setup() {
  Serial.begin(115200);

  // Motor direction pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // ---- ESP32 PWM Setup (NEW for Core 3.x) ----
  // Single function does everything - no channels needed
  ledcAttach(ENA, PWM_FREQ, PWM_RES);
  ledcAttach(ENB, PWM_FREQ, PWM_RES);

  delay(2000);
  calibrateSensors();
  stopMotors();
}

void loop() {
  followLine();

  // --- Or test manual moves ---
  /*
  Forward();
  delay(1000);
  Left();
  delay(500);
  stopMotors();
  delay(1000);
  */
}

// =============================================
// Motor Driver (L298N)
// =============================================

// speed: -255 (full reverse) to 255 (full forward)
void setLeftMotor(int speed) {
  speed = constrain(speed, -255, 255);
  if (speed >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    speed = -speed;
  }
  // Write directly to the pin (NEW for Core 3.x)
  ledcWrite(ENA, speed);
}

void setRightMotor(int speed) {
  speed = constrain(speed, -255, 255);
  if (speed >= 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    speed = -speed;
  }
  // Write directly to the pin (NEW for Core 3.x)
  ledcWrite(ENB, speed);
}

// Convenience wrapper: both motors forward with given speeds (0–255)
void setMotors(int leftSpeed, int rightSpeed) {
  setLeftMotor(leftSpeed);
  setRightMotor(rightSpeed);
}

void stopMotors() {
  setLeftMotor(0);
  setRightMotor(0);
}

// =============================================
// PID Line Following
// =============================================

int readLinePosition() {
  long avg = 0;
  long sum = 0;

  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorValues[i] = analogRead(sensorPins[i]);
    // ESP32 ADC is 12-bit (0–4095); invert for black line on white
    int value = 4095 - sensorValues[i];
    avg += (long)value * (i * 1000);
    sum += value;
  }

  if (sum == 0) {
    return ((NUM_SENSORS - 1) * 1000) / 2; // No line detected — return center
  }
  return (int)(avg / sum); // 0 to 4000 for 5 sensors
}

void calibrateSensors() {
  // Simple warm-up reads — extend this with min/max tracking if needed
  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < NUM_SENSORS; j++) {
      analogRead(sensorPins[j]);
    }
    delay(10);
  }
}

void followLine() {
  int position = readLinePosition();
  int setpoint = ((NUM_SENSORS - 1) * 1000) / 2;
  int error = position - setpoint;

  // PID calculation
  int correction = (KP * error) + (KD * (error - lastError));
  lastError = error;

  // --- DYNAMIC SPEED CONTROL ---
  // Slow down on sharp curves
  int turnFactor = abs(error) / 12;  // Adjust 12 to control slowdown amount
  int dynamicSpeed = BASE_SPEED - turnFactor;
  dynamicSpeed = constrain(dynamicSpeed, 80, BASE_SPEED);

  // Apply speeds with dynamic base
  int leftSpeed = dynamicSpeed + correction;
  int rightSpeed = dynamicSpeed - correction;

  // Clamp to valid range
  leftSpeed = constrain(leftSpeed, 0, M1_MAX_SPEED);
  rightSpeed = constrain(rightSpeed, 0, M2_MAX_SPEED);

  setMotors(leftSpeed, rightSpeed);
}

// =============================================
// Manual Movement Functions
// =============================================

void Forward() {
  setMotors(120, 120);
}

void SmallForward() {
  setMotors(100, 100);
}

void Right() {
  setLeftMotor(-60);   // Left motor slightly reverse
  setRightMotor(150);
}

void Left() {
  setLeftMotor(150);
  setRightMotor(-60);  // Right motor slightly reverse
}

void HardRight() {
  setLeftMotor(-150);
  setRightMotor(150);
}

void HardLeft() {
  setLeftMotor(150);
  setRightMotor(-150);
}

void Reverse() {
  setLeftMotor(-120);
  setRightMotor(-120);
}