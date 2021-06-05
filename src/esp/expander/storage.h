#ifndef __STORAGE_H__
#define __STORAGE_H__ 1

/**
 * XtsTiExpander
 * 
 * Xtase - fgalliat @Jun 2021
 * 
 * SPIFFS builtin storage routines
 */

 #include "FS.h"
 #include "SPIFFS.h"


 // (!!) no real directory inSPIFFS
 // ex.
 // Listing directory: /
 // FILE: /logs/log.txt   SIZE: 14
 #define TIVAR_DIR "/tivars/"
 #define LOG_DIR "/logs/"

 bool STORAGE_READY = false;

 // forward
 void writeFile(fs::FS &fs, const char * path, const char * message);

 bool setupStorage() {
    if ( SCR_READY ) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Wait FS");
        display.display();
    }

    if(! SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED) ){
      Serial.println("SPIFFS Mount Failed");

      if ( !FORMAT_SPIFFS_IF_FAILED ) {
          Serial.println("May need to format it !");
      }

      displayTitle( "SPIFFS Mount Failed" );
      return false;
    }

    // prepare Fs
    // SPIFFS.mkdir(TIVAR_DIR); // no real dirs in SPIFFS -> does nothing
    writeFile( SPIFFS, LOG_DIR"log.txt", "New fresh Logs" );

    STORAGE_READY = true; // use later to WARN User if Storage fails

    if (SCR_READY) {
        display.println("FS is OK");
        display.display();
    }

    return true;
 }

 void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
   Serial.printf("Listing directory: %s\r\n", dirname);

   File root = fs.open(dirname);
   if(!root){
      Serial.println("− failed to open directory");
      return;
   }
   if(!root.isDirectory()){
      Serial.println(" − not a directory");
      return;
   }

   File file = root.openNextFile();
   while(file){
      if(file.isDirectory()){
         Serial.print("  DIR : ");
         Serial.println(file.name());
         if(levels){
            listDir(fs, file.name(), levels -1);
         }
      } else {
         Serial.print("  FILE: ");
         Serial.print(file.name());
         Serial.print("\tSIZE: ");
         Serial.println(file.size());
      }
      file = root.openNextFile();
   }
 }

 void listRoot() {
     listDir(SPIFFS, "/", 0);
 }

 void writeFile(fs::FS &fs, const char * path, const char * message) {
   Serial.printf("Writing file: %s\r\n", path);

   File file = fs.open(path, FILE_WRITE);
   if(!file){
      Serial.println("− failed to open file for writing");
      return;
   }
   if(file.print(message)){
      Serial.println("− file written");
   }else {
      Serial.println("− frite failed");
   }
   
   file.close();
 }

 void appendFile(fs::FS &fs, const char * path, const char * message){
   Serial.printf("Appending to file: %s\r\n", path);

   File file = fs.open(path, FILE_APPEND);
   if(!file){
      Serial.println("− failed to open file for appending");
      return;
   }
   if(file.print(message)){
      Serial.println("− message appended");
   } else {
      Serial.println("− append failed");
   }

   file.close();
 }

 void readFile(fs::FS &fs, const char * path){
   Serial.printf("Reading file: %s\r\n", path);

   File file = fs.open(path);
   if(!file || file.isDirectory()){
       Serial.println("− failed to open file for reading");
       return;
   }

   Serial.println("− read from file:");
   while(file.available()){
      Serial.write(file.read());
   }
   file.close();
 }

 void lsToStream(Stream* client) {
   if ( !STORAGE_READY ) {
      client->println("No FileSystem mounted");
      return;
   }

   File root = SPIFFS.open("/");
   if(!root){
      client->println("− failed to open directory");
      return;
   }

   File file = root.openNextFile();
   while(file){
      if(file.isDirectory()){
         // .. never appens on SPIFFS
      } else {
         client->print("  FILE: ");
         client->print(file.name());
         client->print("\tSIZE: ");
         client->println(file.size());
      }
      file = root.openNextFile();
   }
   client->println( "-EOF-" );
 }
 
 // --- Ti Storage ---

 File createTiFile(const char * varName, uint8_t varType, int &error) {
   char path[64+1]; memset( path, 0x00, 64+1 );
   sprintf( path, "%s%s.%02X", TIVAR_DIR, varName, varType );
   PCSerial.printf("Writing file: %s\r\n", path);

   File file = SPIFFS.open(path, FILE_WRITE);
   if(!file){
      PCSerial.println("− failed to open file for writing");
      error = 1;
      return file;
   }
   error = 0;
   return file;
 }

 char* findTiFile(const char * varName) {
   if ( !STORAGE_READY ) {
      return NULL;
   }

   File root = SPIFFS.open("/");
   if(!root){
      return NULL;
   }

   char toFind[64+1]; memset( toFind, 0x00, 64+1 );
   sprintf(toFind, "%s%s.", TIVAR_DIR, varName);

   File file = root.openNextFile();
   while(file){
      if(file.isDirectory()){
         // .. never appens on SPIFFS
      } else {
         char* entryName = (char*)file.name();

         if ( startsWith( entryName, toFind ) ) {
            return entryName;
         }

      }
      file = root.openNextFile();
   }
   return NULL;
 }


#endif