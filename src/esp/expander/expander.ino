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

#include "globals.h"

#include "screen.h"
#include "storage.h"

#include "ticomm.h"



void setup() {
    PCSerial.begin(115200);
    TISerial.begin(115200);

    setupScreen();
    setupStorage();
}

void handleTiComm() {
  bool validPacket = false;
  if ( TISerial.available() >= 2 ) {
      uint8_t head[2];
      TISerial.readBytes( head, 2 );

      if ( strncmp( (char*)head, IN_BIN_ENTER_DUMMY, 2 ) == 0 ) {
          validPacket = enterDummyMode();
      } else if ( strncmp( (char*)head, IN_BIN_SENDVAR_NAME, 2 ) == 0 ) {
          validPacket = enterRecvVarMode();
      }  else if ( strncmp( (char*)head, IN_BIN_SENDCBL, 2 ) == 0 ) {
          validPacket = recvCBLValue();
      } 
  }

  if ( !validPacket ) {
      PCSerial.println("(!!) Unknown TiComm Packet");
  }
}


void loop() {
    if ( PCSerial.available() > 0 ) { // something from PC
        while( PCSerial.available() > 0 ) { PCSerial.read(); }
        PCSerial.println("OK let me see if it works....");

        int error = 0;
        File f = createTiFile("hello", 0x1C, error);
        if ( error != 0 ) {
            PCSerial.println("There was a WRITE error !");
        } else {
            f.println( "Hello World" );
            f.close();
        }

        listRoot();

        // TISerial.print("\\SS\n"); // request a ScreenDump on Ti (ascii mode)
        // while( TISerial.available() <= 0 ) { delay(10); }

        // const int dumpSize = 240 * 128;
        // int cpt = 0;
        // while( cpt < dumpSize ) {
        //     while( TISerial.available() <= 0 ) { delay(2); }
        //     while( TISerial.available() > 0 ) {
        //         // not very efficient ... just for test purposes
        //         PCSerial.write( TISerial.read() );
        //         cpt++;
        //     }
        // }

    }

    if ( TISerial.available() > 0 ) { // something from TiComm
        handleTiComm();
    }


    delay(10);
}