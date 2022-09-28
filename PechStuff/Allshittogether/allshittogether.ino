
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

#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid

//Constants
#define SS_PIN 5
#define RST_PIN 27

//Parameters
const int ipaddress[4] = {103, 97, 67, 25};

//Variables
byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);

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

  Serial.println(F("Initialize System"));
  //init rfid D8,D5,D6,D7
  SPI.begin();
  rfid.PCD_Init();

  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();

}

void readRFID(void ) { /* function readRFID */
  ////Read RFID card

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new 1 cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if (  !rfid.PICC_ReadCardSerial())
    return;

  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  Serial.print(F("RFID In dec: "));
  printDec(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();
  Serial.print(F("RFID In HEX: "));
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

}


/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  int j=0;
  display.setCursor(0, 40);
  display.print(F("user "));
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);

  display.print(buffer[i], DEC);
  j=j+10;

  }

  j=0;
  display.display();
  display.clearDisplay();
}

void print_info_oled(float temperature, float pressure, float altitude, float humidity){
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
  display.print(100*pow(euler,humidity));
  display.print(F(" %RH"));
  display.display();

  }

void loop() {

  float a,b,c;
  bmp280.startForcedConversion();                 // Start BMP280 forced conversion (if we're in SLEEP_MODE)

  if (bmp280.getMeasurements(temperature, pressure, altitude)){// Check if the measurement is complete
  // Display static text

  a=(-4283.58*temperature)+(4283.58*YDEWPOINT);
  b=(YDEWPOINT+243.04)*(temperature+243.04);
  c=a/b;

    display.clearDisplay();
    Serial.print(temperature);                    // Display the results
    Serial.print(F("*C   "));
    Serial.print(pressure);
    Serial.print(F("hPa   "));
    Serial.print(altitude);
    Serial.println(F("m"));

    Serial.println(a);
    Serial.println(b);
    Serial.println(c);
    Serial.println(100*pow(euler,c));
    Serial.println(F("%RH"));


  print_info_oled(temperature, pressure, altitude, c);

  }

  //
  readRFID();

}
