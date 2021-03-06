//GLOBALS
bool first = true;
Packet readings;
SIP sip(0.2, 0.2, SIP_STEP_THRESHOLD, HEARTBEAT);
#ifdef DEBUG
uint32_t count = 0;
#endif

void resetErrors(){
  /* the error code has been transmitted and so can now be reset.  The
   * method of resetting used here allows for errors to have occurred
   * between sending the message and receiving acknowledgement. */
  if (last_transmitted_errno < last_errno && last_transmitted_errno != 0)
    last_errno = last_errno / last_transmitted_errno;
  else
    last_errno = 1;
}

/**
 * The function takes readings from the battery sensors, and checks the ADXL345 for
 * an interrupt. The data is then checked if it is eventful, and if so is stored
 * to an SD card.
 */
void doSenseCycle()
{
  seq_t seq;

  dbg("Start Sense " + String(++count));

  bool result_transmit = false;
  bool result_store = false;
  bool result_final = false;

  // TODO check this code
  if (last_errno != 1) readings.error = last_errno;
  last_transmitted_errno = last_errno;

  if (sip.update(&readings.steps, &seq, adxl_step, rtc.getEpoch())) {
      readings.rawSteps = adxl_step;
      readings.seq = seq;
      readings.solarBatt = getSolarBatteryVoltage();
      readings.nodeBatt = getBatteryVoltage();

      if (connectGSM()) {
	if (connectMQTT()) {
	  readings.gsmTime = getGSMTime();
	  String JSON = readings.json();
	  dbg("JSON created : " + JSON);
	  result_transmit = transmit(MQTT_TOPIC, JSON);

	  disconnectMQTT();
	}
	disconnectGSM();
      }

      if (result_transmit)
	sip.transmitted_ok(seq);

      result_store = writeDataToFile(&readings);

      result_final = result_transmit && result_store;

      if (result_final)
	{
	  // If this was the first successfull cycle, turn off the LED
	  if (first){
	    nodeFunctional();
	    first = false;
	  }

	  resetErrors();
	}
    }

  dbg("End Sense");
}
