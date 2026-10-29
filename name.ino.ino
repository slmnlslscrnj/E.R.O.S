#include <Servo.h>
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

SoftwareSerial softSerial(10, 11);
DFRobotDFPlayerMini myDFPlayer;
Servo myservo;

int ledPins[] = {7, 5, 3};
int val;
int fsr1;
int fsr2;
int state;
int resultIdx = -1;
bool activate = false;
bool pressed1;
bool pressed2;

void setup() {
  Serial.begin(9600);
  softSerial.begin(9600);
  myservo.attach(9);
  myservo.write(90);  //stop servo
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  for (int i = 0; i < 3; i = i + 1) {
    pinMode(ledPins[i], OUTPUT);
  }
  randomSeed(analogRead(A5));

  if (!myDFPlayer.begin(softSerial, false, false)) {
    Serial.println("Error starting DFplayer");
  }
  myDFPlayer.volume(20);
}

void loop() {
  //ping sensor
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  delayMicroseconds(2);
  digitalWrite(2, HIGH);
  delayMicroseconds(5);
  digitalWrite(2, LOW);
  pinMode(2, INPUT);
  val = pulseIn(2, HIGH, 30000);
  val = val / 29 / 2;  //istance in cm 0 if nothing detected

  //object close enough triggers EROS
  if (val > 0 && val < 5) {
    activate = true;
  }

  //pressure sensors
  fsr1 = analogRead(A1);
  fsr2 = analogRead(A3);
  pressed1 = fsr1 > 300;
  pressed2 = fsr2 > 300;
  digitalWrite(12, pressed1);
  digitalWrite(13, pressed2);

  //both pressed while triggered: lights off, spin, then pick a random LED
  if (activate && pressed1 && pressed2) {
    for (int i = 0; i < 3; i = i + 1) {
      digitalWrite(ledPins[i], LOW);
    }

    Serial.print(fsr1);
    Serial.print(",");
    Serial.print(fsr2);
    Serial.println(",2,-1");
    myservo.write(25);  //rotate
    delay(11000);
    myservo.write(90);  //stop the servo
    delay(1000);

    Serial.print(fsr1);
    Serial.print(",");
    Serial.print(fsr2);
    Serial.println(",3,-1");
    myservo.write(155);  //rotate back
    delay(18000);
    myservo.write(90);  //stop the servo
    delay(1000);

    activate = false;

    //pick a random LED and play its sound
    resultIdx = random(0, 3);
    myDFPlayer.play(resultIdx + 1);
    for (int i = 0; i < 3; i = i + 1) {
      digitalWrite(ledPins[i], i == resultIdx);
    }

    Serial.print(fsr1);
    Serial.print(",");
    Serial.print(fsr2);
    Serial.print(",4,");
    Serial.println(resultIdx);
    delay(5000);  //result stays on for 5 seconds

    for (int i = 0; i < 3; i = i + 1) {
      digitalWrite(ledPins[i], LOW);
    }
    resultIdx = -1;
  } else {
    //triggered = all LEDs on, otherwise all off
    for (int i = 0; i < 3; i = i + 1) {
      digitalWrite(ledPins[i], activate);
    }
  }

  state = 0;
  if (pressed1 || pressed2) {
    state = 1;
  }

  Serial.print(fsr1);
  Serial.print(",");
  Serial.print(fsr2);
  Serial.print(",");
  Serial.print(state);
  Serial.print(",");
  Serial.println(resultIdx);
  delay(50);
}