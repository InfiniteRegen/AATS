#include <Servo.h>

#define ULTRA_MAX_CM            100
#define CHANGE_RANGE            5
#define SHOOTING_RANGE          17
#define DIRECT_SHOOTING_RANGE   8

/*==[ Degree of each Sensor ]==*/
const int angleMap[5] = {0, 45, 90, 135, 180};

/*==[ PIN NUMBER DEFINITION ]==*/
int trigPin[5] = {19, 3, 5, 7, 9}, echoPin[5] = {2, 4, 6, 8, 12};  // Ultrasonic Sensor
int laserPin = 13;        // Laser
int speedA = 11;          // DC Motor speed control (rotate)

int rotatePin1 = 17;      // DC Motor (rotate)
int rotatePin2 = 18;      // DC Motor (rotate)
int dirPinA1 = 15;        // DC Motor (fire)
int dirPinA2 = 16;        // DC Motor (fire)
int speedB = 10;          // DC Motor speed control (fire)

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

int init_t = 1;

int pre_angle;              // previous angle
int angle = 0;
int strLen = 0;
int exponent = 1;
long changeDistance = 0;    // | distance - preDistance | ( Evaluation for Change Detection )
long distance[5] = {0, };     // Current Distance
static long preDistance[5]; // Previous Distance

int i = 0;

/*==[ ULTRA SONIC SENSOR VARIABLE ]==*/
void setup()
{
  Serial.begin(115200);
  inputString.reserve(500);

  /* US INIT */
  USInit(); // Ultra Sonic Initalization -> (pinMode(trigPin[i],OUTPUT), pinMode(echoPin[i],INPUT))

  /* LASER INIT */
  pinMode(laserPin, OUTPUT);

  /* DC MOTOR (rotate) INIT */
  pinMode(rotatePin1, OUTPUT);
  pinMode(rotatePin2, OUTPUT);
  digitalWrite(rotatePin1, LOW);
  digitalWrite(rotatePin2, LOW);

  /* DC MOTOR (fire) INIT */
  pinMode(dirPinA1, OUTPUT);
  pinMode(dirPinA2, OUTPUT);
  digitalWrite(dirPinA1, LOW);
  digitalWrite(dirPinA2, LOW);

  pre_angle = 90;
}

