#include <SparkFun_RHT03.h>

const int RHT03_DATA_PIN = 4; // RHT03 data pin
RHT03 rht;

void setup() {
  Serial.begin(115200); // Serial is used to print sensor readings.
  // Call rht.begin() to initialize the sensor and our data pin
  rht.begin(RHT03_DATA_PIN); 
}

void loop()
{
  // Call rht.update() to get new humidity and temperature values from the sensor.
  int updateRet = rht.update();
  // If successful, the update() function will return 1.
  // If update fails, it will return a value <0
  if (updateRet == 1) {
    // The humidity() and tempC() functions can be called -- after 
    // a successful update() -- to get the last humidity and temperature
    // value 
    float latestHumidity = rht.humidity();
    float latestTempC = rht.tempC();
    Serial.println("Humidity: '" + String(latestHumidity, 3) + "' %, " + "Temp (C): '" + String(latestTempC, 3) + "' deg C");
  }
  else {
    // If the update failed, try delaying for RHT_READ_INTERVAL_MS ms before
    // trying again.
    delay(RHT_READ_INTERVAL_MS);
  }
  delay(1000);
}
