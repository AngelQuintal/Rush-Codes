
#include <stdlib.h>
#include <string.h>
///OLED////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//BMP280E/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <BMP280_DEV.h>                           // Include the BMP280_DEV.h library

float temperature, pressure, altitude;            // Create the temperature, pressure and altitude variables
float a,b,c; //variables used for the long quation to get RH%
BMP280_DEV bmp280;                                // Instantiate (create) a BMP280_DEV object and set-up for I2C operation (address 0x77)

#define YDEWPOINT 24 //YUCATANS DEWPOINT
#define euler 2.718281828
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//RFID SENSOR/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid

//Constants
#define SS_PIN 5
#define RST_PIN 27

//Parameters
const int ipaddress[4] = {103, 97, 67, 25};

//Variables
byte nuidPICC[4] = {0, 0, 0, 0};

//RFID USERS
byte admin[4] = {50, 185, 4, 33};
byte user1[4] = {193, 215, 143, 25};

MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//PIR SENSOR/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define timeSeconds 1

// Set GPIOs for LED and PIR Motion Sensor
const int led = 2;
const int motionSensor = 14;

// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//INTERRUPOT BUTTON/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define pushButton_pin   35
boolean screen_flag=true; 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//WIFI/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "Quintal Home"; // Enter your WiFi name
const char *password = "Quintal2021";  // Enter WiFi password

//const char *ssid = "IZZI-75D2"; // Enter your WiFi name
//const char *password = "50A5DC3975D2";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "1/temp";
const char *topic2 = "1/hum";
const char *topic3 = "1/pres";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Checks if motion was detected, sets LED HIGH and starts a timer
void IRAM_ATTR detectsMovement() {
  Serial.println("MOTION DETECTED!!!");
  digitalWrite(led, HIGH);//WE NEED TO USE A VARIBABLE FOR SOME REASON THE LED CRASHES
  startTimer = true;
  lastTrigger = millis();
}

void IRAM_ATTR toggleLED(){//JUST TO TEST THIS IS NOT GOING TO BE IN THE FINAL VERSION
  Serial.println("TURNING SCREEN OFF");
  //display.ssd1306_command(SSD1306_DISPLAYOFF);
  digitalWrite(screen_flag, !digitalRead(screen_flag));
  Serial.println(screen_flag);
  Serial.println("TURNED OFF");
  //digitalWrite(led, !digitalRead(led));
}

