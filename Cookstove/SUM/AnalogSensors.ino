//Include 1-wire and thermocouple libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SparkFun_Si7021_Breakout_Library.h>
#include <Wire.h>

// initialize the library instances
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Weather Si7021;


/**
 * Setups the Analog sensors
 * 1. Sets the reference voltage
 * 2. Sets up the pin for powering the MAX31850
 * 3. Turns of the LED
 */
void setupAnalogSensors() {
  analogReference(AR_DEFAULT); //3.3V

  //Set MAX3150 power pin to output and make sure it is off
  pinMode(MAX31850_POWER_PIN, OUTPUT);
  digitalWrite(MAX31850_POWER_PIN, LOW);

  //Set Si7021 power pin to output and make sure they are off
  pinMode(Si7021_POWER_PIN, OUTPUT);
  digitalWrite(Si7021_POWER_PIN, LOW);

  //Turn the built in LED on
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  dbg("Digital pins Set");
}

/**
 * Power up the MAX31850
 */
void powerMAX31850() {
  digitalWrite(MAX31850_POWER_PIN, HIGH);
  delay(DIGITAL_ON_WAIT); //Slight delay to allow the switch to happen
  dbg("MAX31850: On");
}

/**
 * Remove power from the MAX31850
 */
void unpowerMAX31850() {
  digitalWrite(MAX31850_POWER_PIN, LOW);
  dbg("MAX31850: Off");
}

/**
 * Gets the current temperature for the MAX31850 1-wire
 * interface
 * @param pointer to a data struct
 */
void getTemperatureThermocouple(Data* readings) {
  powerMAX31850(); //turn on sensor

  sensors.requestTemperatures(); // Send the command to get temperatures
  readings->tempThermocouple = sensors.getTempCByIndex(MAX31850_ADDR); //get temp
  dbg(String("MAX31850: Temp: ") + String(readings->tempThermocouple) + "c");

  unpowerMAX31850(); //turn off sensor
}

/**
 * Power up the Si7021
 */
void powerSi7021() {
  digitalWrite(Si7021_POWER_PIN, HIGH);
  delay(DIGITAL_ON_WAIT);
  Si7021.begin();
  delay(DIGITAL_ON_WAIT);
  dbg("Si7021: On");
}

/**
 * Remove power from the Si7021
 */
void unpowerSi7021() {
  digitalWrite(Si7021_POWER_PIN, LOW);
  dbg("Si7021: Off");
}

/**
 * Gets the current temperature for the Si7021 sensor
 * interface
 * @param pointer to a data struct
 */
void getSi7021Data(Data* readings) {

  powerSi7021(); //turn on sensor

  readings->tempSi7021 = Si7021.getTemp(); // Send the command to get temperatures
  readings->humidity = Si7021.getRH(); // Send the command to get humidity
  dbg(String("Si7021: Temp: ") + String(readings->tempSi7021) + "c");
  dbg(String("Si7021: Humidity: ") + String(readings->humidity) + "%");

  unpowerSi7021(); //turn off sensor
}


/**
 * Gets the battery voltage from the MKR Zero's internal
 * voltage divider
 * @param pointer to a data struct
 */
void getBatteryVoltage(Data* readings)
{
  readings->nodeBatt = analogRead(ADC_BATTERY);
  if (readings->nodeBatt < BATTERY_LOW_VOLTAGE){
    batteryLow = true;
  }
}
