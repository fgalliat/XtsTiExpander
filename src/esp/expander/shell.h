#ifndef __SHELL_H__
#define __SHELL_H__ 1

 /**
 * XtsTiExpander
 * 
 * Xtase - fgalliat @Jun 2021
 * 
 * ESP Mini Shell routines
 */

// from ticomm.h
bool sendTiVar(char* varName, Stream* client);

// from wifi.h
void startWiFi();
void stopWiFi();

void sendToTi(Stream* client, char* varName) {
  // sends from SPIFFS to calc
  client->println("Sending variable");
  bool ok = sendTiVar(varName, client);
  if ( ok ) {
    client->println("Sucessfully sent variable");
  } else {
    client->println("Failed to send variable");
  }
}

void catToStream(Stream* client, char* varName, bool hexMode/*=false*/) {
   if ( !STORAGE_READY ) {
      client->println("No FileSystem mounted");
      return;
   }

   char* path = findTiFile(varName);
   if ( path == NULL ) {
       client->print("No such Var : >"); client->print(varName); client->println("<");
      return;
   }

   if ( ! SPIFFS.exists( path ) ) {
       client->print("No such file : >"); client->print(path); client->println("<");
      return;
   }

   File file = SPIFFS.open(path);
   if(!file || file.isDirectory()){
       client->println("Failed to open file for reading");
       return;
   }
   char hh[4];
   while(file.available()){
      if ( hexMode ) {
        sprintf( hh, "%02X ", file.read() );
        client->print( hh );
      } else {
        client->write(file.read());
      }
   }

   file.close();
   client->println("-EOF-");
}



  /** return true if sessionStream/Mode should be closed */
  bool sessionLoop(Stream* client, int shellMode) {
    if ( client->available() == 0 ) {
        return false;
    }

    #define CMD_LEN 64

    char cmd[CMD_LEN+1]; memset(cmd, 0x00, CMD_LEN+1);
    int len = 0;

    // -------------------------------------------
    // read a line from Stream input
    if ( shellMode == SHELL_MODE_TELNET ) {
        // on Nux telnet -> line blocking wait + echo ...
        len = client->readBytesUntil( '\n', cmd, CMD_LEN );
    } else {
        for(int i=0; i < CMD_LEN; i++) {
            while ( client->available() == 0 ) { delay(10); }
            int ch = client->read();
            
            client->write( ch ); // echo

            cmd[len++] = ch;

            // done after because of next 'chop'
            if ( shellMode == SHELL_MODE_SERIAL ) {
                if ( ch == '\r' ) { break; }
            } else {
                if ( ch == '\n' ) { break; }
            }
        }
    }
    // -------------------------------------------
   
    if ( len > 0 ) {
        // chop the last '\n'
        cmd[ len - 1 ] = 0x00;

        if ( startsWith( cmd, "/quit" ) ) {
            client->println( "Bye" );
            // serverClients[i].stop();
            return true; // telnet client should be closed / DummyMode should End / Serial -> don't care
        } else if ( startsWith( cmd, "ls" ) ) {
            lsToStream( client );
        } else if ( startsWith( cmd, "cat " ) ) {
            char* varName = &cmd[4];
            catToStream( client, varName, false );
        } else if ( startsWith( cmd, "hex " ) ) {
            char* varName = &cmd[4];
            // client->print( ">>" ); client->print( varName ); client->println( "<<" );
            catToStream( client, varName, true );
        } else if ( startsWith( cmd, "send " ) ) {
            char* varName = &cmd[5];
            sendToTi( client, varName );
        } else if ( startsWith( cmd, "find " ) ) {
            char* varName = &cmd[5];
            char* found = findTiFile( varName );
            client->print("Found : ");
            client->println(found == NULL ? "<NULL>" : found);
        } 
        
        else if ( startsWith( cmd, "iw " ) ) {
            char* opName = &cmd[3];
            if ( startsWith( opName, "start" ) ) {
                startWiFi();
            } else if ( startsWith( opName, "stop" ) ) {
                stopWiFi();
            } else {
                client->println( "wifi : ???" );
            }
        } 
        else {
            client->println( "???" );
        }
    }

    return false;
  }




#endif