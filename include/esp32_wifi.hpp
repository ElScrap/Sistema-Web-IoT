/* -------------------------------------------------------------------
 * Elaborado por: Leonardo Aguilar
 * Correo: leonardo-aguilar@hotmail.es
 * Plataforma ESP32-Web
 * Sistema Web para la gestion de dispositivos IoT y Moviles
 * V1.0.0-2024
 * -------------------------------------------------------------------
*/
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
const byte DNSSERVER_PORT = 53;
DNSServer dnsServer;
IPAddress apIP(192,168,6,2);
IPAddress netMsk(255,255,255,0);
int wifi_mode = WIFI_STA;
unsigned long previousMillisWIFI = 0;
unsigned long intervalWIFI = 30000;
//hostname para ESPmDNS ejemplo http://SistemaWeb.local
const char *esp_hostname =id;

void startAP(){
    log("Info: Iniciando Modo AP");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    WiFi.softAPConfig(apIP,apIP, netMsk);
    WiFi.setHostname(deviceID().c_str());
    WiFi.softAP(ap_nameap,ap_passw,ap_canal,ap_hidden,ap_connet);
    log("Info: WIFI AP " + deviceID() + " - IP "+ ipStr(WiFi.softAPIP()));
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(DNSSERVER_PORT, "*", apIP);
    wifi_mode = WIFI_AP;
}
//Fucnion Conexion del cliente Wifi
void startClient(){
    if (wifi_staticIP){
        if (!WiFi.config(CharToIP(wifi_ip_static), CharToIP(wifi_gateway), CharToIP(wifi_subnet), CharToIP(wifi_primaryDNS), CharToIP(wifi_secondaryDNS)))
        {
            log("Error: Falló al conectar el modo Cliente");
        }        
    }
    WiFi.hostname(deviceID());
    WiFi.begin(wifi_ssid,wifi_passw);
    log("Info: Conectando WIFI " + String(wifi_ssid));
    delay(50);
    byte b = 0;
    while (WiFi.status() != WL_CONNECTED && b < 60)
    {
        b++;
        log("Warning: Intentando conexión WIFI...");
        delay(500);
        //Parpadear led Wifi cuadno este conectadose al wifi no bloqueante
        //parpadeo simple del led cada 100 ms
        blinkSingle(100,WIFILED);
    }
    if (WiFi.status() == WL_CONNECTED)
    {   //WIFI station conectado
        log("Info: WiFi conectado (" + String(WiFi.RSSI()) + ") IP " + ipStr(WiFi.localIP()));
        //Parpadeo random de led
        blinkRandomSingle(10,100,WIFILED);
        delay(100);
    }else{
        //Wifi Sation no conectado
        log(F("Error: WiFi no conectado"));
        //Parpadeo ramdon del led
        blinkRandomSingle(10,100,WIFILED);
        delay(100);
    }  
}
//WIFI.mode(WIDI_STA) - trabja en modo estacion
//WIFI.mode(WIDI_AP) - trabja en modo access point
//WIFI.mode(WIDI_AP_STA) - trabja de las dos maneras
void wifi_setup(){
    WiFi.disconnect();
    //Si esta activo en modo AP
    if (ap_accesPoint)
    {
        startAP();
        log("Info: WiFi Modo AP");
    }
    //Caso contrario en modo Cliente
    else{
        WiFi.mode(WIFI_STA);
        wifi_mode = WIFI_STA;
        startClient();
        log("Info: WiFIi Modo Cliente");
    }
    //Inciar hostname broadcaster en modo STA o AP
    if (wifi_mode == WIFI_STA || wifi_mode == WIFI_AP)
    {
        if(MDNS.begin(esp_hostname)){
            MDNS.addService("http","tcp",80);
        }
    }  
}
// Loop Modo Cliente
void wifiLoop(){
    unsigned long currentMillis = millis();
    if (WiFi.status() != WL_CONNECTED && (currentMillis - previousMillisWIFI >= intervalWIFI)){
        // para parpadear un led cuando esta conectandose al wifi no bloqueante
        // Parpadeo Simple del Led cada 100 ms
        blinkSingle(100, WIFILED);

        WiFi.disconnect();
        WiFi.reconnect();                 
        previousMillisWIFI = currentMillis;
    }else{
        // parpadeo del led Tiempo On y Tiempo Off
        blinkSingleAsy(10,500,WIFILED);     
    }     
}
// Loop Modo AP
void wifiAPLoop(){
    // Parpadeo del Led con tiempo variable como transferencia de Datos
    blinkRandomSingle(50,100,WIFILED);
    dnsServer.processNextRequest();     // Captive portal DNS re-dierct
}