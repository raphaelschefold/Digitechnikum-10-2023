#include <ThreeWire.h>  
#include <RtcDS1302.h>

ThreeWire myWire(8,7,6); // DAT (I/O), CLK (SCLK), RST (CE)
RtcDS1302<ThreeWire> rtc(myWire);

void setup () {
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
    char daysOfTheWeek[7][4] = {"Sun","Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

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
}
