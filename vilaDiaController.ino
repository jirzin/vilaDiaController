#include "IntervalType.h"
#include <Servo.h>
#include <EEPROM.h>

IntervalType i01;
IntervalType i02;
IntervalType i03;

unsigned long i02_TimeOffset = 250;
unsigned long m;

int servoPositions[] = {90, 130};

int steps = 0; // actual diaprojector step position
int maxSteps = 10; // top olimit of frames in stack

enum global_state {start, front, back, conf}; // global program states
enum global_state s = start; // actual states


bool confEnabled = true; // for safe switching to conf state

// memory addresses for configurable values
byte maxStepsAddr = 0;
byte slideIntervalAddr = 1;

// DEBUG
bool debug = true;
int initStep;

void setup() {
  // indidivual interval setups
  i01.interval = 10000;
  i02.interval = 10000;
  i03.interval = 2500;

  // debugging mode starts serial link
  if (debug) {
    Serial.begin(9600);
    while (!Serial);
    if (Serial) {
      randomSeed(analogRead(A0));
      initStep = random(9);
      Serial.print("hello...");
      Serial.print(s);
      Serial.println();
    };
  }
}

void loop() {
  // get actual time in millis from power up
  m = millis();
  
  switch (s) {
    case start:
      // program reaches the first slide in stack 
      // than it switches to fron & back operation
      execInInterval(m, &i01, &startSearchPush);
      execInInterval(m - i02_TimeOffset, &i02, &startSearchRelease);

      // there has to be implemented function that checks if stack is in its base position
      // possible HW variants: plain contact, switch or proximity sensor ar opt gate
      if (initStep == 0) {
        s = front;
        steps = 0;
      }
      break;
    case front:
      // program is going forward in stack
      // if it reaches the maximum nuber of slides operation switches to back direction
      execInInterval(m, &i01, &buttonPush);
      execInInterval(m - i02_TimeOffset, &i02, &buttonRelease);
      if (steps >= maxSteps - 1) {
        s = back;
      }
      break;
    case back:
    // program is going backward in stack
    // if it reaches zero operation switches to front direction
      execInInterval(m, &i01, &buttonPush);
      execInInterval(m - i02_TimeOffset, &i02, &buttonRelease);
      if (steps <= 0) {
        s = front;
      }
      break;
    case conf:
      // in conf operation user should be able to configure and save to memory
      // values of certain variables that should be changing in future
      // known variables are:
      // - number of slides in stack
      // - interval for one slide - in seconds
      // - interval offset for switch release
      break;
  }
  if (debug) {
    execInInterval(m, &i03, &printStat);
  }
}

void buttonPush () {
  if (debug) {
    Serial.println("Zmacknu tlacitko na posun diaku");
  }
  confEnabled = false;
}
void buttonRelease () {
  if (debug) {
    Serial.println("Pustim tlacitko na posund diaku");
  }
  switch (s) {
    case front:
      steps++;
      break;
    case back:
      steps--;
      break;
  }
  confEnabled = true;
}

void startSearchPush() {
  if (debug) {
    Serial.println("Zmacknu tlacitko na posun diaku -> start seq");
  }
  confEnabled = false;
}

void startSearchRelease() {
  if (debug) {
    Serial.println("Pustim tlacitko na posun diaku -> start seq");
  }
  initStep--;
  confEnabled = true;
}

void printStat() {
  Serial.print("initSteps: ");
  Serial.print(initStep);
  Serial.print(" | steps: ");
  Serial.print(steps);
  Serial.print(" | state: ");
  Serial.print(s);
  Serial.println();
}

