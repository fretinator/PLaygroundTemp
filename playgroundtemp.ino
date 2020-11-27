// Demo program for testing library and board - flip the switch to turn on/off buzzer

#include <Adafruit_CircuitPlayground.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// we light one pixel at a time, this is our counter
bool screen_opened = false;
float curTemp = 0.0;
const int max_temp = 86;
const int min_temp = 66;
const int num_pixels = 10;
const uint8_t start_color = 170;
const uint8_t end_color = 0;
const uint8_t color_increments = uint8_t((start_color - end_color) / num_pixels);
const uint8_t led_level = 10; // Brightness 0 to 255
const float temp_adjust = -6.0; // Amount to adjust temp due to warmth of lights/board
unsigned long lastMillis = 0;
unsigned long refreshMillis = 10 * 1000; 
unsigned long curMillis = 0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint32_t getColorForPixel(uint8_t pixel) {
  uint32_t color = CircuitPlayground.colorWheel(uint8_t(start_color - (color_increments * (pixel + 1))));
  
  Serial.print(F("pixel color for Pixel : "));
  Serial.print(pixel);
  Serial.print(F(", color = "));
  Serial.println(color);

  return color;
  }

void tempToPixels(float pTemp) {
  // Calculate ratio of desired temp range
  uint8_t max_pixel = uint8_t(9.0 * (pTemp - min_temp) / (max_temp - min_temp));

  // We want a number from 0 to 9 - 0: Only lowest pixel, 9: all pixels
  max_pixel = max(min(9,max_pixel),0);

  Serial.print(F("Pixels to set : "));;
  Serial.println(max_pixel);

  CircuitPlayground.clearPixels();
  CircuitPlayground.setBrightness(led_level);
  
  Serial.println(F("Pixels are cleared.")); 

  for(uint8_t p=0;p<10;p++) {
    if(p <= max_pixel) {
      // We want clock-wise, pixels are numbered counter=clockwise
      uint8_t pixelToSet = 9 - p;
      uint32_t setColor = getColorForPixel(p);
      Serial.print(F("Pixel to set : "));
      Serial.print(p);
      Serial.print(F(", color = "));
      Serial.println(setColor);
      
      CircuitPlayground.setPixelColor(pixelToSet, setColor);
    }
  }
}

void displayTemp(float pTemp) {
  Serial.print(F("Temp to display: "));
  Serial.println(pTemp);
  
  display.clearDisplay();
  display.setTextSize(5);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(4,8);             // Start at top-left corner
  display.print(int(pTemp));
  display.println(F(" F"));
  display.display();
  delay(1);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Circuit Playground test!");

  CircuitPlayground.begin();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  screen_opened = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  if(screen_opened) {
      display.display();
      delay(1);
  }

  lastMillis = 0;
}

void loop() {
  curMillis = millis();

  // Add check fir clock rolling over back to 0: curMillis < lastMillis
  if( ((curMillis - lastMillis) > refreshMillis) || (curMillis < lastMillis) ) { 
    /************* Get THERMISTOR temp */
    
    curTemp = temp_adjust + 1.8 * CircuitPlayground.temperature() + 32;
    Serial.print("Current Temp: "); 
    Serial.println(curTemp);
       
    tempToPixels(curTemp);

    if(screen_opened) {

      displayTemp(curTemp);
    }

    lastMillis = curMillis;
  }
}
