#ifndef __TICOMM_H__
#define __TICOMM_H__ 1

/**
 * XtsTiExpander
 * 
 * Xtase - fgalliat @Jun 2021
 * 
 * XtsTiLink Serial protocol
 */

 // ========= Comm header BYTES Section =========

 // DummyMode
 #define IN_BIN_ENTER_DUMMY "\x89\xFE"
 #define IN_BIN_EXIT_DUMMY  "\x89\xFF"

 // Ti Send Var
 // + name 0-terminated
 #define IN_BIN_SENDVAR_NAME "\x89\x06\x01"
 // + type 1 byte
 #define IN_BIN_SENDVAR_TYPE "\x89\x06\x02"
 // + size 32b MSB-LSB
 #define IN_BIN_SENDVAR_SIZE "\x89\x06\x03"
 // + x(size) Data bytes
 #define IN_BIN_SENDVAR_DATA "\x89\x06\x04"
 // just EOF
 #define IN_BIN_SENDVAR_EOF  "\x89\x06\x05"

 // Ti Send CBL Value(s)
 // + sent value 16b MSB-LSB
 #define IN_BIN_SENDCBL "\x89\xCB"

 // ========= Comm ROUTINES Section =========

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
        for(int i=0; i < 8+1; i++) {
            while( TISerial.available() <= 0 ) { delay(2); }
            int ch = TISerial.read();
            // if ( ch == -1 ) { return false; } // not enough bytes
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
                        PCSerial.print( TISerial.read(), HEX ); PCSerial.print( ' ' );
                    }
                    PCSerial.println();

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

#endif