//libreria
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <TimeLib.h>
// Archivos *hpp - Fragmentar el Codigo
#include "configuracion.hpp"
#include "functions.hpp"
#include "configuracionReset.hpp"
#include "configuracionRead.hpp"
#include "configuracionSave.hpp"
#include "esp32_wifi.hpp"
#include "esp32_mqtt.hpp"
#include "esp32_websocket.hpp"
#include "esp32_server.hpp"
//Setup
void setup() {
  Serial.begin(115200);//velocidad
  setCpuFrequencyMhz(240);//frecuencia de CPU
  log("\nInfo: Iniciando Setup");//inicio del log por serial
  configPines();//configurar los pines
  if (!SPIFFS.begin(true))//chequeo incio de SPIFFS
  {
    log(F("Error: Falló la inicialización del SPIFFS"));
    while(true);
  }
  //leer los estados de los relays
  if(!configuracionReadRelays()){
        //salvar estados de los relays
        settingsSaveRelays();
  }
  ledEncendidoApagado(RELAY1,Relay01_status);//Paso estados a los pines de rele
  ledEncendidoApagado(RELAY2,Relay02_status);//Paso estados a los pines de rele
  //leer configuracion wifi
  if(!configuracionReadWIFI()){        
    // Salvar las configuraciones del WIFI
    configuracionSaveWiFi();
  }
  bootCount++;  //Incrementar el contador de reinicios
  configuracionSaveWiFi();//Salvar las configuraciones del WiFi
  WiFi.disconnect(true);//configuracion wifi
  delay(1000);
  wifi_setup();//setup del wifi
  //lee la configuracion mqtt
  if(!settingsReadMQTT()){
    // Salvar la configuracion del MQTT
    settingsSaveMQTT();
  } 
  //leer usuario y contras
  if(!settingsReadUsuario()){
    // Salvar el usuario y Contraseña
    settingsSaveAdmin();
  }  
  //iniciar ws
  InitWebSockets();//Iniciar el webSocket
  InitServer(); //Inicializar el servidor
  //listDir(SPIFFS,"/",0);// Devuelve la lista de carpetas y archivos del SPIFFS ONLYDEBUG
  log("Info: Setup Completado");
}
// Loop Pincipal Nucleo 0
void loop() {
    yield();
    // WIFI para que lea el modo
    if (wifi_mode == WIFI_STA){
        wifiLoop();
    }else if (wifi_mode == WIFI_AP){
        wifiAPLoop();
    }
    // MQTT para que lea el modo
    if ((WiFi.status() == WL_CONNECTED) && (wifi_mode == WIFI_STA)){
        if(mqtt_server != 0){
            mqttLoop();
            if (mqttclient.connected()){
                if (millis() - lastMsg > mqtt_time){
                    lastMsg = millis();
                    mqtt_publish();
                }
                mqtt_publish2();
                mqtt_publishglobal();
            }      
        }
    }
    // Enviar JSON por WS 
    if (millis() - lastWsSend > 1000){
        lastWsSend = millis();
        WsMessage(GetJson(), "");
    }
}