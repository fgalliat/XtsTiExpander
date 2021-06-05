#ifndef __SCREEN_H__
#define __SCREEN_H__ 1

/**
 * XtsTiExpander
 * 
 * Xtase - fgalliat @Jun 2021
 * 
 * SSD1306 - 128x32 oled I2C Screen
 * 
 * @@ ESP Wiring - SSD1306
 * 
 * VCC -> VCC
 * GND -> GND
 * 21  -> SDA
 * 22  -> SCL
 **/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin #
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool SCR_READY = false;


void displayGauge(int percent);

bool setupScreen() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    return false;
  }
  SCR_READY = true;            // use it later to redirect to Serial if not OK

  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner

  display.println("Hello TiExpander");

  // displayGauge(64);

  display.display();

  return true;
}

// display progress gauge 
void displayGauge(int percent) {
  const int gaugeWidth = 100 + 4;
  const int gaugeHeight = 5;

  int x = ((SCREEN_WIDTH - gaugeWidth)/2);
  int y = SCREEN_HEIGHT - gaugeHeight - 4;

  display.fillRect(x, y, gaugeWidth, gaugeHeight, SSD1306_BLACK);
  display.drawRoundRect(x, y, gaugeWidth, gaugeHeight, 3, SSD1306_WHITE);

  x += 2;
  y += 2;
  display.drawLine(x, y, x+percent, y, SSD1306_WHITE);

  display.display();
}

void displayCls() {
  display.clearDisplay();
  display.setCursor(0, 0);
}

void displayBlitt() {
  display.display();
}

void displayTitle(char* str) {
  display.fillRect(0, 0, SCREEN_WIDTH, 10, SSD1306_BLACK);
  display.setCursor(0, 0);
  display.println(str);

  display.display();
}

void displayIncomingVar(char* varName, uint8_t varType, uint32_t varSize) {
  displayCls();
  char msg[64+1]; memset( msg, 0x00, 64+1 );
  sprintf(msg, "> Var  : %s #%02X\n> Size : %ld", varName, varType, varSize);
  displayTitle(msg);
}

void displayByteHex(uint8_t bte) {
  char msg[3+1]; memset( msg, 0x00, 3+1 );
  sprintf(msg, "%02X ", bte);
  display.print( msg );
}

void displayPrintln(char* str) {
  display.println( str );
}

#endif