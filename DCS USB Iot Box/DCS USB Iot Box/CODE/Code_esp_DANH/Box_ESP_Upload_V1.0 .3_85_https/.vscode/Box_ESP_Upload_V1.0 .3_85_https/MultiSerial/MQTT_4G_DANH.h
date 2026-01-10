#include <PubSubClient.h>

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char* topic, byte* payload, unsigned int len) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  Serial.println();

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
  boolean status = mqtt.connect(mynamedevice, "altamedia" , "Altamedia",au_topic_pub,1,true,"Offline");

  if (status == false) {
    Serial.println(" fail");
    return false;
  }
  Serial.println(" success");
  const char* datastatus = "Online";
  Serial.println(au_topic_pub);
  mqtt.publish(au_topic_pub, datastatus);
//   mqtt.subscribe(topicLed);
  return mqtt.connected();
}

void setup_mqtt(PubSubClient &mqtt, char *domain)
{
    mqtt.setServer(domain, 1883);
    mqtt.setKeepAlive(900);
    mqtt.setSocketTimeout(30000);
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

