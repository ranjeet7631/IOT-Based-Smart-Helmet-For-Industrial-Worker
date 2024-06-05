#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050.h>

#define TEMPERATURE_PIN A0
#define GAS_PIN A1
#define HUMIDITY_PIN 2
//#define DHT_PIN 2
//#define DHT_TYPE DHT11
#define BUZZER_PIN 8
#define TEMP_LED_PIN 9
#define DATA_LED_PIN 10
#define GAS_LED_PIN 11

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 for a 16 chars and 2 line display
MPU6050 mpu;
//DHT dht(DHT_PIN, DHT_TYPE);

int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup() {
  Serial.begin(9600);
  lcd.init();  // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.begin(16, 2); // Specify the number of columns and rows
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TEMP_LED_PIN, OUTPUT);
  pinMode(DATA_LED_PIN, OUTPUT);
  pinMode(GAS_LED_PIN, OUTPUT);
  digitalWrite(TEMP_LED_PIN, LOW);
  digitalWrite(DATA_LED_PIN, LOW);
  digitalWrite(GAS_LED_PIN, LOW);

  Wire.begin();
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println("MPU6050 Connection successful");
  } else {
    Serial.println("MPU Connection failed");
  }
}

void loop() {
  float temperature = getTemperature();
  float humidity = getHumidity();
  float gas = getGas();
  bool fallDetected = checkForFall();

  // Format the data string in the required format
  String data = "field1=" + String(temperature) + "&field2=" + String(humidity) + "&field3=" + String(gas) + "&field4=" + String(fallDetected ? "1" : "0");

  // Send sensor data over serial to NodeMCU
  Serial.println(data);

  // Indicate data transmission
  digitalWrite(DATA_LED_PIN, HIGH);
  delay(100);
  digitalWrite(DATA_LED_PIN, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");

  if (gas > 3) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(GAS_LED_PIN, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Warning: Gas!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(GAS_LED_PIN, LOW);
  }

  delay(1000);

  // Temperature check
  if (temperature > 35) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(TEMP_LED_PIN, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Warning: Temperature!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(TEMP_LED_PIN, LOW);
  }

  delay(2000);

  // Fall Detection
  if (fallDetected) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Warning: Fall!");
    digitalWrite(BUZZER_PIN, HIGH);
  }

  delay(1000);
}

float getTemperature() {
  int sensorValue = analogRead(TEMPERATURE_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  float temperatureC = (voltage - 0.5) * 100;
  if(temperatureC < 0){
      return temperatureC * -1;
    }
  return temperatureC ;
}

float getHumidity() {
  int sensorValue = analogRead(HUMIDITY_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  float humidity = voltage * 100 / 5; // Assuming humidity sensor outputs 0-5V
  return humidity;
}

float getGas() {
  int sensorValue = analogRead(GAS_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  return voltage;
}

bool checkForFall() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  int threshold = 25000; // change according to your use case
  if (abs(ax) > threshold || abs(ay) > threshold || abs(az) > threshold) {
    return true;
  }
  return false;
}
