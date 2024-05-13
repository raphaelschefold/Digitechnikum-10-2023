
    /* 
     * Eine Realtimeclock ist über ThreeWire angeschlossen
     * Ein Button ist über einen Pin und GND angeschlossen und Bounce2 entprellt. 
     * Jeder Buttonklick ruft einen Wechsel zu einem neuen Zustand hervor. 
     * Jeder Zustand hat eine eigene Methode. 
     * Ein LCD wird über I2C angeschlossen und zeigt im ersten Zustand die Uhrzeit an. 
     * Zu jedem Zustand kann das LCD Informationen anzeigen. 
     */



// Die Bibliotheken
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>  // Include LiquidCrystal_I2C library
#include <Bounce2.h>  // Include Bounce2 library
#include <ThreeWire.h>  // Include Rtc_by_Makuna library
#include <RtcDS1302.h>

// Die Konstanten
const int LCD_ADDRESS = 0x27;
const int LCD_COLUMNS = 16;
const int LCD_ROWS = 2;
const int BUTTON_PIN = 4;
const int DEBOUNCE_INTERVAL = 10; // in Millisekunden

// Die Softwareobjekte
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Bounce debouncer = Bounce(); // Create a Bounce object
ThreeWire myWire(8,7,6); // DAT (I/O), CLK (SCLK), RST (CE) (RTC)
RtcDS1302<ThreeWire> rtc(myWire);
RtcDateTime now; 

// Die Namen der möglichen Zustände
enum MachineState {
  MACHINE_FIRST,
  MACHINE_SECOND,
  MACHINE_THIRD,
  MACHINE_FOURTH,
  MACHINE_FIFTH,
  MACHINE_SIXTH,
  MACHINE_COUNT  // Anzahl der Zustände
};

//Der aktuelle Zustand
MachineState machineState = MACHINE_FIRST;  

// Deklaration der Funktionen
void workStateFirst();
void workStateSecond();
void workStateThird();
void workStateFourth();
void workStateFifth();
void workStateSixth();
void workStates();
void lcdPrint(const char* line1, const char* line2);
void printLCDdateTime(const RtcDateTime& dt);

// Typdefinition des Pointers auf die Liste der Zustandsfunktionen
typedef void (*StateFunction)();

// Zwei Array, mit denen der Zustandsname und die Zustandsfunktion verknüpft werden
MachineState states[] = {MACHINE_FIRST, MACHINE_SECOND, MACHINE_THIRD, MACHINE_FOURTH, MACHINE_FIFTH, MACHINE_SIXTH};
StateFunction stateFunctions[] = {workStateFirst, workStateSecond, workStateThird, workStateFourth, workStateFifth, workStateSixth};

void setup() {

  // LCD starten
  lcd.init();
  lcd.backlight();

  // Button starten
  debouncer.attach(BUTTON_PIN, INPUT_PULLUP); // Set button pin as input with internal pull-up resistor
  debouncer.interval(DEBOUNCE_INTERVAL); // Debounce interval in milliseconds

  // Serielle Schnittstelle starten
  Serial.begin(9600);

  // RealTimeClock starten
  rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Serial.println("START SETUP. Compilation time: ");  
  SerialprintlnDateTime(compiled);

  // RealTimeClock testen
  if (!rtc.IsDateTimeValid()) {
      Serial.println("RTC lost confidence in the DateTime!");
      rtc.SetDateTime(compiled);
  }
  if (rtc.GetIsWriteProtected()){
      Serial.println("RTC was write protected, enabling writing now");
      rtc.SetIsWriteProtected(false);
  }
  if (!rtc.GetIsRunning()){
      Serial.println("RTC was not actively running, starting now");
      rtc.SetIsRunning(true);
  }

  // RealTimeClock auf richtige Uhrzeit stellen
  now = rtc.GetDateTime();
  if (now < compiled) {
      Serial.println("RTC is older than compile time!  (Updating DateTime)");
      rtc.SetDateTime(compiled);
  }
  else if (now > compiled) {
      Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled) {
      Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
  Serial.println("Startup finished. Now: "); 
  SerialprintlnDateTime(now); 
}



void loop() {

  // Buttonstatus updaten
  debouncer.update(); // Update the debounce state

  // aktuelle Uhrzeit holen
  now = rtc.GetDateTime();

  // Uhrzeit testen
  if (!now.IsValid()) {
      Serial.println("RTC lost confidence in the DateTime!");
      rtc.Begin(); // Versuche, die RTC neu zu starten, falls die Zeit ungültig ist
  }

  // auf Buttonklick reagieren
  if (debouncer.rose()) { // If button state changes from LOW to HIGH
    machineState = static_cast<MachineState>((machineState + 1) % MACHINE_COUNT); // Switch to next State
    lcd.clear();          
  }

  // Bearbeitung gemäß Buttonklick fortsetzen
  stateFunctions[machineState]();

  // ganz kurz warten zum Strom sparen
  delay(10);
}


void workStateFirst() {
  printLCDdateTime(now);
  //delay(50); 
}

void workStateSecond() {
  lcdPrint("Second screen   ", "of my menu (2)  ");
  //delay(1000);
}

void workStateThird() {
  lcdPrint("Hello, world!   ", "Third screen (3)");
  //delay(1000);
}

void workStateFourth() {
  lcdPrint("This is screen 4", "Just press btn  ");
  //delay(1000);
}

void workStateFifth() {
  lcdPrint("Fifth screen of ", "I2C LCD screen  ");
  //delay(1000);
}

void workStateSixth() {
  lcdPrint("The last screen ", "Sixth and last  ");
  //delay(1000);
}

// Zwei Zeilen auf LCD ausgeben
void lcdPrint( const char* line1, const char* line2) {
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

// Uhrzeit und Datum auf LCD
void printLCDdateTime(const RtcDateTime& dt){
    char line1[LCD_COLUMNS];
    char line2[LCD_COLUMNS];
    char dayW[7][3] = {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
    int d = dt.DayOfWeek();  
    snprintf_P(line1, LCD_COLUMNS, PSTR("    %02u:%02u:%02u"), dt.Hour(), dt.Minute(), dt.Second());
    snprintf_P(line2, LCD_COLUMNS, PSTR(" %s, %02u.%02u.%04u"), dayW[d] , dt.Day(), dt.Month(), dt.Year());
    lcd.setCursor(0, 0);
    lcd.print((String)line1);
    lcd.setCursor(0, 1);
    lcd.print((String)line2);
}

// Präcompiler Hilfsfunktion 
#define countof(a) (sizeof(a) / sizeof(a[0]))

// Uhrzeit und Datum auf seriellem Monitor
void SerialprintlnDateTime(const RtcDateTime& dt){
    char datestring[25];
    char daysOfTheWeek[7][4] = {"So","Mo", "Di", "Mi", "Do", "Fr", "Sa"};
    snprintf_P(datestring, 
        countof(datestring),
        PSTR("%3s, %02u.%02u.%04u %02u:%02u:%02u"),
        daysOfTheWeek[dt.DayOfWeek()],
        dt.Day(),
        dt.Month(),
        dt.Year(),
        dt.Hour(),
        dt.Minute(),
        dt.Second() );
    Serial.println(datestring);
}
