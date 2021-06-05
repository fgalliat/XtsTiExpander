# XtsTiExpander

Xtase - fgalliat @June 04  2021

- **TiComm protocol**
  - [x] Ti send variable(s)
  - [x] Ti Enter / Exit DUMMY mode
  - [x] Ti Send CBL Value
  
- **Expander Hardware**
  - [x] ESP core (base wirings)
  - [x] ESP SPIFFS
  - [x] ESP Screen
  - [x] ESP WiFi
  - [ ] ESP Button
  - [ ] ESP Buzzer
  - [ ] ESP / TiComm Control lines
  
- **Expander macro Functions**

  - [x] Ti -> Storage (Ti request)
  - [ ] Storage -> Ti (Ti request)
  - [ ] PC -> Storage ( -> Ti ) (PC request) -- **issue** : Each you connect to an ESP32 by USBSerial - it reboots

- **Expander Functions**

  - [x] Save TiVar to internal Storage (ex. popbin.1C / xtsterm2.21 -> .1C, .21 is the var type)

  - [ ] Get free Storage space

  - [ ] List Stored TiVar names (DUMMY MODE ??)

  - [ ] Send a/some Stored TiVar to TiComm (? how to do that w/o any PC/Serial/Telnet ctrl ?)

    - [ ] cannot be done in DUMMY MODE [!!]

    - [ ] ```
      FILE *f = fopen ("example", "w"); // writes a TEXT variable
      fputs ("First line\n", f);
      fputs ("Second line\n", f);
      fputs ("Third line\n", f);
      fclose (f);
      
      
      // with stdio.h --> writes a StringVar -- auto-add length bytes + chk ?
      FILE *f = fopen ("example", "wb");
      fputc (0, f);
      fputs ("This is a string", f);
      fputc (0, f);
      fputc (STR_TAG, f);
      fclose (f);
      ```
      
      
      
    - [ ] ~~**CBL String comand** (need multi bytes)~~
  
      - [ ] disconnect ESP from TiComm (to check protocol)
      - [ ] check # between Send { 255 } / Send { 255, 4 } / **Send "coucou" -> seems to not be possible** (even if it's the protocol used...)
  
    - [ ] **SendCalc** :
  
      ```
      : "file2get" -> varname
      : SendCalc varname
      ```
  
      - [ ] => when receive this specific variable => triggers an ESP -> TiComm -> Ti silent file sending
      
      - [x] that seems to work : Exp. recv "> Var : varname #0C / > Size : 13"
      
      - [ ] make capability to reference packs of files ex. "#system" -> varname - esp knows that list-of-files name
      
      - [x] **maybe :** 
      
        - [x] call 'varname' -> 'tiaction'
      
          ```
          "get:popbin" -> tiaction : SendCalc tiaction -- SYNC Op to get a file from storage
          "wifi:start" -> tiaction : SendCalc tiaction -- ASYNC Start WiFi + telnet server + telnet handle
          "wifi:stop" -> tiaction : SendCalc tiaction  -- SYNC Stop WiFi + telnet server
          "wifi:ssid:xxxx" \___ save in a conf.file on ESP
          "wifi:psk:xxxx"
          "wifi:sta"        ___
          "wifi:ap"        /
           L---> @least those could be requested by a TiBasic Menu PRGM
          "url:192.168.1.x/api/sensors/1" ....         -- ASYNC call REST url + writes back 'tiresp' variable + send KeyCode ?
          ```
      
      - [x] write a VarRecv handler for this specific variable name -> decode requested action
  
  - [ ] Use DUMMY MODE for :
  
    - [ ] Pseudo ESP shell
    - [ ] Sound tone String
    - [ ] WiFi REST client
    - [ ] WiFi telnet client
    - [ ] (.md reader -> ESP Local / Web Remote content)
  
- **Telnet / Dummy Functions**

  - [x] ls => in DUMMY too
  - [ ] ls only TiVars => in DUMMY too
  - [x] find TiVar
  - [ ] cat TiVar
  - [x] hex TiVar
  - [ ] text TiVar - display only what can be displayed (0x0D -> 0x0A ? on TEXT Vars) => in DUMMY too
  - [ ] upload TiVar to ESP Storage (-then- to Ti ?)
  - [ ] ESP Storage to Ti (see the "/SP" tiComm protocol ...) => then [ ESP Storage to Ti ] by tiAction handler
  - [ ] TEXT Vars / File create / ?edit? w/ telnet stdin
