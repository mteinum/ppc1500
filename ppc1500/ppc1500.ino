/*
  PPC1500 program for Arduino Micro
  morten.teinum@gmail.com

  Requirements:
  1. board: https://www.kjell.com/no/produkter/elektro-og-verktoy/arduino/utviklingskort/arduino-micro-utviklingskort-p87112
  3 leds
  3 buttons
  1 relay: https://www.kjell.com/no/produkter/elektro-og-verktoy/arduino/moduler/relemodul-for-arduino-1x-p87032

  Revision
  2021-09-18, v1

*/

const int cycleButtonPin = 3;
const int startButtonPin = 4;
const int relayPin = 5;

// beep duration and program duration
const int beepTime = 2000;
const int programTimes[] = {0, 8, 12, 20, 35, 90, 165};
// program state
int selectedProgram = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("setup");

  pinMode(cycleButtonPin, INPUT);
  pinMode(startButtonPin, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relayPin, OUTPUT);

  digitalWrite(LED_BUILTIN, HIGH); // ready/reset
}

void beep() {
  digitalWrite(relayPin, HIGH);
  delay(beepTime);
  digitalWrite(relayPin, LOW);
}

void onOff(int ms) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(ms);
  digitalWrite(LED_BUILTIN, LOW);
  delay(ms);
}

void setSelectedProgram(int value) {

  selectedProgram = value % 7;

  Serial.println("setSelectedProgram(" + String(selectedProgram) + ")");

  if (selectedProgram == 0) {
    // reset mode
    digitalWrite(relayPin, LOW);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);

    for (int i = 0; i < selectedProgram; i++) {
      onOff(500);
    }
  }
}

uint8_t lastCycleButtonState = LOW;
uint8_t lastStartButtonState = LOW;

void loop() {

  int reading = digitalRead(cycleButtonPin);

  if (reading != lastCycleButtonState) {
    if (reading == HIGH) {
      setSelectedProgram(selectedProgram + 1);
    }

    lastCycleButtonState = reading;
  }

  reading = digitalRead(startButtonPin);

  if (reading != lastStartButtonState) {
    if (reading == HIGH && selectedProgram) {

      int programDuration = programTimes[selectedProgram] * 1000;
      int sleepTime = programDuration - beepTime * 2;
      
      Serial.println("Starting program " + String(selectedProgram));
      Serial.println("Beep (" + String(beepTime) + ") - Sleep (" + String(sleepTime) + ") - Beep(" + String(beepTime) + ")");

      beep();
      delay(sleepTime);
      beep();
      setSelectedProgram(0);
    }

    lastStartButtonState = reading;
  }
}
