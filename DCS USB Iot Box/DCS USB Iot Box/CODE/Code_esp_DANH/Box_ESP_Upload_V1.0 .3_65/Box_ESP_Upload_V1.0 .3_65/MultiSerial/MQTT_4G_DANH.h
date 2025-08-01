#include <PubSubClient.h>

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char* topic, byte* payload, unsigned int len) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  Serial.println();

  // Only proceed if incoming message's topic matches
//   if (String(topic) == topicLed) 
//   {
//     ledStatus = !ledStatus;
//     digitalWrite(LED_PIN, ledStatus);
//     mqtt.publish(topicLedStatus, ledStatus ? "1" : "0");
//   }
}

boolean mqttConnect(PubSubClient &mqtt, const char *mynamedevice) 
{
  char au_topic_pub[100];
  Serial.print("Connecting to ");
  Serial.print("mqtt.altacloud.biz");

  // Connect to MQTT Broker
//   boolean status = mqtt.connect("GsmClientTest");

  // Or, if you want to authenticate MQTT:
  sprintf(au_topic_pub, "PRINTER/%s/STATUS", mynamedevice);
//   mynamedevice = my_device;
  boolean status = mqtt.connect(mynamedevice, "altamedia" , "Altamedia");

  if (status == false) {
    Serial.println(" fail");
    return false;
  }
  Serial.println(" success");
  const char* datastatus = "online";
  Serial.println(au_topic_pub);
  mqtt.publish(au_topic_pub, datastatus);
//   mqtt.subscribe(topicLed);
  return mqtt.connected();
}

void setup_mqtt(PubSubClient &mqtt, char *domain)
{
    mqtt.setServer(domain, 1883);
    mqtt.setCallback(mqttCallback);
}

void reconnect_mqtt(PubSubClient &mqtt, const char *mynamedevice)
{
    if (!mqtt.connected()) 
    {
        Serial.println("=== MQTT NOT CONNECTED ===");
        // Reconnect every 10 seconds
        uint32_t t = millis();
        if (t - lastReconnectAttempt > 10000L) {
        lastReconnectAttempt = t;
        if (mqttConnect(mqtt, mynamedevice)) { lastReconnectAttempt = 0; }
        }
        delay(100);
        return;
    }

    mqtt.loop();
}