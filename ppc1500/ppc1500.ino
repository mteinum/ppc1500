/*
   PPC 1500 TIMER UNIT

   morten.teinum@gmail.com

   "line is ready" [3s] [beep]             [beep]
                        [ .... count down  .... ]

   Hardware:
   
   Arduino Nano Board
   Grove Shield for Arduino Nano
   Grove - Relay (D6)
   Grove Dual Button (D4)
   Grove - 4-Digit Display (D2)
   12V relay> 896-1AH-D1

   Controls:

   CycleButton:
   0: Reset, display: PPC
   1:   8 sec
   2:  12 sec
   3:  20 sec
   4:  35 sec
   5:  90 sec
   6: 165 sec

   Start:
   Starts the selected program.
*/

#include "TM1637.h"

const int CLK = 2;
const int DIO = 3;
TM1637 tm1637(CLK, DIO);

int cycleButton = 4;
int startButton = 5;
int relay = 6;

const int beepTime = 2000;
const int programTimes[] = {0, 8, 12, 20, 35, 90, 165};

// program state
int selectedProgram = 0;
int previousCycleButtonState = HIGH;
int previousStartButtonState = HIGH;

void displayPpc() {
  int8_t timeDisplay[] = {0x7f, 'P', 'P', 'C'};
  tm1637.display(timeDisplay);
}

void setup() {
  Serial.begin(9600);
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  pinMode(cycleButton, INPUT);
  pinMode(startButton, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relay, OUTPUT);

  displayPpc();
}

void setSelectedProgram(int value) {
  selectedProgram = value % 7;

  if (selectedProgram == 0) {
    displayPpc();
  } else {
    tm1637.displayNum(programTimes[selectedProgram]);
  }
}

void beepOn() {
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(relay, HIGH);
}

void beepOff() {
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(relay, LOW);
}

void loopChangeProgram() {
  setSelectedProgram(selectedProgram + 1);
}

void loopStartProgram() {
  if (!selectedProgram)
    return;

  // "LINE IS READY"
  // 3 sec

  delay(3000);

  int beepTime = 1;
  int remainingSeconds = programTimes[selectedProgram] - 1;

  beepOn();

  for (int i = 0; remainingSeconds >= 0; remainingSeconds--, i++) {

    // first off
    if (i == beepTime) {
      beepOff();
    }

    delay(1000);

    tm1637.displayNum(remainingSeconds);

    // last on
    if (remainingSeconds == beepTime) {
      beepOn();
    }
  }

  beepOff();

  setSelectedProgram(0);

  delay(100);
}

void loop() {

  int reading = digitalRead(cycleButton);

  if (reading != previousCycleButtonState) {
    if (reading == LOW) {
      loopChangeProgram();
    }

    previousCycleButtonState = reading;
  }

  reading = digitalRead(startButton);

  if (reading != previousStartButtonState) {

    if (reading == LOW) {
      loopStartProgram();
    }

    previousStartButtonState = reading;
  }
}
