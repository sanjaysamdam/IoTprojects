// Blynk template ID and name for the project
#define BLYNK_TEMPLATE_ID "TMPL3NdkePXVx"
#define BLYNK_TEMPLATE_NAME "IoT weather monitoring"

// Include required libraries
#include <Wire.h>   // Include the Wire library for I2C communication
#include <LiquidCrystal_I2C.h>   // Include the LiquidCrystal_I2C library for I2C-based LCD display control
#include <DHT.h>    // Include the DHT library for DHT sensor interaction
#include <Adafruit_Sensor.h>    // Include the Adafruit_Sensor library for sensor abstraction
#include <ESP8266WiFi.h>      // Include the ESP8266WiFi library for Wi-Fi connectivity
#include <BlynkSimpleEsp8266.h>   // Include the BlynkSimpleEsp8266 library for Blynk integration with ESP8266

// Pin definitions
#define GASPIN A0  //analog pin A0 of gas sensor connected to A0 pin of nodemcu
#define DHTPIN D3  //signal pin of dht sensor connected to D3 pin of the nodemcu
#define LIGHTPIN D4 //signal pin of ldr sensor connected to D4 pin of the nodemcu
#define RAINPIN D5  //digital signal pin (D0) of  rainvalue sensor connected to D5 pin of the nodemcu

#define DHTTYPE DHT11   // DHT sensor type

// Blynk authentication token, Wi-Fi credentials
char auth[] = "*******************";
char ssid[] = "**********";
char password[] = "*********";

// Global variables 
DHT dht(DHTPIN, DHTTYPE);
int temperature = 0;
int humidity = 0;
int gasValue = 0;
int rainValue = 0;
int ldrValue = 0;

LiquidCrystal_I2C lcd(0x3F, 16, 2);    // Initialize the LCD display with the I2C address 0x3F, 16 columns, and 2 rows

void setup() {
  Serial.begin(9600);


  Blynk.begin(auth, ssid, password);    // Initialize Blynk connection

  dht.begin();   // Initialize DHT sensor 

  // Set pin modes
  pinMode(GASPIN, INPUT);
  pinMode(RAINPIN, INPUT);
  pinMode(LIGHTPIN, INPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
}

void loop() {
  // Read sensor values
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  gasValue = analogRead(GASPIN);
  rainValue = !digitalRead(RAINPIN);  // ( !=logical not ) invert the value, read by the sensor
  ldrValue = !digitalRead(LIGHTPIN);  // ( !=logical not ) invert the value, read by the sensor
  
  // Run Blynk
  Blynk.run();
  
  // Update LCD display with temperature
  lcd.clear();   //clear the display
  lcd.setCursor(0, 0);  //set cursor at 0th row and 0th column <lcd.setCursor(column,row)> .Column (0 to 15) and row (0 to 1)
  lcd.print("TEMP: ");   //Display TEMP in lcd
  lcd.print(temperature); //Display the value stored in temperature on lcd
  lcd.setCursor(12, 0);   //set cursor at 0th row and 12th column
  lcd.write(byte(0xDF)); // Display the degree(°) symbol
  lcd.print("C");     // Display C on lcd
  Blynk.virtualWrite(V0, temperature);   // Send the temperature value to the Blynk app, updating virtual pin V0

  // Update LCD display with humidity
  lcd.setCursor(0, 1);  //set cursor at 1st row and 0th column
  lcd.print("HUMIDITY: ");   //Display HUMIDITY on lcd
  lcd.print(humidity);     //Display the value stored in humidity on lcd
  lcd.print(" %");      // Display the percentage symbol
  Blynk.virtualWrite(V1, humidity);    // Send the humidity value to the Blynk app, updating virtual pin V1
  delay(2000);  //display the text for 2 sec
  
  // Update LCD display with rain status
  lcd.clear();     //clear the display  
  lcd.setCursor(0, 0);   //set cursor at 0th row and 0th column 
  lcd.print("RAINING: ");   //Display RAINING on lcd
  if (rainValue == HIGH) {   //compile if rainValue is HIGH
    lcd.print("Yes");          //Display Yes on lcd
    Blynk.logEvent("rain_alert", "RAIN COME");    // Log an event with the label "rain_alert" and message "RAIN COME" in the Blynk app . (NOTIFY ON THE USER MOBILE)
  } else {                    //compile if rainValue is LOW
    lcd.print("No");         //Display No on lcd
  }
  Blynk.virtualWrite(V3, rainValue);     // Send the rainValue value to the Blynk app, updating virtual pin V3

  // Update LCD display with light status
  lcd.setCursor(0, 1);      //set cursor at 1st row and 0th column 
  lcd.print("LIGHT: ");    //Display LIGHT on lcd 
  lcd.print(ldrValue ? "yes" : "no");    // Print "yes" if ldrValue is non-zero (true), otherwise print "no"
  Blynk.virtualWrite(V4, ldrValue);   // Send the ldrValue value to the Blynk app, updating virtual pin V4
  delay(2000);   //display the text for 2 sec
  
  // Update LCD display with gas status
  lcd.clear();     //clear the display
  lcd.setCursor(0, 0);    //set cursor at 0th row and 0th column   
  lcd.print("GasLevel: ");   //Display GasLevel on lcd
  lcd.print(gasValue);       //Display the value stored in gasValue on lcd
  Blynk.virtualWrite(V2, gasValue);   // Send the gasValue value to the Blynk app, updating virtual pin V2
  lcd.setCursor(0, 1);     //set cursor at 1st row and 0th column
  if (gasValue < 650)      // If gasValue is less than 650, print ...SAFE... on the LCD
  {    
    lcd.print("...SAFE...");
  } else                // If gasValue is 650 or greater
   {
    lcd.print("...UNSAFE...");   // print ...UNSAFE.. on the LCD
    Blynk.logEvent("gas_alert", "Gas leakage");  // Log an event with the label "gas_alert" and the message "Gas leakage" in the Blynk app .(NOTIFY ON THE USER MOBILE)  
    }
  delay(1500);   //display the text for 1.5 sec  
}
