#include <Servo.h>
#define START_BUTTON_PIN A0
#define ENA 6
#define ENB 5
#define BUZZER_PIN 7
#define TRIGGER_PIN 4
#define ECHO_PIN 3
#define SERVO_PIN 9
#define MIN_DISTANCE 25 
#define MAX_DISTANCE 100

#define BACKWARD HIGH, LOW, HIGH, LOW
#define FORWARD LOW, HIGH, LOW, HIGH
#define RIGHT HIGH, LOW, LOW, HIGH
#define LEFT LOW, HIGH, HIGH, LOW 
#define STOP LOW, LOW, LOW, LOW

const byte motorPins[] = { 13, 12, 11, 10 };
Servo myServo;

float distance, frontDistance, leftDistance, rightDistance;
float duration;

void setup() {
  for (int i = 0; i < 4; i++) pinMode(motorPins[i], OUTPUT);

  pinMode(START_BUTTON_PIN, INPUT_PULLUP);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  analogWrite(ENA, 255);
  analogWrite(ENB, 255);

  myServo.attach(SERVO_PIN);
  myServo.write(90);

  Serial.begin(9600);

  startupTone();
}

void loop() {
  if (digitalRead(START_BUTTON_PIN) == LOW) {
    frontDistance = getDistance();
   
    takeAction(frontDistance);
  } else {
    move(STOP, 0, 0);
  }
}

void move(bool a, bool b, bool c, bool d, int speed, int moveTime) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(motorPins[0], a);
  digitalWrite(motorPins[1], b);
  digitalWrite(motorPins[2], c);
  digitalWrite(motorPins[3], d);
  delay(moveTime);
}

void takeAction(int frontDistance) {
  if (frontDistance >= MIN_DISTANCE) {
    int speed = map(frontDistance, MIN_DISTANCE, MAX_DISTANCE, 80, 150);
    Serial.print("Moving Forward at Speed: ");
    Serial.println(speed);
    move(FORWARD, 150, 0);
  } else {
    if (frontDistance <= 15 && frontDistance >= 2) {
      move(BACKWARD, 140, 400);
    }

    Serial.print("Obstacle Detected : ");
    Serial.println(frontDistance);
    move(STOP, 0, 0);

    startupTone();

    // check right
    myServo.write(0);
    rightDistance = getDistance();
    delay(1000);
    // check left
    myServo.write(180);
    leftDistance = getDistance();
    delay(1000);
    // servo back to front
    myServo.write(90);
    delay(1000);

    Serial.print("left: ");
    Serial.println(leftDistance);
    Serial.print("right: ");
    Serial.println(rightDistance);

    if (leftDistance >= MIN_DISTANCE || rightDistance >= MIN_DISTANCE) {
      if (abs(leftDistance - rightDistance) < 5) {
        Serial.println("Almost equal distances, turning randomly!");
        move(random(0, 2) ? LEFT : RIGHT, 140, 450);
      } else if (leftDistance > rightDistance) {
        Serial.println("Turning Left..");
        move(LEFT, 140, 450);
      } else if (rightDistance > leftDistance) {
        Serial.println("Turning Right..");
        move(RIGHT, 140, 450);
      }
    }else{
      Serial.println("Rotating 180..°");  
      move(RIGHT, 140, 2000);
    }
    move(STOP, 0, 150);
  }
}

float getDistance() {
  // distance = 0;
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration * 0.034)/2;
  return distance;
}

void startupTone() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(400);
  digitalWrite(BUZZER_PIN, LOW);
}
