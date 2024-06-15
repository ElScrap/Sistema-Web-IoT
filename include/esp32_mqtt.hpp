#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient mqttclient(espClient);
char topico[150];
String mqtt_data = "";
long lastMqttReconnectAttempt = 0;
long lastMsg = 0;
// WebSocket instance
void callback(char *topic, byte *payload, unsigned int length);
String Json();
// MQTT CONEXION
boolean mqtt_connect()
{
    mqttclient.setServer(mqtt_server, mqtt_port);
    mqttclient.setCallback(callback);
    log("Info: Intentando conexión al Broker MQTT...");
    if (mqttclient.connect(mqtt_id, mqtt_user, mqtt_passw))
    {
        log("Info: Conectado al Broker MQTT");
        String topico_subscribe = String(mqtt_user) + "/" + mqtt_id + "/command";
        mqttclient.subscribe(topico_subscribe.c_str());
        mqttclient.publish(topico_subscribe.c_str(), "ok");
        String topico_dispositivo = "+/+/dispositivo/command";
        mqttclient.subscribe(topico_dispositivo.c_str());
    }
    else
    {
        log("Error: failed, rc= " + mqttclient.state());
        return false;
    }
    return true;
}
// Manejo de los Mensajes Entrantes
void callback(char *topic, byte *payload, unsigned int length)
{
    String mensaje = "";
    char *topicCopy = strdup(topic);
    char *token = strtok(topicCopy, "/");
    for (uint16_t i = 0; i < length; i++)
    {
        mensaje += (char)payload[i];
        mqttRX();
    }
    if (strstr(topic, "/dispositivo/command"))
    {
        String parts[4];
        int index = 0;
        while (token != NULL)
        {
            parts[index] = String(token);
            index++;
            token = strtok(NULL, "/");
        }
        if (index == 4)
        {
            String dispositivo = parts[0];
            String modelo = parts[1];
            modelomovil = modelo;
            emeidispo = dispositivo;
            // Serial.print("dispositivo: " + dispositivo);
            // Serial.print(" modelo: " + modelo);
            // Serial.print("TOPIC: " + String(topic));
            if (mensaje=="Estado_Activado"||mensaje=="Estado_Enuso"||mensaje=="Estado_Inactivo")
            {
                estadomovil=mensaje;
            }else{
                mensajecoordenadas=mensaje;
            }
            
        }
    }
    
    topiccc="";
    mensaje.trim();
    OnOffRelays(mensaje);
    log("Info: Topico -->" + String(topic));
    log("Info: Mensaje -->" + mensaje);
    free(topicCopy);
}

// Manejo de los Mensajes Salientes
void mqtt_publish()
{
    String topic = String(mqtt_user) + "/" + mqtt_id + "/values";
    mqtt_data = Json();
    mqttclient.publish(topic.c_str(), mqtt_data.c_str());
    mqtt_data = "";
    mqttTX();
}
void mqtt_publish2()
{
    if (!mensajemovil.isEmpty())
    {   Serial.print("PUBLICADO EN: " + topiccc + " MENSAJE: " + mensajemovil);
        mqttclient.publish(topiccc.c_str(), mensajemovil.c_str());
        mensajemovil = "";
        topiccc="";
        mqttTX();
    }
}
void mqtt_publishglobal()
{
    if (!mensajeGlobal.isEmpty())
    {   Serial.print("PUBLICADO EN: GLOBAL MENSAJE: " + mensajeGlobal);
        String topicoglobal= "global/dispositivo/command";
        mqttclient.publish(topicoglobal.c_str(), mensajeGlobal.c_str());
        mensajeGlobal = "";
        mqttTX();
    }
}
// JSON con informacion de envio por MQTT
String Json()
{
    DynamicJsonDocument jsonDoc(3000);
    jsonDoc["wifi_dbm"] = WiFi.status() == WL_CONNECTED ? String(WiFi.RSSI()) : F("0");
    jsonDoc["wifi_percent"] = WiFi.status() == WL_CONNECTED ? String(getRSSIasQuality(WiFi.RSSI())) : F("0");
    jsonDoc["temp_cpu"] = String(TempCPUValue());
    jsonDoc["ram_available"] = String(ESP.getFreeHeap() * 100 / ESP.getHeapSize());
    jsonDoc["flash_available"] = String(round(SPIFFS.usedBytes() * 100 / SPIFFS.totalBytes()), 0);
    jsonDoc["relay1_status"] = String(Relay01_status ? "true" : "false");
    jsonDoc["relay2_status"] = String(Relay02_status ? "true" : "false");
    String response;
    serializeJson(jsonDoc, response);
    return response;
}
// MQTT Loop Principal
void mqttLoop()
{
    if (mqtt_enable)
    {
        if (!mqttclient.connected())
        {
            long now = millis();
            if ((now < 60000) || ((now - lastMqttReconnectAttempt) > 120000))
            {
                lastMqttReconnectAttempt = now;
                if (mqtt_connect())
                {
                    lastMqttReconnectAttempt = 0;
                }
                ledEncendido(MQTTLED);
            }
        }
        else
        {
            mqttclient.loop();
            // ledApagado(MQTTLED);
        }
    }
}