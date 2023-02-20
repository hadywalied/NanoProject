#include <Arduino.h>
#include <HX711.h>
#include <EEPROM.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;



// Known Weight def, EEPROM Index
const int KNOWN_WEIGHT = 3730;
const int IDX = 0;


HX711 scale;

void setup() {
  Serial.begin(9600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void loop() {

  if (scale.wait_ready_timeout(1000)) {
    scale.set_scale();    
    // Serial.println("Tare... remove any weights from the scale.");
    // delay(5000);
    // scale.tare();
    // Serial.println("Tare done...");
    // Serial.print("Place a known weight on the scale...");
    // delay(5000);
    long reading = scale.read_average(10);
    Serial.print("Result: ");
    Serial.println(reading);
    
    // float calibration_factor = reading / KNOWN_WEIGHT;
    // Serial.print("Calibration Factor: ");
    // Serial.println(calibration_factor);
    
    // EEPROM.put(IDX, calibration_factor);

  } else {
    Serial.println("HX711 not found.");
  }

  
}