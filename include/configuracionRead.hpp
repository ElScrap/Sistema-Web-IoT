// Leer comentarios WIFI
boolean configuracionReadWIFI()
{
    StaticJsonDocument<500> jsonConfig;
    File file = SPIFFS.open("/ConfigWIFI.json", "r");
    if (deserializeJson(jsonConfig, file))
    {
        ConfigResetWIFI();
        log("Error: Falló la lectura d ela configuración WIFI, tomando valores por defecto");
        return false;
    }else{
         /* ------------------- GENERAL -------------------- */
        strlcpy(id, jsonConfig["id"], sizeof(id)); 
        bootCount = jsonConfig["boot"];
        /* ------------------- CLIENTE -------------------- */
        wifi_staticIP = jsonConfig["wifi_staticIP"];
        strlcpy(wifi_ssid, jsonConfig["wifi_ssid"], sizeof(wifi_ssid));
        strlcpy(wifi_passw, jsonConfig["wifi_passw"], sizeof(wifi_passw));        
        strlcpy(wifi_ip_static, jsonConfig["wifi_ip_static"], sizeof(wifi_ip_static));
        strlcpy(wifi_gateway, jsonConfig["wifi_gateway"], sizeof(wifi_gateway));
        strlcpy(wifi_subnet, jsonConfig["wifi_subnet"], sizeof(wifi_subnet));
        strlcpy(wifi_primaryDNS, jsonConfig["wifi_primaryDNS"], sizeof(wifi_primaryDNS));
        strlcpy(wifi_secondaryDNS, jsonConfig["wifi_secondaryDNS"], sizeof(wifi_secondaryDNS));
        /* ------------------- AP ------------------------ */
        ap_accesPoint = jsonConfig["ap_accessPoint"];
        strlcpy(ap_nameap, jsonConfig["ap_nameap"], sizeof(ap_nameap));
        strlcpy(ap_passw, jsonConfig["ap_passwordap"], sizeof(ap_passw));
        ap_canal = jsonConfig["ap_canalap"];
        ap_hidden = jsonConfig["ap_hiddenap"];
        ap_connet = jsonConfig["ap_connetap"];
        file.close();
        log("Info: Lectura configuración WiFi correcta");
        return true;
    }
    
}