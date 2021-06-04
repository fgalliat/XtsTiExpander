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

void displayGauge(int percent);

bool setupScreen() {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    return false;
  }

  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner

  display.println("Hello TiExpander");

  // display progress gauge 
  displayGauge(64);
  // display progress gauge 

  display.display();

  return true;
}

// display progress gauge 
void displayGauge(int percent) {
  int gaugeWidth = 100 + 4;
  int gaugeHeight = 5;

  display.fillRect((SCREEN_WIDTH - gaugeWidth)/2, SCREEN_HEIGHT - gaugeHeight - 4, gaugeWidth, gaugeHeight, 
      SSD1306_BLACK);

  display.drawRoundRect((SCREEN_WIDTH - gaugeWidth)/2, SCREEN_HEIGHT - gaugeHeight - 4, gaugeWidth, gaugeHeight,
      3, SSD1306_WHITE);

  int x = ((SCREEN_WIDTH - gaugeWidth)/2)+2;
  int y = SCREEN_HEIGHT - gaugeHeight - 4 + 2;
  display.drawLine(x, y, x+percent, y, SSD1306_WHITE);

  display.display();
}

#endif