void setup() {
  Serial.begin(115200);//SERIAL MONITOR

  //OLED/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  //BMP280/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  bmp280.begin();
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //RFID/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Serial.println(F("Initialize System"));
  //init rfid D8,D5,D6,D7
  SPI.begin();
  rfid.PCD_Init();

  Serial.print(F("Reader :"));
  rfid.PCD_DumpVersionToSerial();
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //PIR SENSOR/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);
    Serial.print("SOBREVIVO");
  // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);
    Serial.print("SOBREVIVO 2");
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // Set LED to LOW
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  //PIN INTERRUPT FOR LED2 AND THEN SCREEN
  pinMode(pushButton_pin, INPUT_PULLUP);
  attachInterrupt(pushButton_pin, toggleLED, RISING);

  //WIFI///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  WiFi.begin(ssid, password);

      Serial.println("Connected to the WiFi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public emqx mqtt broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    
     // publish and subscribe 
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void compare_user_stored(byte rfid_read[4]){//PUT THE ID HERE
   
   display.setCursor(0, 50);
   display.print("Name: ");//Printing on screen
   
  if( admin[0] == rfid_read[0] && admin[1] == rfid_read[1] && 
      admin[2] == rfid_read[2] && admin[3] == rfid_read[3] )
  {
       display.print("admin");//Printing on screen}
       display.print(",UNLOCKED!");//Printing on screen
       digitalWrite(led, HIGH);//OPENS DOOR
  }
  else
  {
    display.print("unknown");//Printing on screen
    digitalWrite(led, LOW);//CLOSE DOOR
    }
    
    }

void closed_door_message(){//WHEN THE DOOR CLOSES
  delay(5000);
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print(F("LOCK!"));
  display.display();
  digitalWrite(led, LOW);
  delay(1000);
  display.clearDisplay();
  display.display();
  }

void readRFID(void ) { /* function readRFID */

  //display.ssd1306_command(SSD1306_DISPLAYON);
  
  ////Read RFID card
  //byte andy[4]; DEBUGGING VARIABLE
  //display.ssd1306_command(SSD1306_DISPLAYON);//after display offed

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

//  Serial.print(F("RFID In HEX: "));
//  printHex(rfid.uid.uidByte, rfid.uid.size);
//  Serial.println();
  // Halt PICC
  
  Serial.print(F("RFID In dec: "));
  printDec(rfid.uid.uidByte, rfid.uid.size); //INSIDE THIS VOID MOST OF THE MAGIC HAPPENS
  Serial.println();

  Serial.print("PUTO\n");
  compare_user_stored(nuidPICC);
  display.display();

  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();


  closed_door_message();
  float humidity;
  humidity = 100*pow(euler,c);
  
  // String temp = String(temperature,2);
  char *data = (char *)malloc(sizeof(char) * 25);
  
  sprintf(data, "%.2f", temperature);
  client.publish(topic, data);
  client.subscribe(topic);
  delay(100);
  sprintf(data, "%.2f", humidity);
  client.publish(topic2, data);
  client.subscribe(topic2);
  delay(100);
  sprintf(data, "%.2f", pressure);
  client.publish(topic3, data);
  client.subscribe(topic3);
  delay(100);
  Serial.print("aqui!");
  // Serial.print(data);      
   
  
  
  
  
  //display.ssd1306_command(SSD1306_DISPLAYOFF);
  free(data);

  
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
void printDec(byte *buffer, byte bufferSize) {//PRINTS USER ID
  
  int j=0;
  display.clearDisplay();
  print_info_oled(temperature, pressure, altitude, c);//Info gets printed on screen

  display.setCursor(0, 40);
  display.print(F("user "));
  
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);

  display.print(buffer[i], DEC);//Printing on screen
  j=j+10;

  }

  j=0;
  
  
}

void Serial_BMP280_PRINT(float a, float b, float c, float temperature, float pressure, float altitude){//DISPLAY SENSOR VALUES IN SERIAL
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
}

void print_info_oled(float temperature, float pressure, float altitude, float humidity){//DISPLAY SENSOR VALUES IN OLED
  //display.ssd1306_command(SSD1306_DISPLAYON);
  //display.clearDisplay();
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
  //display.display();

  }

void checkPIR(){//
    // Current time
  now = millis();
  // Turn off the LED after the number of seconds defined in the timeSeconds variable
  if(startTimer && (now - lastTrigger > (timeSeconds*1000))) {
    Serial.println("Motion stopped...");
    digitalWrite(led, LOW);
    startTimer = false;
  }
  
  }

void loop() {  

  client.loop();
  checkPIR();//resseting PIR sensor and getting the status
  bmp280.startForcedConversion();// Start BMP280 forced conversion (if we're in SLEEP_MODE)

  if (bmp280.getMeasurements(temperature, pressure, altitude)){// Check if the measurement is complete

  a=(-4283.58*temperature)+(4283.58*YDEWPOINT);//simplification of a long equation to sums
  b=(YDEWPOINT+243.04)*(temperature+243.04);//simplification of a long equation to sums
  c=a/b;//simplification of a long equation to get the Relative Humidity in %
   
  //Serial_BMP280_PRINT(a,b,c,temperature, pressure, altitude);//Serial debugginh of the variables
  //print_info_oled(temperature, pressure, altitude, c);//Info gets printed on screen

  }

  //Serial_BMP280_PRINT(a,b,c,temperature, pressure, altitude);//Serial debugginh of the variables

    
  readRFID();//MAIN FUNCTION; WHEN NOTHING SLEEPS/IDLES
  
}