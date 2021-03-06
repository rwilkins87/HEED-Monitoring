//Include MQTT library
#include <MQTT.h>
MQTTClient client(MQTT_JSON_BUFFER);

//SETUP TOPIC STRTNG BASED ON NODE_ID
const String MQTT_TOPIC = String("Lantern/") + NODE_ID + "/data";

/**
 * Connects the node to the MQTT broker
 */
bool connectMQTT() {
  if (!client.connected())
    {
      //Set options: Keep alive for 10 hours, clean session, 1 second timeout
      client.setOptions(MQTT_KEEP_ALIVE, MQTT_CLEAN_SESSION, MQTT_TIMEOUT);
      client.begin(MQTT_SERVER, net);  //Start a connection
      dbg("MQTT: Connecting to broker...");
      bool res = client.connect("Lantern1", MQTT_USER, MQTT_PASS);
      if(res){
        dbg("MQTT: Connected!");
        return true;
      }
      else {
	digitalWrite(LED_BUILTIN, LOW);
	reportError(ERR_MQTT_CONNECTION_FAILED);
	dbg("MQTT: Can't connect!");
	return false;
      }
    }
  else return true;
}


/**
 * Disconnects the node from the MQTT broker
 */
void disconnectMQTT() {
  client.disconnect();
  dbg("MQTT: Disconnected");
}

/**
 * Transmits a packet via MQTT
 * @param topic The MQTT topic to send to
 * @param dataString The String to be sent
 * @return True if the String was saved
 */
bool transmit(String topic, String dataString) {
  char* cPayload;
  cPayload = &dataString[0u];
  bool res = client.publish(topic, cPayload);
  if (!res){
    reportError(ERR_MQTT_TRANSMISSION_FAILED);
    dbg("ERROR: MQTT: Data not sent!");
  }
  else dbg("MQTT: Data sent!");
  return res;
}
