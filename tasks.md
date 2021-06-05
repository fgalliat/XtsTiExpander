# XtsTiExpander

Xtase - fgalliat @June 04  2021

- **TiComm protocol**
  - [x] Ti send variable(s)
  - [x] Ti Enter / Exit DUMMY mode
  - [x] Ti Send CBL Value
  
- **Expander Hardware**
  - [x] ESP core (base wirings)
  - [ ] ESP SPIFFS
  - [x] ESP Screen
  - [ ] ESP Button
  - [ ] ESP Buzzer
  - [ ] ESP / TiComm Control lines

- **Expander Functions**

  - [ ] Save TiVar to internal Storage (ex. popbin.1C / xtsterm2.21 -> .1C, .21 is the var type)

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
  
      - [ ]  => when receive this specific variable => triggers an ESP -> TiComm -> Ti silent file sending
      - [ ] that seems to work : Exp. recv "> Var : varname #0C / > Size : 13"
      - [ ] make capability to reference packs of files ex. "#system" -> varname - esp knows that list-of-files name
  
  - [ ] Use DUMMY MODE for :
  
    - [ ] Pseudo ESP shell
    - [ ] Sound tone String
    - [ ] WiFi REST
    - [ ] WiFi telnet
    - [ ] (.md reader -> ESP Local / Web Remote content)

