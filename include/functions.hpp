/* -------------------------------------------------------------------
 * Elaborado por: Leonardo Aguilar
 * Correo: leonardo-aguilar@hotmail.es
 * Plataforma ESP32-Web
 * Sistema Web para la gestion de dispositivos IoT y Moviles
 * V1.0.0-2024
 * -------------------------------------------------------------------
*/
#include "ledBlink.hpp"
// Sensor Temp Interno CPU
#ifdef __cplusplus
extern "C"
{
#endif
    uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();
//Genera un log en el puerto serial
void log(String s){
    Serial.println(s);
}
//Definir la plataforma
String platform(){
    //Optiene la Placa de desarollo
    #ifdef ARDUINO_ESP32_DEV
        return "ESP32";
    #endif
}
//Convierte un char a IP
IPAddress CharToIP(const char *str){
    sscanf(str,"%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
    return IPAddress(ip[0],ip[1],ip[2],ip[3]);
}
// Retorna segundos como "d:hh:mm:ss"
String longTimeStr(const time_t &t){        
    String s = String(t / SECS_PER_DAY) + ':';
    if (hour(t) < 10)
    {
        s += '0';
    }
    s += String(hour(t)) + ':';
    if (minute(t) < 10)
    {
        s += '0';
    }
    s += String(minute(t)) + ':';
    if (second(t) < 10)
    {
        s += '0';
    }
    s += String(second(t));
    return s;
}

//Convertidor de Ip a string
String ipStr(const IPAddress &ip){
    String sFn ="";
    for (byte bFn = 0; bFn < 3; bFn++){
        sFn += String((ip>>(8*bFn)) & 0xFF) + ".";
    }
    sFn += String(((ip >> 8 * 3 ))& 0xFF);
    return sFn;
}
// De HEX a String
String hexStr(const unsigned long &h, const byte &l = 8){
    String s;
    s = String(h, HEX);
    s.toUpperCase();
    s = ("00000000" + s).substring(s.length() + 8 - l);
    return s;
}
// Crear un ID unico desde la direccion MAC
String idUnique(){
    // Retorna los ultimos 4 Bytes del MAC rotados
    char idunique[15]; 
    uint64_t chipid = ESP.getEfuseMac();           
    uint16_t chip = (uint16_t)(chipid >> 32);
    snprintf(idunique, 15, "%04X", chip);
    return idunique;
}
// ID del servidor para La Base de Datos
const String device_id = hexStr(ESP.getEfuseMac()) + "LI" + String(idUnique()); 
// ID del servidor para La Base de Datos
String deviceID(){
    return String(platform()) + hexStr(ESP.getEfuseMac()) + String(idUnique());
}
//Configurar los pines de salida
void configPines(){
   pinMode(WIFILED, OUTPUT);
    pinMode(MQTTLED, OUTPUT);
    pinMode(RELAY1, OUTPUT);  
    pinMode(RELAY2, OUTPUT); 
    //Los iniciamos en nivel bajo
    ledApagado(RELAY1);
    ledApagado(RELAY2);    
    ledApagado(MQTTLED);    
    ledApagado(WIFILED);
}
// Parpadeo LED MQTT Transmisión
void mqttTX(){ 
    for (int i = 0; i < 2; i++){
        ledEncendido(MQTTLED);
        delay(50);
        ledApagado(MQTTLED);
        delay(10);
    }  
}
// Parpadeo LED MQTT Recepción
void mqttRX(){
    for (int i = 0; i < 1; i++){
        blinkRandomSingle(5,50,MQTTLED);
        delay(5);
    }
}
// Parpadeo de los LEDs WIFI & MQTT
void leds(){
    for(int i=0; i < 4; i++) { 
        ledEncendido(MQTTLED);
        ledEncendido(WIFILED);
        delay(100);
        ledApagado(MQTTLED);
        ledApagado(WIFILED);
        delay(100);
    }
}
// Retorna la calidad de señal WIFI en %
int getRSSIasQuality(int RSSI){
    int quality = 0;
    if(RSSI <= -100){
        quality = 0;
    } else if(RSSI >= -50){
        quality = 100;
    } else{
       quality = 2 * (RSSI + 100); 
    }
    return quality;
}
// Retorna la temperatura del CPU
float TempCPUValue (){
    return temp_cpu = (temprature_sens_read() - 32) / 1.8;
}
// Retorna el listado de todos los archivos en el SPIFFS
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listado de Directorio: %s\r\n", dirname);
    File root = fs.open(dirname);
    if(!root){
        log("- No se pudo abrir el directorio");
        return;
    }
    if(!root.isDirectory()){
        log(" - No es un directorio");
        return;
    }
    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            log("  DIR : " + String(file.name()));
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            log("  ARCHIVO : " + String(file.name()));
            log("\tTAMAÑO : " + String(file.size()));
        }
        file = root.openNextFile();
    }
    log("  Total Espacio: " + String(SPIFFS.totalBytes()));
    log("  Total Espacio Usado:  " + String(SPIFFS.usedBytes()));
}
//Convierte un char a ip
IPAddress StrIP(const char *str){
    sscanf(str, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
    return IPAddress(ip[0], ip[1], ip[2], ip[3]);
}
// Nos Devuelve un String con el SweetAlert2

String SweetAlert(String TitleWeb, String SweetTitle, String SweetText, String SweetIcon, String type){
    String SweetAlert;
    if(type == "aviso"){
        SweetAlert = "<head>"
                        "<html><meta charset='UTF-8'>"
                        "<title>AdminTools | "+ TitleWeb +"</title>"
                        "<meta content='width=device-width, initial-scale=1.0' name='viewport' />"
                        "<link rel='icon' href='www/esp32.png' type='image/x-icon'>"
                        "<link rel='stylesheet' href='www/bootstrap.css' />"
                        "<link rel='stylesheet' href='www/main.css' />"
                        "<link rel='stylesheet' href='www/MoneAdmin.css' />"
                        "<link rel='stylesheet' href='www/font-awesome.css' />"
                        "<link rel='stylesheet' href='www/sweetalert2.min.css' />"
                        "<script src='www/jquery-2.0.3.min.js'></script>"
                        "<script src='www/bootstrap.min.js'></script>"
                        "<script src='www/modernizr-2.6.2.min.js'></script>"
                        "<script src='www/sweetalert2.min.js'></script>"
                    "</head>"
                    "<body>"
                    "<script>"
                        "Swal.fire({title: '"+SweetTitle+"!',"
                                        " text: '"+SweetText+"',"
                                        " icon: '"+SweetIcon+"',"
                                        " confirmButtonText: 'Cerrar'}).then((result) => {"
                                        "if (result.isConfirmed){"
                                            "history.back();"
                                        "};"
                                    "})"
                    "</script>"
                    "<body>"
                    "</html>";
    }else if(type == "accion"){
        SweetAlert = "<head>"
                        "<html><meta charset='UTF-8'>"
                        "<title>AdminTools | "+ TitleWeb +"</title>"
                        "<meta content='width=device-width, initial-scale=1.0' name='viewport' />"
                        "<link rel='icon' href='www/esp32.png' type='image/x-icon'>"
                        "<link rel='stylesheet' href='www/bootstrap.css' />"
                        "<link rel='stylesheet' href='www/main.css' />"
                        "<link rel='stylesheet' href='www/MoneAdmin.css' />"
                        "<link rel='stylesheet' href='www/font-awesome.css' />"
                        "<link rel='stylesheet' href='www/sweetalert2.min.css' />"
                        "<script src='www/jquery-2.0.3.min.js'></script>"
                        "<script src='www/bootstrap.min.js'></script>"
                        "<script src='www/modernizr-2.6.2.min.js'></script>"
                        "<script src='www/sweetalert2.min.js'></script>"
                    "</head>"
                    "<body>"
                    "<script>"
                        "Swal.fire({title: '"+SweetTitle+"!',"
                        " text: '"+SweetText+"',"
                        " icon: '"+SweetIcon+"',"
                        " showCancelButton: true,"
                        " confirmButtonColor: '#3085d6',"
                        " cancelButtonColor: '#d33',"
                        " confirmButtonText: 'Si, reiniciar',"
                        " cancelButtonText: 'Cancelar',"
                        " reverseButtons: true"
                        " }).then((result) => {"
                                    "if (result.isConfirmed){"
                                        "window.location = 'esp-servidor';"
                                        "}else if ("
                                        "result.dismiss === Swal.DismissReason.cancel"
                                        "){"
                                        "history.back();"
                                        "}"
                                    "})"
                    "</script>"
                    "<body>"
                    "</html>";
    }else{
        SweetAlert = "<head>"
                        "<html><meta charset='UTF-8'>"
                        "<title>AdminTools | "+ TitleWeb +"</title>"
                        "<meta content='width=device-width, initial-scale=1.0' name='viewport' />"
                        "<link rel='icon' href='www/portada_iot.jpg' type='image/x-icon'>"
                        "<link rel='stylesheet' href='www/bootstrap.css' />"
                        "<link rel='stylesheet' href='www/main.css' />"
                        "<link rel='stylesheet' href='www/MoneAdmin.css' />"
                        "<link rel='stylesheet' href='www/font-awesome.css' />"
                        "<link rel='stylesheet' href='www/sweetalert2.min.css' />"
                        "<script src='www/jquery-2.0.3.min.js'></script>"
                        "<script src='www/bootstrap.min.js'></script>"
                        "<script src='www/modernizr-2.6.2.min.js'></script>"
                        "<script src='www/sweetalert2.min.js'></script>"
                    "</head>"
                    "<body>"
                    "<script>"
                        "Swal.fire({title: '"+SweetTitle+"!',"
                                        " text: '"+SweetText+"',"
                                        " icon: '"+SweetIcon+"',"
                                        " confirmButtonText: 'Cerrar'}).then((result) => {"
                                        "if (result.isConfirmed){"
                                            "window.location = '/';"
                                        "};"
                                    "})"
                    "</script>"
                    "<body>"
                    "</html>";
    }
    return SweetAlert;
}
//Control de los Relay desde MQTT Y WS
boolean settingsSaveRelays();
void OnOffRelays(String command){
    DynamicJsonDocument JsonDoc(1024);
    deserializeJson(JsonDoc, command);
    if(JsonDoc["protocol"] == "WS"){
        log("Info: Commando por WS => " + command);
    }else{
        log("Info: Commando por MQTT => " + command);
    }
    serializeJsonPretty(JsonDoc, Serial);
    if (JsonDoc["value"]){
        digitalWrite(JsonDoc["output"] == "RELAY1" ? RELAY1 : RELAY2, HIGH);
        JsonDoc["output"] == "RELAY1" ? Relay01_status = HIGH : Relay02_status = HIGH ;
    }else {
        digitalWrite(JsonDoc["output"] == "RELAY1" ? RELAY1 : RELAY2, LOW);
        JsonDoc["output"] == "RELAY1" ? Relay01_status = LOW : Relay02_status = LOW ;
    }
    settingsSaveRelays();   
}


// Retorna el Tipo de Encriptacion segun el codigo (0-1-2-3-4-5)
String EncryptionType(int encryptionType) {
  switch (encryptionType) {
    case (0):
      return "Abierta";
    case (1):
      return "WEP";
    case (2):
      return "WPA_PSK";
    case (3):
      return "WPA2_PSK";
    case (4):
      return "WPA_WPA2_PSK";
    case (5):
      return "WPA2_ENTERPRISE";
    default:
      return "Bloqueada";
    }
}
