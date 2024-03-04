// Anzeige der Uhrzeit der RTC auf dem I2C-LCD
// R. Schefold am 29.2.2024

#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the I2C-LCD address to 0x27 for a 16 chars and 2 line display
ThreeWire myWire(8,7,6); // DAT (I/O), CLK (SCLK), RST (CE) (RTC)
RtcDS1302<ThreeWire> rtc(myWire);

void setup() {

    //LCD
    lcd.init();
    lcd.clear();         
    lcd.backlight();      // Make sure backlight is on
    // Print a message on both lines of the LCD.
    lcd.setCursor(2,0);   //Set cursor to character 2 on line 0
    lcd.print("Hello world!");
    lcd.setCursor(2,1);   //Move cursor to character 2 on line 1
    lcd.print("LCD Tutorial");
  

    //RTC
    Serial.begin(9600);
    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println(__TIME__);

    rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!rtc.IsDateTimeValid()) {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

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
    RtcDateTime now = rtc.GetDateTime();
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
}

void loop () {
    RtcDateTime now = rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    if (!now.IsValid()) {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    delay(1000); //  seconds
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt){
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
    Serial.print(datestring);

    String strg = datestring; 
    lcd.clear();         
    lcd.setCursor(0,1);   //Move cursor to character 0 on line 1
    lcd.print(strg.substring(0, 15));
    lcd.setCursor(4,0);   //Move cursor to character 4 on line 0
    lcd.print(strg.substring(16, 25));
}
