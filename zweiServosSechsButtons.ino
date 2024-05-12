#include <Servo.h>
#include <OneButton.h>

// Definition der Pins für Servos
enum ServoPins {
  SP1 = 3,
  SP2 = 4
};

// Definition der Pins für Tasten
enum ButtonPins {
  BP1 = 5,
  BP2 = 6,
  BP3 = 7,
  BP4 = 8,
  BP5 = 9,
  BP6 = 10
};

// Definition der Sollwinkel
#define S1_ANGLE_30 30
#define S1_ANGLE_60 60
#define S2_ANGLE_30 30
#define S2_ANGLE_45 45
#define S2_ANGLE_60 60

// Definition der Anzahl der Zustände
#define NUM_STATES 6

// Definition der Zustände
enum States {
  Z1, Z2, Z3, Z4, Z5, Z6
};

// Definition der Sollwinkel-Arrays für Servos
const int servo1Angles[] = {S1_ANGLE_30, S1_ANGLE_60};
const int servo2Angles[] = {S2_ANGLE_30, S2_ANGLE_45, S2_ANGLE_60};

// Servo-Objekte erstellen
Servo servo1;
Servo servo2;

// Button-Objekte erstellen
OneButton buttons[NUM_STATES];

// Aktueller Zustand
States currentState = Z1;

// Globale Istwinkel der Servos
int currentServoAngles[] = {0, 0};

// Globale Sollwinkel der Servos
int targetServoAngles[] = {0, 0};

// Schrittgröße für die Servowinkeländerung
#define ANGLE_STEP 2

// Funktion zur Aktualisierung des Zustands
void updateState(States state) {
  currentState = state;
  switch (state) {
    case Z1:
      targetServoAngles[0] = servo1Angles[0];
      targetServoAngles[1] = servo2Angles[0];
      break;
    case Z2:
      targetServoAngles[0] = servo1Angles[0];
      targetServoAngles[1] = servo2Angles[1];
      break;
    case Z3:
      targetServoAngles[0] = servo1Angles[0];
      targetServoAngles[1] = servo2Angles[2];
      break;
    case Z4:
      targetServoAngles[0] = servo1Angles[1];
      targetServoAngles[1] = servo2Angles[0];
      break;
    case Z5:
      targetServoAngles[0] = servo1Angles[1];
      targetServoAngles[1] = servo2Angles[1];
      break;
    case Z6:
      targetServoAngles[0] = servo1Angles[1];
      targetServoAngles[1] = servo2Angles[2];
      break;
  }
}

// Funktion zur Aktualisierung der Servo-Winkel
void updateServoAngles() {
  for (int i = 0; i < 2; i++) {
    if (currentServoAngles[i] != targetServoAngles[i]) {
      int direction = currentServoAngles[i] < targetServoAngles[i] ? 1 : -1;
      currentServoAngles[i] = constrain(currentServoAngles[i] + direction * ANGLE_STEP, 0, 180);
      Servo& servo = (i == 0) ? servo1 : servo2;
      servo.write(currentServoAngles[i]);
    }
  }
}

// Funktion, die aufgerufen wird, wenn ein Button geklickt wird
void onButtonClick(void *state) {
  States s = *(States *)state;
  updateState(s);
}

void setup() {
  // Servos an den entsprechenden Pins befestigen
  servo1.attach(SP1);
  servo2.attach(SP2);

  // Buttons initialisieren
  for (int i = 0; i < NUM_STATES; i++) {
    buttons[i] = OneButton(i + BP1, true); // true für Input-Pullup
  }

  // Attach Click-Funktion für alle Buttons
  for (int i = 0; i < NUM_STATES; i++) {
    States state = (States)i; // Hier wird der aktuelle Zustand für diesen Button gespeichert
    buttons[i].attachClick(onButtonClick, &state);
  }

  // Anfangszustand einstellen
  updateState(currentState);
}

void loop() {
  // Buttons aktualisieren
  for (int i = 0; i < NUM_STATES; i++) {
    buttons[i].tick();
  }

  // Winkeländerung für die Servos
  updateServoAngles();
}
