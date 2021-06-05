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
 * 
 **/

#define PCSerial Serial
// 16 RX2 - 17 TX2
#define TISerial Serial2

// (!!) BEWARE w/ that FLAG
// #define FORMAT_SPIFFS_IF_FAILED true
#define FORMAT_SPIFFS_IF_FAILED false

// ---------- Generic functions -----------

bool startsWith(char* strFull, char* strToFind) {
    if ( strToFind == NULL || strFull == NULL ) { return false; }
    if ( strlen( strFull ) < strlen( strToFind ) ) { return false; }
    return strncmp( strFull, strToFind, strlen( strToFind ) ) == 0;
}

// 0C -> 12
int hexStrToInt(char* str) {
    int hexNumber;
    sscanf(str, "%02X", &hexNumber);
    return hexNumber;
}