void loop()
{
  if (Serial.available()) {
        delay(1000);
        Serial.write("6, 7");
        digitalWrite(dirPinA1, HIGH); // fire (forward rotation)
        digitalWrite(dirPinA2, LOW);
        analogWrite(speedB, 255);
        delay(1300); // One shot:600, Go Go Go:1300
        digitalWrite(dirPinA1, LOW);  // original position (Reverse rotation)
        digitalWrite(dirPinA2, HIGH);
        analogWrite(speedB, 255);
        delay(300);
        digitalWrite(dirPinA1, LOW);
        digitalWrite(dirPinA2, LOW);
  }

  for (i = 0; i < 5; ++i) {
    distance[i] = US(trigPin[i]);
  }

  for (i = 0; i < 5; ++i) {
    changeDistance = abs(distance[i] - preDistance[i]);

    if (distance[i] == -1 || preDistance[i] == -1)
        changeDistance = 0;   // 불안정한 값 예외처리

    preDistance[i] = distance[i];
    if (changeDistance >= CHANGE_RANGE && distance[i] <= SHOOTING_RANGE) {// 경고 범위 안으로 들어왔을 때

      Serial.print(i + 1);
      Serial.print(",");
      Serial.println(distance[i]);

      digitalWrite(laserPin, HIGH);     // Laser On

      angle = angleMap[i];

      if (pre_angle == 0) {
        if (angle == 0) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 45) {
          digitalWrite(rotatePin1, HIGH);
          digitalWrite(rotatePin2, LOW);
          analogWrite(speedA, 255);
          delay(115);
          digitalWrite(rotatePin1, LOW);
        }
        else if (angle == 90) {
          digitalWrite(rotatePin1, HIGH);
          digitalWrite(rotatePin2, LOW);
          analogWrite(speedA, 255);
          delay(240);
          digitalWrite(rotatePin1, LOW);
        }
        else if (angle == 135) {
          digitalWrite(rotatePin1, HIGH);
          digitalWrite(rotatePin2, LOW);
          analogWrite(speedA, 255);
          delay(375);
          digitalWrite(rotatePin1, LOW);
        }
        else if (angle == 180) {
          digitalWrite(rotatePin1, HIGH);
          digitalWrite(rotatePin2, LOW);
          analogWrite(speedA, 255);
          delay(500);
          digitalWrite(rotatePin1, LOW);
        }
      }
      else if (pre_angle == 45) {
        if (angle == 0) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, HIGH);
          analogWrite(speedA, 255);
          delay(115);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 45) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 90) {
          digitalWrite(rotatePin1, HIGH);
          digitalWrite(rotatePin2, LOW);
          analogWrite(speedA, 255);
          delay(115);
          digitalWrite(rotatePin1, LOW);
        }
        else if (angle == 135) {
          digitalWrite(rotatePin1, HIGH);
          digitalWrite(rotatePin2, LOW);
          analogWrite(speedA, 255);
          delay(240);
          digitalWrite(rotatePin1, LOW);
        }
        else if (angle == 180) {
          digitalWrite(rotatePin1, HIGH);
          digitalWrite(rotatePin2, LOW);
          analogWrite(speedA, 255);
          delay(375);
          digitalWrite(rotatePin1, LOW);
        }
      }
      else if (pre_angle == 90) {
        if (angle == 0) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, HIGH);
          analogWrite(speedA, 255);
          delay(240);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 45) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, HIGH);
          analogWrite(speedA, 255);
          delay(115);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 90) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 135) {
          digitalWrite(rotatePin1, HIGH);
          digitalWrite(rotatePin2, LOW);
          analogWrite(speedA, 255);
          delay(115);
          digitalWrite(rotatePin1, LOW);
        }
        else if (angle == 180) {
          digitalWrite(rotatePin1, HIGH);
          digitalWrite(rotatePin2, LOW);
          analogWrite(speedA, 255);
          delay(240);
          digitalWrite(rotatePin1, LOW);
        }
      }
      else if (pre_angle == 135) {
        if (angle == 0) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, HIGH);
          analogWrite(speedA, 255);
          delay(375);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 45) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, HIGH);
          analogWrite(speedA, 255);
          delay(240);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 90) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, HIGH);
          analogWrite(speedA, 255);
          delay(115);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 135) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 180)
        {
          digitalWrite(rotatePin1, HIGH);
          digitalWrite(rotatePin2, LOW);
          analogWrite(speedA, 255);
          delay(115);
          digitalWrite(rotatePin1, LOW);
        }
      }
      else if (pre_angle == 180) {
       if (angle == 0) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, HIGH);
          analogWrite(speedA, 255);
          delay(500);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 45) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, HIGH);
          analogWrite(speedA, 255);
          delay(375);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 90) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, HIGH);
          analogWrite(speedA, 255);
          delay(240);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 135) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, HIGH);
          analogWrite(speedA, 255);
          delay(115);
          digitalWrite(rotatePin2, LOW);
        }
        else if (angle == 180) {
          digitalWrite(rotatePin1, LOW);
          digitalWrite(rotatePin2, LOW);
        }
      }

      pre_angle = angle;

      if (distance[i] <= DIRECT_SHOOTING_RANGE) {
        digitalWrite(dirPinA1, HIGH); // fire (Forward Rotation)
        digitalWrite(dirPinA2, LOW);
        analogWrite(speedB, 255);
        delay(1300); // One shot:600, Go Go Go:1300
        digitalWrite(dirPinA1, LOW);  // original position (Reverse Rotation)
        digitalWrite(dirPinA2, HIGH);
        analogWrite(speedB, 255);
        delay(300);
        digitalWrite(dirPinA1, LOW);  // stop
        digitalWrite(dirPinA2, LOW);
      }
      digitalWrite(laserPin, LOW);   // Laser Off
    }
    /* preDistance[i] = { 0, } 초기화
    changeDistance를 배열로 만들어서 저장 (changeDistance[i]) -> 상관없을듯
    delayMicroseconds(숫자) -> 숫자 좀더 크게
    첫번째 for문에 delay넣기 */
  }

   if (stringComplete) {
    /* String to Integer */
    strLen = inputString.length();
    Serial.print(inputString);
   }

    delay(2000);
}

/* UltraSonic sensor Handle */
long US(int pinNum)
{
  int trPin = pinNum;
  int ecPin = pinNum + 1;

  if (pinNum == 9)
    ecPin = 12;
  if (pinNum == 19)
    ecPin = 2;

  digitalWrite(trPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trPin, HIGH); // Trigger pin to HIGH
  delayMicroseconds(10); // 10us high
  digitalWrite(trPin, LOW); // Trigger pin to HIGH

  long Duration = pulseIn(ecPin, HIGH); // Waits for the echo pin to get high, it returns the Duration in microseconds

  long Distance_cm = Duration / 29 / 2;

  delayMicroseconds(200); // Wait to do next measurement

  /* Sensor Maximum Range Set */
  if (Distance_cm > ULTRA_MAX_CM)
    return -1; // calling routine has to handle '-1'.

  return Distance_cm;
}

void USInit()
{
   for (int i = 0; i < 5; ++i) {
      pinMode(trigPin[i], OUTPUT);
      pinMode(echoPin[i], INPUT);
   }
}

void serialEvent()
{
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;

    if (inChar == '\n')
      stringComplete = true;
  }
}
