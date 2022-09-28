
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#include <BMP280_DEV.h>                           // Include the BMP280_DEV.h library

float temperature, pressure, altitude;            // Create the temperature, pressure and altitude variables
BMP280_DEV bmp280;                                // Instantiate (create) a BMP280_DEV object and set-up for I2C operation (address 0x77)

#define YDEWPOINT 24 //YUCATANS DEWPOINT
#define euler 2.718281828

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
 
  bmp280.begin(); 

}

void loop() {
   //delay(1000);
  
   bmp280.startForcedConversion();                 // Start BMP280 forced conversion (if we're in SLEEP_MODE)
   
  if (bmp280.getMeasurements(temperature, pressure, altitude)){// Check if the measurement is complete
  // Display static text
  
  float a=(-4283.58*temperature)+(4283.58*YDEWPOINT);
  float b=(YDEWPOINT+243.04)*(temperature+243.04);
  float c=a/b;
  
//    display.clearDisplay();
//    Serial.print(temperature);                    // Display the results    
//    Serial.print(F("*C   "));
//    Serial.print(pressure);    
//    Serial.print(F("hPa   "));
//    Serial.print(altitude);
//    Serial.println(F("m"));  
    
    //Serial.println(a);  
    //Serial.println(b); 
    //Serial.println(c); 
//    Serial.println(100*pow(euler,c)); 
//    Serial.println(F("%RH")); 
  
  display.setCursor(0, 0);
  display.print(temperature);
  display.print(F(" C"));

  display.setCursor(0, 10);
  display.print(pressure);
  display.print(F(" hPa"));

  display.setCursor(0, 20);
  display.print(altitude);
  display.print(F(" m"));

  display.setCursor(0, 30);
  display.print(100*pow(euler,c));
  display.print(F(" %RH"));
  display.display(); 
  }
    
}
  
