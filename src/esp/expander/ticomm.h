/**
 * XtsTiExpander
 * 
 * Xtase - fgalliat @Jun 2021
 * 
 * XtsTiLink Serial protocol
 */

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