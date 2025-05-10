#include <OneWire.h>
#include <DallasTemperature.h>
#include <HardwareSerial.h>

// DS18B20 Setup
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// BP Module (UART2)
HardwareSerial BP_Serial(2);  // RX2=GPIO16, TX2=GPIO17 (unused)

// Variables
float temperature = 0;
int bpSys = 0, bpDia = 0;
unsigned long lastValidBP = 0;

void setup() {
  Serial.begin(115200);
  tempSensor.begin();
  
  // Initialize BP Module
  BP_Serial.begin(9600);
  Serial.println("System Ready");
  Serial.println("---------------------");
}

void loop() {
  // Read temperature
  tempSensor.requestTemperatures();
  temperature = tempSensor.getTempCByIndex(0);
  
  // BP Module Handling
  if (BP_Serial.available()) {
    String rawData = BP_Serial.readStringUntil('\n');
    rawData.trim();
    
    if (validateBPData(rawData)) {
      parseBPData(rawData);
      lastValidBP = millis();
    }
  }

  printReadings();
  
  if (millis() - lastValidBP > 30000) {
    BP_Serial.updateBaudRate(9600);
    lastValidBP = millis();
  }
  
  delay(1000);
}

bool validateBPData(String data) {
  if (data.length() < 5 || data.indexOf(',') == -1) return false;
  int comma1 = data.indexOf(',');
  int comma2 = data.indexOf(',', comma1 + 1);
  if (comma1 <= 0 || comma2 <= 0) return false;
  
  int sys = data.substring(0, comma1).toInt();
  int dia = data.substring(comma1 + 1, comma2).toInt();
  return (sys > 50 && sys < 250 && dia > 30 && dia < 150);
}

void parseBPData(String data) {
  int comma1 = data.indexOf(',');
  int comma2 = data.indexOf(',', comma1 + 1);
  bpSys = data.substring(0, comma1).toInt();
  bpDia = data.substring(comma1 + 1, comma2).toInt();
}

void printReadings() {
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.println("°C");
}