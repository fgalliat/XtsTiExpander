#ifndef __WIFI_H__
#define __WIFI_H__ 1

/**
 * XtsTiExpander
 * 
 * Xtase - fgalliat @Jun 2021
 * 
 * WiFi routines
 */

// this file is voluontary not a part of Git repository
#include "wifi_psk.h"

#ifndef SSID1
  #error "No SSID1 & PSK1 were defined (see wifi_psk.h)"
#endif

#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

//how many clients should be able to telnet to this ESP32
#define MAX_SRV_CLIENTS 1

WiFiServer server(23);
WiFiClient serverClients[MAX_SRV_CLIENTS];

bool WIFI_READY = false;

void startWiFi() {
  if ( WIFI_READY ) { return; }
  displayCls();
  displayPrintln("Connecting to WiFi");
  displayBlitt();

  
  wifiMulti.addAP(SSID1, PSK1);
  #ifdef SSID2
    wifiMulti.addAP(SSID2, PSK2);
  #endif
  
  for (int loops = 10; loops > 0; loops--) {
    if (wifiMulti.run() == WL_CONNECTED) {
      const char* ipAddr = WiFi.localIP().toString().c_str();
      displayPrintln( (char*)ipAddr );
      displayBlitt();
      WIFI_READY = true;
      break;
    }
    else {
      // Serial.println(loops);
      delay(1000);
    }
  }
  if (wifiMulti.run() != WL_CONNECTED) {
    displayPrintln( "WiFi connect failed" );
    displayBlitt();
    WIFI_READY = false;
    return;
  }

  server.begin();
  server.setNoDelay(true);
}

void stopWiFi() {
  if ( ! WIFI_READY ) { return; }

  displayCls();
  displayPrintln("Closing WiFi");
  displayBlitt();

  for(int i = 0; i < MAX_SRV_CLIENTS; i++) {
    if (serverClients[i]) serverClients[i].stop();
  }
  server.close();
  delay(500);
  // wifiMulti.disconnect(); // need to delete ref ... WiFiMulti::~WiFiMulti()

  displayPrintln("Closed WiFi");
  displayBlitt();
}

void lsToTelnet(Stream* client);
void catToTelnet(Stream* client, char* varName, bool hexMode=false);

void loopTelnet() {
    if (wifiMulti.run() != WL_CONNECTED) {
        return;
    }
    int i;

    //check if there are any new clients
    if (server.hasClient()){
      for(i = 0; i < MAX_SRV_CLIENTS; i++){
        //find free/disconnected spot
        if (!serverClients[i] || !serverClients[i].connected()){
          if(serverClients[i]) serverClients[i].stop();
          serverClients[i] = server.available();
          if (!serverClients[i]) Serial.println("available broken");
          Serial.print("New client: ");
          Serial.print(i); Serial.print(' ');
          Serial.println(serverClients[i].remoteIP());
          break;
        }
      }
      if (i >= MAX_SRV_CLIENTS) {
        //no free/disconnected spot so reject
        server.available().stop();
      }
    }
    //check clients for data
    for(i = 0; i < MAX_SRV_CLIENTS; i++){
      if (serverClients[i] && serverClients[i].connected()){
        // if(serverClients[i].available()){
        //   //get data from the telnet client and push it to the UART
        //   while(serverClients[i].available()) Serial1.write(serverClients[i].read());
        // }

        // serverClients[i].write(sbuf, len);

        // // --------- sequential session ---------
        // // read potential connection termcap ...
        // while(serverClients[i].available()) serverClients[i].read();

        // serverClients[i].println( "Hello telnet stranger" );
        // while(serverClients[i].available() == 0) { delay(10); }
        // serverClients[i].println( "Bye" );
        // serverClients[i].stop();
        // // --------- sequential session ---------

        if(serverClients[i].available()){
            char cmd[64+1]; memset(cmd, 0x00, 64+1);
            int len = serverClients[i].readBytesUntil( '\n', cmd, 64 );
            if ( len > 0 ) {
                // chop the last '\n'
                cmd[ len - 1 ] = 0x00;

                if ( startsWith( cmd, "/quit" ) ) {
                    serverClients[i].println( "Bye" );
                    serverClients[i].stop();
                } else if ( startsWith( cmd, "ls" ) ) {
                    lsToTelnet( &serverClients[i] );
                } else if ( startsWith( cmd, "cat " ) ) {
                    char* filename = &cmd[4];
                    catToTelnet( &serverClients[i], filename, false );
                } else if ( startsWith( cmd, "hex " ) ) {
                    char* filename = &cmd[4];
                    catToTelnet( &serverClients[i], filename, true );
                } else if ( startsWith( cmd, "find " ) ) {
                    char* filename = &cmd[5];
                    char* found = findTiFile( filename );
                    serverClients[i].print("Found : ");
                    serverClients[i].println(found == NULL ? "<NULL>" : found);
                } else {
                    serverClients[i].println( "???" );
                }
            }
        }

      }
      else {
        if (serverClients[i]) {
          serverClients[i].stop();
        }
      }
    }

}

void lsToTelnet(Stream* client) {
   lsToStream( client );
}

void catToTelnet(Stream* client, char* varName, bool hexMode/*=false*/) {
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


// void loop() {
//   uint8_t i;
//   if (wifiMulti.run() == WL_CONNECTED) {
//     //check if there are any new clients
//     if (server.hasClient()){
//       for(i = 0; i < MAX_SRV_CLIENTS; i++){
//         //find free/disconnected spot
//         if (!serverClients[i] || !serverClients[i].connected()){
//           if(serverClients[i]) serverClients[i].stop();
//           serverClients[i] = server.available();
//           if (!serverClients[i]) Serial.println("available broken");
//           Serial.print("New client: ");
//           Serial.print(i); Serial.print(' ');
//           Serial.println(serverClients[i].remoteIP());
//           break;
//         }
//       }
//       if (i >= MAX_SRV_CLIENTS) {
//         //no free/disconnected spot so reject
//         server.available().stop();
//       }
//     }
//     //check clients for data
//     for(i = 0; i < MAX_SRV_CLIENTS; i++){
//       if (serverClients[i] && serverClients[i].connected()){
//         if(serverClients[i].available()){
//           //get data from the telnet client and push it to the UART
//           while(serverClients[i].available()) Serial1.write(serverClients[i].read());
//         }
//       }
//       else {
//         if (serverClients[i]) {
//           serverClients[i].stop();
//         }
//       }
//     }
//     //check UART for data
//     if(Serial1.available()){
//       size_t len = Serial1.available();
//       uint8_t sbuf[len];
//       Serial1.readBytes(sbuf, len);
//       //push UART data to all connected telnet clients
//       for(i = 0; i < MAX_SRV_CLIENTS; i++){
//         if (serverClients[i] && serverClients[i].connected()){
//           serverClients[i].write(sbuf, len);
//           delay(1);
//         }
//       }
//     }
//   }
//   else {
//     Serial.println("WiFi not connected!");
//     for(i = 0; i < MAX_SRV_CLIENTS; i++) {
//       if (serverClients[i]) serverClients[i].stop();
//     }
//     delay(1000);
//   }
// }


#endif