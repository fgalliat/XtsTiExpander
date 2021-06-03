/**
 * XtsTiExpander
 * 
 * Xtase - fgalliat @Jun 2021
 * 
 * Co MCU for Ti92 - V200
 * 
 * see : https://github.com/fgalliat/XtsTiLink_gh.git for XtsTiLink project details
 * 
 * 
 * @@ ESP Wiring
 * 
 * VCC -> /switch/ -> ProMini "RAW" (5v)
 * GND -> GND
 * 3.3 -> Voltage Converter "LV" (low value)
 * 16  -> RX2 -> Voltage Converter "L1" <::> "H1" -> ProMini "TX"
 * 17  -> TX2 -> ProMini "RX"
 **/

#include "ticomm.h"

#define PCSerial Serial
// 16 RX2 - 17 TX2
#define TISerial Serial2

void setup() {
    PCSerial.begin(115200);
    TISerial.begin(115200);
}

void loop() {
    if ( PCSerial.available() > 0 ) {
        while( PCSerial.available() > 0 ) { PCSerial.read(); }
        PCSerial.println("OK let me see if it works....");

        TISerial.print("\\SS\n"); // requires a ScreenDump on Ti
        while( TISerial.available() <= 0 ) { delay(10); }

        const int dumpSize = 240 * 128;
        int cpt = 0;
        while( cpt < dumpSize ) {
            while( TISerial.available() <= 0 ) { delay(2); }
            while( TISerial.available() > 0 ) {
                // not very efficient ... just for test purposes
                PCSerial.write( TISerial.read() );
                cpt++;
            }
        }

    }
    delay(500);
}