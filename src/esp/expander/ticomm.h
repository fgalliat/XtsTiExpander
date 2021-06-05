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

 // ========= TiVar Types Section =========
 #define VAR_STRING 0x0C

 // ========= TiAction Request Section ====
 // max action length
 #define TI_ACTION_LENGTH 64

 // ========= Comm ROUTINES Section =======

 void handleTiActionRequest(char* str);

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

                bool tiActionRequest = false;
                char tiActionValue[TI_ACTION_LENGTH+1];
                int  tiActionValueCursor = 0;

                if ( strncmp(varName, "tiaction", 8) == 0 && varType == VAR_STRING ) {
                    memset( tiActionValue, 0x00, TI_ACTION_LENGTH+1 );
                    tiActionRequest = true;
                }

                // ------ Screen output --------
                displayIncomingVar( varName, varType, varSize );
                // ------ Screen output --------

                // ------ Storage output --------
                bool store = STORAGE_READY && ( ! tiActionRequest );
                File f;
                if ( store ) {
                    int error = 0;
                    f = createTiFile(varName, varType, error);
                    if ( error != 0 ) {
                        store = false;
                        displayTitle("WRITE error !");
                    }
                }
                // ------ Storage output --------

                while( TISerial.available() <= 0 ) { delay(2); }

                if ( TISerial.available() >= 3 ) {
                    uint8_t dataDef[3];
                    TISerial.readBytes( dataDef, 3 );
                    if ( strncmp( (char*)dataDef, IN_BIN_SENDVAR_DATA, 3 ) != 0 ) {
                        if ( store ) {
                            f.close();
                        }
                        return false;
                    }

                    int lastPercent = 0;
                    long t0 = 0;

                    for(uint32_t i=0; i < varSize; i++) {
                        // FIXME : do better than byte-per-byte
                        while( TISerial.available() <= 0 ) { delay(2); }
                        int bte = TISerial.read();
                        // PCSerial.print( bte, HEX ); PCSerial.print( ' ' );
                        if ( store ) {
                            f.write( bte );
                        }

                        if ( tiActionRequest ) {
                            if ( i > 0 ) { // skip 1st 0x00
                                if ( bte == 0x00 ) { // stop to next 0x00 - 0x2D - CHK - CHK 
                                    tiActionValueCursor = -99;
                                }
                                if ( tiActionValueCursor >= 0 ) { // sure we can write String value
                                    tiActionValue[ tiActionValueCursor++ ] = (char)bte;
                                }
                            }
                            // displayByteHex( bte );
                        }
                        else {
                            // ----- Progress Gauge -----
                            long t1 = millis();
                            if ( t1 - t0 > 100 ) {
                                int percent = (int)( (uint32_t)100 * i / varSize  );
                                if ( percent - lastPercent > 5 ) { // only if more than 5% delta
                                displayGauge( percent );
                                lastPercent = percent;
                                }
                                t0 = t1;
                            }
                        }
                    }
                    if ( store ) {
                        f.close();
                    }
                    if ( tiActionRequest ) {
                        displayPrintln( "= ACTION REQ =" );
                        displayPrintln( tiActionValue );
                        displayBlitt();
                    } else { // regular Var ...
                        displayGauge( 100 );
                    }
                    // PCSerial.println();

                    while( TISerial.available() <= 0 ) { delay(2); }
                    if ( TISerial.available() >= 3 ) {
                        uint8_t eofDef[3];
                        TISerial.readBytes( eofDef, 3 );
                        if ( strncmp( (char*)eofDef, IN_BIN_SENDVAR_EOF, 3 ) != 0 ) {
                            return false;
                        }
                        PCSerial.println("(ii) TiComm send a variable : EOF ");


                        if ( tiActionRequest ) {
                            handleTiActionRequest( tiActionValue );
                        }

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

// Ti silent mode
bool sendTiVar(char* varName) {
    // read potential Garbage datas
    while ( TISerial.available() ) { TISerial.read(); }

    char* filename = findTiFile( varName );
    if ( filename == NULL ) {
        displayCls();
        displayTitle("(!!) FAIL FINDING VAR");
        displayPrintln( varName );
        displayBlitt();
        return false;
    }

    int nlength = strlen( filename );
    char* strVarType = &filename[ nlength - 2 ];
    uint8_t varType = (uint8_t)hexStrToInt( strVarType );

    File file = SPIFFS.open(filename);
    if(!file || file.isDirectory()){
        displayCls();
        displayTitle("(!!) FAIL OPENING VAR");
        displayPrintln( varName );
        displayBlitt();
        return false;
    }

    long varSize = file.size();
    displayOutgoingVar( varName, varType, varSize );

    int lastPercent = 0;
    long t0 = 0;

    TISerial.print("\\SP");
    TISerial.write( (uint8_t)(varSize >> 8) ); TISerial.write( (uint8_t)(varSize % 256) );
    TISerial.print( varName ); TISerial.write( 0x00 );
    TISerial.write( 0x00 ); // non ASM content
    TISerial.write( 0x00 ); // no autorun
    char resp[64+1]; memset( resp, 0x00, 64+1 );
    TISerial.readBytesUntil( '\n', resp, 64 );
    if ( ! startsWith( resp, "I:" ) ) {
        // may had an error
        displayCls();
        displayTitle("(!!) FAIL SENDING VAR");
        displayPrintln( resp );
        displayBlitt();
        file.close();
        return false;
    }
    while( TISerial.available() == 0 ) { delay(2); }
    int code = TISerial.read();
    // code should be 0x01
    TISerial.write( 0x00 ); // - ready to send datas

    const int packetLen = 64;
    char packet[ packetLen ];
    long i = 0;

    while( file.available() ) {
      while( TISerial.available() == 0 ) { delay(2); }
      int handshake = TISerial.read();
      // handshake should be 0x02
      int read = file.readBytes( packet, packetLen );
      TISerial.write( (uint8_t*)packet, read);

      i += read;

        // ----- Progress Gauge -----
        long t1 = millis();
        if ( t1 - t0 > 100 ) {
            int percent = (int)( (long)100 * i / varSize  );
            if ( percent - lastPercent > 5 ) { // only if more than 5% delta
                displayGauge( percent );
                lastPercent = percent;
            }
            t0 = t1;
        }
    }

    memset( resp, 0x00, 64+1 );
    TISerial.readBytesUntil( '\n', resp, 64 );
    if ( ! startsWith( resp, "I:" ) ) {
        // may had an error
        displayCls();
        displayTitle("(!!) FAIL WH.SENDING VAR");
        displayPrintln( resp );
        displayBlitt();
        file.close();
        return false;
    }

    // if required autorun --> have to read another line of text

    displayGauge( 100 );
    file.close();

    // durring that time - the Arduino ProMini (TiComm) reboots ...

    return true;   
}

// -------------------------------------------------
void handleTiActionRequest(char* str) {
    if ( startsWith( str, "get:" ) ) {
        char* filename = &str[4];
        displayCls();
        displayPrintln("> Req a file : ");
        displayPrintln( filename );
        displayBlitt();
    } else if ( startsWith( str, "wifi:" ) ) {
        char* op = &str[5];
        displayCls();
        displayPrintln("> Req a WiFi Op : ");
        displayPrintln( op );
        displayBlitt();

        if ( startsWith( op, "start" ) ) {
            startWiFi();
        } else if ( startsWith( op, "stop" ) ) {
            stopWiFi();
        }

    } else {
        displayCls();
        displayPrintln("> Req a ???? : ");
        displayPrintln( str );
        displayBlitt();
    }
}


#endif