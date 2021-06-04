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

bool enterDummyMode() {
  PCSerial.println("(ii) TiComm entered in Dummy Mode");
  bool inDummy = true;
  while(inDummy) {
      // FIXME : do better
      while ( PCSerial.available() > 0 ) {
          TISerial.write( PCSerial.read() );
      }

      while ( TISerial.available() > 0 ) {
          if ( TISerial.peek() == IN_BIN_EXIT_DUMMY[0] ) {
              TISerial.read();
              if ( TISerial.peek() == IN_BIN_EXIT_DUMMY[1] ) {
                TISerial.read();
                PCSerial.println("(ii) TiComm exited from Dummy Mode");
                inDummy = false;
                break;
              }
              PCSerial.write( TISerial.read() );
          }
          PCSerial.write( TISerial.read() );
      }
  }
  return true;  
}

bool enterRecvVarMode() {
  if ( TISerial.available() >= 1 ) {
      if ( TISerial.peek() == IN_BIN_SENDVAR_NAME[2] ) {
        TISerial.read();

        char varName[8+1]; memset( varName, 0x00, 8+1 );
        for(int i=0; i < 8; i++) {
            int ch = TISerial.read();
            if ( ch == -1 ) { return false; } // not enough bytes
            if ( ch == 0 ) { break; } // end of VarName
            varName[i] = (char)ch;
        }
        PCSerial.print("(ii) TiComm send a variable : "); PCSerial.println( varName );

        while( TISerial.available() <= 0 ) { delay(2); }

        if ( TISerial.available() >= 3+1 ) {
            uint8_t typeDef[3+1];
            TISerial.readBytes( typeDef, 3+1 );
            if ( strncmp( (char*)typeDef, IN_BIN_SENDVAR_TYPE, 3 ) != 0 ) {
                return false;
            }
            uint8_t varType = typeDef[3];
            PCSerial.print("(ii) TiComm send a variable of type : "); PCSerial.println( varType, HEX );

            while( TISerial.available() <= 0 ) { delay(2); }

            if ( TISerial.available() >= 3+4 ) {
                uint8_t sizeDef[3+4];
                TISerial.readBytes( sizeDef, 3+4 );
                if ( strncmp( (char*)sizeDef, IN_BIN_SENDVAR_SIZE, 3 ) != 0 ) {
                    return false;
                }
                uint32_t varSize = ( sizeDef[3+0] << 24 ) + ( sizeDef[3+1] << 16 ) + ( sizeDef[3+2] << 8 ) + ( sizeDef[3+3] );
                PCSerial.print("(ii) TiComm send a variable of size : "); PCSerial.println( varSize );

                while( TISerial.available() <= 0 ) { delay(2); }

                if ( TISerial.available() >= 3 ) {
                    uint8_t dataDef[3];
                    TISerial.readBytes( dataDef, 3 );
                    if ( strncmp( (char*)dataDef, IN_BIN_SENDVAR_DATA, 3 ) != 0 ) {
                        return false;
                    }

                    for(uint32_t i=0; i < varSize; i++) {
                        // FIXME : do better
                        while( TISerial.available() <= 0 ) { delay(2); }
                        PCSerial.println( TISerial.read(), HEX );
                    }

                    while( TISerial.available() <= 0 ) { delay(2); }
                    if ( TISerial.available() >= 3 ) {
                        uint8_t eofDef[3];
                        TISerial.readBytes( eofDef, 3 );
                        if ( strncmp( (char*)eofDef, IN_BIN_SENDVAR_EOF, 3 ) != 0 ) {
                            return false;
                        }
                        PCSerial.println("(ii) TiComm send a variable : EOF ");
                        return true; // No errors
                    }
                }

            }
        }

      }
  }
  return false;
}

bool recvCBLValue() {
  if ( TISerial.available() >= 2 ) {
      uint8_t value[2];
      TISerial.readBytes( value, 2 ); // MSB - LSB
      uint16_t iValue = (value[0] * 256) + ( value[1] );
      PCSerial.print("(ii) TiComm sent a value from CBL : "); PCSerial.println( iValue );
      return true;
  }
  return false;
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

    if ( TISerial.available() > 0 ) {
        handleTiComm();
    }


    delay(10);
}