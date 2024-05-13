
   /* 
   * Ein Button wird angeschlossen und entprellt. 
   * Jeder Buttonklick ruft einen Wechsel zu einem neuen Zustand hervor. 
   * Jeder Zustand hat eine eigene Methode. 
   * Ein LCD wird über I2C angeschlossen. 
   * Zu jedem Zustand kann das LCD Informationen anzeigen. 
   */
   

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>  // Include Bounce2 library

LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

enum MachineState {
  MACHINE_FIRST,
  MACHINE_SECOND,
  MACHINE_THIRD,
  MACHINE_FOURTH,
  MACHINE_FIFTH,
  MACHINE_SIXTH,
  MACHINE_COUNT  // Anzahl der Bildschirme
};

MachineState machineState = MACHINE_FIRST;  // Diese Variable speichert den aktuellen Zustand
const int buttonPin = 4;    // die Nummer des Tasters
Bounce debouncer = Bounce(); // Create a Bounce object

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(buttonPin, INPUT_PULLUP); // Set button pin as input with internal pull-up resistor
  debouncer.attach(buttonPin);
  debouncer.interval(10); // Debounce interval in milliseconds
}

void loop() {
  debouncer.update(); // Update the debounce state

  if (debouncer.rose()) { // If button state changes from LOW to HIGH
    machineState = static_cast<MachineState>((machineState + 1) % MACHINE_COUNT); // Switch to next State
  }

  workStates(); // Call workStates() in every loop iteration

  delay(10); // Entprellen des Buttons benötigt etwas Zeit. Warten zum Strom sparen.  
}

void workStates() {
  switch(machineState) {
    case MACHINE_FIRST:
      workStateFirst();
      break;
    case MACHINE_SECOND:
      workStateSecond();
      break;
    case MACHINE_THIRD:
      workStateThird();
      break;
    case MACHINE_FOURTH:
      workStateFourth();
      break;
    case MACHINE_FIFTH:
      workStateFifth();
      break;
    case MACHINE_SIXTH:
      workStateSixth();
      break;
  }
}


void workStateFirst() {
  lcdPrint("This is screen 1", "On a menu system");
}

void workStateSecond() {
  lcdPrint("  Second screen", "of my menu !!");
}

void workStateThird() {
  lcdPrint("Hello, world!", "Third screen (3)");
}

void workStateFourth() {
  lcdPrint("This is screen 4", "Just press btn");
}

void workStateFifth() {
  lcdPrint("   Fifth screen", "i2C LCD screen");
}

void workStateSixth() {
  lcdPrint("THE last screen", "  Sixth and last");
}


void lcdPrint(const char* line1, const char* line2) {
  //lcd.clear(); //  clearing the LCD before printing makes the display flickering
  lcd.setCursor(0, 0); 
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}
