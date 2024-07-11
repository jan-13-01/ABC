#define BLYNK_TEMPLATE_ID "TMPL6FG6VkqkZ"
#define BLYNK_TEMPLATE_NAME "temp"
#define BLYNK_AUTH_TOKEN "SxF6ITny6nujiUmrcs0E8ze4SvIADJy_"

#define BLYNK_PRINT Serial // Kita menggunakan Blynk serial

#include <WiFi.h>           // Library WiFi
#include <WiFiClient.h>     // Library WiFiClient
#include <BlynkSimpleEsp32.h> // Library BlynkESP32

#include "DHT.h"
#include <LiquidCrystal_I2C.h>

#define ANALOG_SOIL 5
#define DIGITAL_SOIL 37
#define TRIG 48
#define ECHO 47
/*#define SDA 18
#define SCL 19*/
#define REL_1 41
#define REL_2 20
#define REL_3 21
#define DHTPIN 7

#define button1_vpin    V1
#define button2_vpin    V2
#define button3_vpin    V3 

#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

char ssid[] = "Tan";
char pass[] = "11111111";

BlynkTimer timer;

float temp;
float humid;
float moisture_val;
float distance_val;

int relay1_state = 0;
int relay2_state = 0;
int relay3_state = 0;
int counter1 = 0;
int counter2 = 0;
int counter3 = 0;

bool useWs = false;

BLYNK_CONNECTED() {
  Blynk.syncVirtual(button1_vpin);
  Blynk.syncVirtual(button2_vpin);
  Blynk.syncVirtual(button3_vpin);
}

BLYNK_WRITE(button1_vpin) {
  relay1_state = param.asInt();
  digitalWrite(REL_1, relay1_state);
}

BLYNK_WRITE(button2_vpin) {
  relay2_state = param.asInt();
  digitalWrite(REL_2, relay2_state);
}

BLYNK_WRITE(button3_vpin) {
  relay3_state = param.asInt();
  digitalWrite(REL_3, relay3_state);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(18, 19);

  dht.begin();

  pinMode(REL_1, OUTPUT);
  pinMode(REL_2, OUTPUT);
  pinMode(REL_3, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  lcd.init();
  lcd.backlight();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Tambahkan perintah Blynk untuk sinkronisasi tombol keadaan relay
  Blynk.virtualWrite(button1_vpin, relay1_state);
  Blynk.virtualWrite(button2_vpin, relay2_state);
  Blynk.virtualWrite(button3_vpin, relay3_state);

  // Atur timer untuk memantau kondisi dan mengontrol relay secara otomatis
  timer.setInterval(1000L, controlRelays);
}

void loop() {
  Blynk.run();
  timer.run();

  distance_val = readHigh();
  temp = readAir();
  humid = readHumid();
  float sensor_analog = analogRead(ANALOG_SOIL);
  float moisture = ( 100 - ( (sensor_analog/4095.00) * 100 ) );

  // Logika kontrol relay dan penulisan nilai ke Blynk
  if (temp == 33 && moisture < 50 && humid < 30) {
    relay1_state = 1;
  } else {
    relay1_state = 0;
  }

  if (distance_val > 22) {
    relay2_state = 1;
  } else {
    relay2_state = 0;
  }

  // Update LCD display
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.setCursor(0, 1);
  lcd.print(temp);
  /*lcd.print(" H:");
  lcd.print(humid);
  lcd.setCursor(0, 1);
  lcd.print("M:");
  lcd.print(moisture);
  lcd.print(" D:");
  lcd.print(distance_val);*/
  delay(1000);
  lcd.clear();

  Blynk.virtualWrite(V4, temp); 
  Blynk.virtualWrite(V5, humid); 
  Blynk.virtualWrite(V6, moisture); 
  Blynk.virtualWrite(V7, distance_val); 

  delay(1000);
}

void controlRelays() {
  // Kontrol relay secara otomatis berdasarkan kondisi
  digitalWrite(REL_1, relay1_state);
  digitalWrite(REL_2, relay2_state);
}

float readHigh() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  float duration = pulseIn(ECHO, HIGH);
  float distance = (duration * 0.0343) / 2;
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(100);

  return distance;
}

float readAir() {
  float t = dht.readTemperature();
  Serial.print(F("Temperature: "));
  Serial.print(t);
  return t;
}

float readHumid() {
  float h = dht.readHumidity();
  return h;
}
