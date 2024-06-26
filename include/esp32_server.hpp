bool enableCors = true;
// Función de ayuda para realizar autenticación en una solicitud http
bool requestPreProcess(AsyncWebServerRequest *request, AsyncResponseStream *&response, const char *contentType = "application/json"){
  if(!request->authenticate(www_username, www_password)) {
    request->requestAuthentication();
    return false;
  }
  response = request->beginResponseStream(contentType);
  if(enableCors) {
    response->addHeader("Access-Control-Allow-Origin", "*");
  }
  return true;
}
//Cargar pagina Index.html o home url: /
void handleHome(AsyncWebServerRequest *request) {
    AsyncResponseStream *response;
    File file = SPIFFS.open(F("/index.html"),"r");
    if(file){
        file.setTimeout(100);
        String s = file.readString();
        file.close();
        // Actualiza el contenido del html al cargar
        s.replace(F("#id#"), id);
        s.replace(F("#serie#"), device_id);
        /* Bloque Inalámbrico */
        s.replace(F("#wifi_status#"), WiFi.status() == WL_CONNECTED ? F("<span class='label label-success'>CONECTADO</span>") : F("<span class='label label-danger'>DESCONECTADO</span>"));
        s.replace(F("#wifi_ssid#"), WiFi.status() == WL_CONNECTED ? F(wifi_ssid) : F("WiFi No conectado"));
        s.replace(F("#wifi_ip#"), ipStr(WiFi.status() == WL_CONNECTED ? WiFi.localIP() : WiFi.softAPIP()));
        s.replace(F("#wifi_mac#"), String(WiFi.macAddress()));
        s.replace(F("#wifi_dbm#"), WiFi.status() == WL_CONNECTED ? String(WiFi.RSSI()) : F("0"));
        s.replace(F("#wifi_percent#"), WiFi.status() == WL_CONNECTED ? String(getRSSIasQuality(WiFi.RSSI())) : F("0"));
        /* Bloque MQTT */
        s.replace(F("#mqtt_status#"), mqttclient.connected() ? F("<span class='label label-success'>CONECTADO</span>") : F("<span class='label label-danger'>DESCONECTADO</span>"));
        s.replace(F("#mqtt_server#"), mqttclient.connected() ? F(mqtt_server) : F("Server No Conectado"));
        s.replace(F("#mqtt_user#"), F(mqtt_user));
        s.replace(F("#mqtt_id#"), F(mqtt_id));
        /* Bloque Información */
        s.replace(F("#clientIP#"), ipStr(request->client()->remoteIP()));
        s.replace(F("#userAgent#"), request->getHeader("User-Agent")->value().c_str());
        s.replace(F("#firmware#"), currentfirmware);
        s.replace(F("#sdk#"), String(ESP.getSdkVersion()));
        s.replace(F("#hardware#"), F(HW));
        s.replace(F("#cpu_freq#"), String(getCpuFrequencyMhz()));
        s.replace(F("#flash_size#"), String(ESP.getFlashChipSize() / (1024.0 * 1024), 2));
        s.replace(F("#ram_size#"), String(ESP.getHeapSize() / 1024.0, 2));
        s.replace(F("#time_active#"), longTimeStr(millis() / 1000));
        s.replace(F("#bootCount#"), String(bootCount));
        /* Bloque General */
        s.replace(F("#platform#"), platform());
        s.replace(F("#mqtt_on#"), mqttclient.connected() ? F("<span class='label btn-metis-2'>Online</span>") : F("<span class='label label-danger'>Offline</span>"));
        s.replace(F("#temp_cpu#"), String(TempCPUValue()));
        /* Bloque Progressbar */
        s.replace(F("#spiffs_used#"), String(round(SPIFFS.usedBytes() * 100 / SPIFFS.totalBytes()), 0));
        s.replace(F("#ram_available#"), String(ESP.getFreeHeap() * 100 / ESP.getHeapSize()));
        /* Botones Relays */
        s.replace(F("#relay1#"), Relay01_status ? "checked" : "");
        s.replace(F("#relay2#"), Relay02_status ? "checked" : "");
        s.replace(F("#label1#"), Relay01_status ? "label-primary" : "label-info");
        s.replace(F("#label2#"), Relay02_status ? "label-primary" : "label-info");
        s.replace(F("#Statustext1#"), Relay01_status ? "Activo" : "Apagado");
        s.replace(F("#Statustext2#"), Relay02_status ? "Activo" : "Apagado");
        // Send Data
        request->send(200, "text/html", s);
    }else{
        request->send(500, "text/plain","/index.html no funciona, esta seguro que lo cargo a la memoria SPIFFS?");
    }
}
void InitServer(){//Cargar informacion de las paginas al servidor es decir peticiones GET/POST
    //Cargar todos los archivos estaticos del servidor es decir loque no es html
    server.serveStatic("/www/bootstrap-switch.css", SPIFFS, "/www/bootstrap-switch.css").setDefaultFile("www/bootstrap-switch.css").setCacheControl("max-age=600");
    server.serveStatic("/www/bootstrap-switch.min.js", SPIFFS, "/www/bootstrap-switch.min.js").setDefaultFile("www/bootstrap-switch.min.js").setCacheControl("max-age=600");
    server.serveStatic("/www/bootstrap.css", SPIFFS, "/www/bootstrap.css").setDefaultFile("/assets/bootstrap.css").setCacheControl("max-age=600");
    server.serveStatic("/www/bootstrap.min.js", SPIFFS, "/www/bootstrap.min.js").setDefaultFile("www/bootstrap.min.js").setCacheControl("max-age=600");
    server.serveStatic("/www/portada_iot.jpg", SPIFFS, "/www/portada_iot.jpg").setDefaultFile("www/portada_iot.jpg").setCacheControl("max-age=600");
    server.serveStatic("/www/font-awesome.css", SPIFFS, "/www/font-awesome.css").setDefaultFile("www/font-awesome.css").setCacheControl("max-age=600");
    server.serveStatic("/www/fontawesome-webfont.eot", SPIFFS, "/www/fontawesome-webfont.eot").setDefaultFile("www/fontawesome-webfont.eot").setCacheControl("max-age=600");
    server.serveStatic("/www/fontawesome-webfont.woff", SPIFFS, "/www/fontawesome-webfont.woff").setDefaultFile("www/fontawesome-webfont.woff").setCacheControl("max-age=600");
    server.serveStatic("/www/jquery-2.0.3.min.js", SPIFFS, "/www/jquery-2.0.3.min.js").setDefaultFile("www/jquery-2.0.3.min.js").setCacheControl("max-age=600");
    server.serveStatic("/www/layout2.css", SPIFFS, "/www/layout2.css").setDefaultFile("www/layout2.css").setCacheControl("max-age=600");
    server.serveStatic("/www/main.css", SPIFFS, "/www/main.css").setDefaultFile("www/main.css").setCacheControl("max-age=600");
    server.serveStatic("/www/modernizr-2.6.2.min.js", SPIFFS, "/www/modernizr-2.6.2.min.js").setDefaultFile("www/modernizr-2.6.2.min.js").setCacheControl("max-age=600");
    server.serveStatic("/www/MoneAdmin.css", SPIFFS, "/www/MoneAdmin.css").setDefaultFile("www/MoneAdmin.css").setCacheControl("max-age=600");
    server.serveStatic("/www/scripts.js", SPIFFS, "/www/scripts.js").setDefaultFile("www/scripts.js").setCacheControl("max-age=600");
    server.serveStatic("/www/styles.css", SPIFFS, "/www/styles.css").setDefaultFile("www/styles.css").setCacheControl("max-age=600");
    server.serveStatic("/www/theme.css", SPIFFS, "/www/theme.css").setDefaultFile("www/theme.css").setCacheControl("max-age=600");
    server.serveStatic("/www/error.css", SPIFFS, "/www/error.css").setDefaultFile("www/error.css").setCacheControl("max-age=600");
    server.serveStatic("/www/sweetalert2.min.css", SPIFFS, "/www/sweetalert2.min.css").setDefaultFile("www/sweetalert2.min.css").setCacheControl("max-age=600");
    server.serveStatic("/www/sweetalert2.min.js", SPIFFS, "/www/sweetalert2.min.js").setDefaultFile("www/sweetalert2.min.js").setCacheControl("max-age=600");
    //Cargar pagina Index.html o home
    //url: /
    //Metodo: GET
    server.on("/",HTTP_GET,handleHome);
    //Cargar pagina wifi.html
    //url: /esp-wifi
    server.on("/esp-wifi", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        // Cargar página html Configuración del WiFi
        File file = SPIFFS.open(F("/wifi.html"), "r");
        if(file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            // Sección Cliente
            s.replace(F("#platform#"), platform());
            s.replace(F("#wifi_ssid#"), wifi_ssid);
            s.replace(F("#wifi_staticIP#"), wifi_staticIP ? "checked" : "");
            s.replace(F("#wifi_staticIPen#"), wifi_staticIP ? "1" : "0");
            s.replace(F("#wifi_ip_static#"), ipStr(CharToIP(wifi_ip_static)));
            s.replace(F("#wifi_subnet#"), ipStr(StrIP(wifi_subnet)));
            s.replace(F("#wifi_gateway#"), ipStr(StrIP(wifi_gateway)));
            s.replace(F("#wifi_primaryDNS#"), ipStr(StrIP(wifi_primaryDNS)));
            s.replace(F("#wifi_secondaryDNS#"), ipStr(StrIP(wifi_secondaryDNS)));
            // Sección AP
            s.replace(F("#ap_accessPoint#"), ap_accesPoint ? "checked" : "");
            s.replace(F("#ap_AP_en#"), ap_accesPoint ? "1" : "0");
            s.replace(F("#ap_nameap#"), String(ap_nameap));
            s.replace(F("#ap_canalap#"), String(ap_canal));
            s.replace(F("#ap_hiddenap#"), ap_hidden ? "" : "checked");
            s.replace(F("#ap_visibility#"), ap_hidden ? "0" : "1");
            s.replace(F("#ap_connetap#"), String(ap_connet));
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/wifi.html not found, have you flashed the SPIFFS?");
        }
    });
    //Cargar pagina mqtt.html
    //url: /esp-mqtt
    server.on("/esp-mqtt", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        // Cargar página html Configuración del MQTT
        File file = SPIFFS.open(F("/mqtt.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            s.replace(F("#platform#"), platform());
            s.replace(F("#mqtt_enable#"), mqtt_enable ? "checked" : "");
            s.replace(F("#mqtten#"), mqtt_enable ? "1" : "0");
            s.replace(F("#mqtt_id#"), mqtt_id);
            s.replace(F("#mqtt_user#"), mqtt_user);
            s.replace(F("#mqtt_server#"), mqtt_server);
            s.replace(F("#mqtt_port#"), String(mqtt_port));
            s.replace(F("#mqtt_time#"), String(mqtt_time / 60000));
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/mqtt.html not found, have you flashed the SPIFFS?");
        }
    });
    //Cargar pagina servidor.html
    //url: /esp-servidor
    server.on("/esp-servidor", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        // Cargar página html Configuración del MQTT
        File file = SPIFFS.open(F("/servidor.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            s.replace(F("#platform#"), platform());
            s.replace(F("#id#"), id);
            s.replace(F("#serie#"), device_id);
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/servidor.html not found, have you flashed the SPIFFS?");
        }
    });
    // Cargar página restore.html
    // url: /esp-dispositivos
    // Metodo: GET
    server.on("/esp-dispositivos", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        // Cargar página html Configuración del DISPOSITIVOS
        File file = SPIFFS.open(F("/dispositivos.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            s.replace(F("#platform#"), platform());
            s.replace(F("#modelomovil#"), modelomovil);
            s.replace(F("#emeidispomovil#"), emeidispo);
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/dispositivos.html not found, have you flashed the SPIFFS?");
        }
    });
    server.on("/esp-usuario", HTTP_GET, [](AsyncWebServerRequest *request){
       AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        // Cargar página html Configuración del MQTT
        File file = SPIFFS.open(F("/usuario.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            s.replace(F("#platform#"), platform());
            // Send data
            request->send(200, "text/html", s);
        }else{
            request->send(500, "text/plain","/usuario.html not found, have you flashed the SPIFFS?");
        }
    });
    //METODOS POST
    // Guardar la Contraseña y el Usuario
    // url: /esp-usuario
    // Metodo: POST
    server.on("/esp-usuario", HTTP_POST, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        // Validar datos enviados
        if(request->params() == 5){
            String u, p, nu, np, cp; 
            if (request->hasArg("www_username"))
                u = request->arg("www_username");
            if (request->hasArg("www_password"))
                p = request->arg("www_password");
            if (request->hasArg("new_www_username"))
                nu = request->arg("new_www_username");
            if (request->hasArg("new_www_password"))
                np = request->arg("new_www_password");
            if (request->hasArg("c_new_www_password"))
                cp = request->arg("c_new_www_password");
            u.trim();
            p.trim();
            nu.trim();
            np.trim();
            cp.trim();
            // Validar que los datos del usario y contraseña anteriores no esten en blanco
            if(u != "" && p != ""){
                // validar que el usuario y contraseña coincidan con los anteriores
                if(u == www_username && p == www_password){
                    // Guardamos el Nuevo Usuario
                    if(nu != "" && np == "" && cp == ""){
                        // Validar que el usuario nuevo sea diferente al antiguo
                        if(nu == www_username){
                            request->send(200, "text/html", SweetAlert("Usuario", "Warning", "El usuario no puede ser igual al anterior", "warning", "aviso"));
                            return;
                        }
                        strlcpy(www_username, nu.c_str(), sizeof(www_username));
                        // Parpadeo de los LEDS
                        leds();
                        // Guardar el nuevo usuario
                        if(settingsSaveAdmin()){
                            request->send(200, "text/html", SweetAlert("Usuario", "Exito", "Usuario actualizado", "success", "aviso"));
                            return;
                        }else{
                            request->send(200, "text/html", SweetAlert("Usuario", "Error", "Usuario no Actualizado", "error", "aviso"));
                            return;
                        }
                    }
                    // Guardar solo la contraseña nueva 
                    if(np != "" && cp != "" && np == cp && nu == ""){
                        // Validar que el usuario nuevo sea diferente al antiguo
                        if(np == www_password){
                            request->send(200, "text/html", SweetAlert("Contraseña", "Advertencia", "La contraseña no puede ser igual a la anterior", "warning", "aviso"));
                            return;
                        }
                        strlcpy(www_password, np.c_str(), sizeof(www_password));
                        // Leds
                        leds();
                        // Guardar el nuevo usuario
                        if(settingsSaveAdmin()){
                            request->send(200, "text/html", SweetAlert("Contraseña", "Exito", "Contraseña actualizada", "success", "aviso"));
                            return;
                        }else{
                            request->send(200, "text/html", SweetAlert("Contraseña", "Error", "Contraseña no Actualizada", "error", "aviso"));
                            return;
                        } 
                    }else if(np != cp){
                        request->send(200, "text/html", SweetAlert("Contraseña", "Advertencia", "La contraseña Anterior y Nueva no coinciden", "warning", "aviso"));
                        return;
                    }
                    // Guardo Usuario y Contraseña
                    if(nu != "" && np != "" && cp != "" && np == cp){
                        // Validar que el usuario nuevo y la contraseña nueva sea diferente al antiguos
                        if(np == www_password && nu == www_username){
                            request->send(200, "text/html", SweetAlert("Usuario & Contraseña", "Advertencia", "La contraseña y el usuario no puede ser iguales a los anteriores", "warning", "aviso"));
                            return;
                        }
                        strlcpy(www_username, nu.c_str(), sizeof(www_username));
                        strlcpy(www_password, np.c_str(), sizeof(www_password));
                        // Leds
                        leds();
                        // Guardar el nuevo usuario
                        if(settingsSaveAdmin()){
                            request->send(200, "text/html", SweetAlert("Usuario & Contraseña", "Exito", "Usuario & Contraseña actualizados", "success", "aviso"));
                            return;
                        }else{
                            request->send(200, "text/html", SweetAlert("Usuario & Contraseña", "Error", "Usuario & Contraseña no Actualizados", "error", "aviso"));
                            return;
                        }
                    }
                }else{
                    request->send(200, "text/html", SweetAlert("Información", "Información", "No se pudo Guardar, usuario y contraseña anterior no conciden", "info", "aviso"));
                }
            }else{
                request->send(200, "text/html", SweetAlert("Información", "Información", "No se permiten Usuario/Contraseña Actuales en blanco", "info", "aviso"));
            }            
        }else{
            request->send(200, "text/html", SweetAlert("Error", "Error", "Error de Parámetros", "error", "aviso"));
        }
    });
    // Guardar el ID del servidor
    // url: /esp-device
    // Metodo: POST
    server.on("/esp-servidor", HTTP_POST, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        if (request->params() == 1){
            String s;
            if(request->hasArg("id"))
            s = request->arg("id");
            s.trim();
            if(s != "" && s != id){
                strlcpy(id, s.c_str(), sizeof(id));  
                // leds
                leds();
                // Guardamos la informacion
                if(configuracionSaveWiFi()){
                    request->send(200, "text/html", SweetAlert("Identificador", "Exito", "El Identificador se Guardó correctamente", "success", "aviso"));
                    return;
                }else{
                    request->send(200, "text/html", SweetAlert("Identificador", "Error", "El Identificador no se pudo Guardar", "error", "aviso")); 
                    return;
                }              
            }else{
               request->send(200, "text/html", SweetAlert("Warning", "Advertencia", "El Identificador no puede estar en Blanco ni ser igual al anterior", "warning", "aviso"));               
            }            
        }else{
            request->send(200, "text/html", SweetAlert("Error", "Error", "Error de Parámetros", "error", "aviso"));            
        }
    });
    // Guardar la configuración del MQTT
    // url: /esp-mqtt
    // Metodo: POST
    server.on("/esp-mqtt", HTTP_POST, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        if(request->hasArg("mqtten"))
        mqtt_enable = request->arg("mqtten").toInt();
        if(mqtt_enable == 1){
            if (request->params() == 7){
                String s;
                // ID MQTT Broker
                if(request->hasArg("mqtt_id"))
                s = request->arg("mqtt_id");
                s.trim();
                if (s != ""){
                    strlcpy(mqtt_id, s.c_str(), sizeof(mqtt_id));
                }
                // Usuario MQTT Broker
                if(request->hasArg("mqtt_user"))
                s = request->arg("mqtt_user");
                s.trim();
                if (s != ""){
                    strlcpy(mqtt_user, s.c_str(), sizeof(mqtt_user));
                }
                // Contraseña MQTT Broker
                if(request->hasArg("mqtt_passw"))
                s = request->arg("mqtt_passw");
                s.trim();
                if (s != ""){
                    strlcpy(mqtt_passw, s.c_str(), sizeof(mqtt_passw));
                }
                // Servidor MQTT
                if(request->hasArg("mqtt_server"))
                s = request->arg("mqtt_server");
                s.trim();
                if (s != ""){
                    strlcpy(mqtt_server, s.c_str(), sizeof(mqtt_server));
                }
                // Puerto Servidor MQTT
                if(request->hasArg("mqtt_port"))
                mqtt_port = request->arg("mqtt_port").toInt();
                // Publish cada (min)
                if(request->hasArg("mqtt_time"))
                mqtt_time = request->arg("mqtt_time").toInt()*60000;
                // leds
                leds();
                // Guardar la configuracion del mqtt
                if(settingsSaveMQTT()){
                    request->send(200, "text/html", SweetAlert("MQTT", "Exito", "La configuración del MQTT se Guardó correctamente", "success", "accion"));
                    return;
                }else{
                    request->send(200, "text/html", SweetAlert("MQTT", "Error", "La configuración del MQTT no se pudo Guardar", "error", "aviso")); 
                    return;
                }
            }else{
                request->send(200, "text/html", SweetAlert("Error", "Error", "Error de Parámetros", "error", "aviso"));
            }
        }else{
            // leds
            leds();
            // Guardar la configuracion del mqtt
            if(settingsSaveMQTT()){
                request->send(200, "text/html", SweetAlert("MQTT", "Exito", "La configuración del MQTT se Guardó correctamente", "success", "accion"));
                return;
            }else{
                request->send(200, "text/html", SweetAlert("MQTT", "Error", "La configuración del MQTT no se pudo Guardar", "error", "aviso")); 
                return;
            }
        }
    });
    // Guardar la configuración del WIFI
    // url: /esp-wifi
    // Metodo: POST
    server.on("/esp-wifi", HTTP_POST, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        // IP Estática
        if(request->hasArg("wifi_staticIPen"))
        wifi_staticIP = request->arg("wifi_staticIPen").toInt();
        // AP Habilitado
        if(request->hasArg("ap_AP_en"))
        ap_accesPoint = request->arg("ap_AP_en").toInt();
        
        String s; 
        // ssid Wifi
        if(request->hasArg("wifi_ssid"))
        s = request->arg("wifi_ssid");
        s.trim();
        if (s != ""){
            strlcpy(wifi_ssid, s.c_str(), sizeof(wifi_ssid));
        }
        s="";
        // password wifi
        if(request->hasArg("wifi_passw"))
        s = request->arg("wifi_passw");
        s.trim();
        if (s != ""){
            strlcpy(wifi_passw, s.c_str(), sizeof(wifi_passw));
        }
        s="";
        if(wifi_staticIP == 1){
            // Ip estatico
            if(request->hasArg("wifi_ip_static"))
            s = request->arg("wifi_ip_static");
            s.trim();
            if (s != ""){
                strlcpy(wifi_ip_static, s.c_str(), sizeof(wifi_ip_static));
            }
            s="";
            // Mascara de subred
            if(request->hasArg("wifi_subnet"))
            s = request->arg("wifi_subnet");
            s.trim();
            if (s != ""){
                strlcpy(wifi_subnet, s.c_str(), sizeof(wifi_subnet));
            }
            s="";
            // Gateway
            if(request->hasArg("wifi_gateway"))
            s = request->arg("wifi_gateway");
            s.trim();
            if (s != ""){
                strlcpy(wifi_gateway, s.c_str(), sizeof(wifi_gateway));
            }
            s="";
            // Primary DNS
            if(request->hasArg("wifi_primaryDNS"))
            s = request->arg("wifi_primaryDNS");
            s.trim();
            if (s != ""){
                strlcpy(wifi_primaryDNS, s.c_str(), sizeof(wifi_primaryDNS));
            }
            s="";
            // Secondary DNS
            if(request->hasArg("wifi_secondaryDNS"))
            s = request->arg("wifi_secondaryDNS");
            s.trim();
            if (s != ""){
                strlcpy(wifi_secondaryDNS, s.c_str(), sizeof(wifi_secondaryDNS));
            }
            s="";
        }else if (ap_accesPoint == 1){
            // Nombre del AP
            if(request->hasArg("ap_nameap"))
            s = request->arg("ap_nameap");
            s.trim();
            if (s != ""){
                strlcpy(ap_nameap, s.c_str(), sizeof(ap_nameap));
            }
            s="";
            // password del AP
            if(request->hasArg("ap_passwordap"))
            s = request->arg("ap_passwordap");
            s.trim();
            if (s != ""){
                strlcpy(ap_passw, s.c_str(), sizeof(ap_passw));
            }
            // AP visibility ON 0 - OFF 1
            if(request->hasArg("ap_visibility"))
            ap_hidden = request->arg("ap_visibility").toInt();
            // Canal del AP - 1- 13
            if(request->hasArg("ap_canalap"))
            ap_canal = request->arg("ap_canalap").toInt();    
            // Conexiones del AP - 0- 8
            if(request->hasArg("ap_connetap"))
            ap_connet = request->arg("ap_connetap").toInt();
        }
        // Parpadea los LED
        leds();
        // Guarda configuracion del WiFi
        if (configuracionSaveWiFi()){
            request->send(200, "text/html", SweetAlert("Inalámbrico", "Exito", "La configuración Inálambrica se Guardó correctamente", "success", "accion"));
            return;
        }else{
            request->send(200, "text/html", SweetAlert("Inalámbrico", "Error", "La configuración Inálambrica no se pudo Guardar", "error", "aviso")); 
            return;
        }
    });
    // Error 404 página no encontrada
    // url: "desconocido"
    server.onNotFound([](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        log("Info: Página NOT_FOUND: ");
        if(request->method() == HTTP_GET) {
            log("Info: Método -> GET");
        } else if(request->method() == HTTP_POST) {
            log("Info: Método -> POST");
        } else if(request->method() == HTTP_DELETE) {
            log("Info: Método -> DELETE");
        } else if(request->method() == HTTP_PUT) {
            log("Info: Método -> PUT");
        } else if(request->method() == HTTP_PATCH) {
            log("Info: Método -> PATCH");
        } else if(request->method() == HTTP_HEAD) {
            log("Info: Método -> HEAD");
        } else if(request->method() == HTTP_OPTIONS) {
            log("Info: Método -> OPTIONS");
        } else {
            log("Info: Método -> UNKNOWN");
        }
        log("Info: http://"+ request->host()+ request->url());
        if(request->contentLength()){
            log("Info: _CONTENT_TYPE: "+ request->contentType());
            log("Info: _CONTENT_LENGTH: "+ request->contentLength());
        }

        int headers = request->headers();
        int i;
        for(i=0; i<headers; i++) {
            AsyncWebHeader* h = request->getHeader(i);
            log("Info: _HEADER[]: "+ h->name()+ h->value());
        }

        int params = request->params();
        for(i = 0; i < params; i++) {
            AsyncWebParameter* p = request->getParam(i);
            if(p->isFile()){
                log("Info: _FILE[]: , size: " + p->name() + p->value() + p->size());
            } else if(p->isPost()){
                log("Info: _POST[]: " + p->name() + p->value());
            } else {
                log("Info: _GET[]: " + p->name() + p->value());
            }
        }
        //Scanear Wifi
    server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        String json = "";
        int n = WiFi.scanComplete();
        if(n == -2){
        json = "{";
        json += "\"meta\": { \"serial\": \""+ device_id +"\", \"count\": 0},";
        json += "\"data\": [";
        json += "],";   
        json += "\"code\": 0 ";
        json += "}";
        WiFi.scanNetworks(true, true); 
        } else if(n){
            json = "{";
            json += "\"meta\": { \"serial\": \""+ device_id +"\", \"count\":"+String(n)+"},";
            json += "\"data\": [";
            for (int i = 0; i < n; ++i){
                if(i) json += ",";
                json += "{";
                json += "\"n\":"+String(i+1);
                json += ",\"rssi\":"+String(WiFi.RSSI(i));
                json += ",\"ssid\":\""+WiFi.SSID(i)+"\"";
                json += ",\"bssid\":\""+WiFi.BSSIDstr(i)+"\"";
                json += ",\"channel\":"+String(WiFi.channel(i));
                json += ",\"secure\":\""+ EncryptionType(WiFi.encryptionType(i))+"\"";
                json += "}";
            }
            json += "],";   
            json += "\"code\": 1 ";
            json += "}";
            WiFi.scanDelete();
            if(WiFi.scanComplete() == -2){
                WiFi.scanNetworks(true, true);
            }
        }
        response->addHeader("Server","ESP32 Admin Tools");
        request->send(200, "application/json", json);
        json = String();

    });
        // Error 404 página no encontrada
        File file = SPIFFS.open(F("/error_404.html"), "r");
        if (file){
            file.setTimeout(100);
            String s = file.readString();
            file.close();
            // Send data
            request->send(404, "text/html", s);
        }
    });
    // Petición de salir del Servidor
    // url: "esp-logout"
    // Método: GET
    server.on("/esp-logout", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncResponseStream *response;
        if(false == requestPreProcess(request, response)) {
            return;
        }
        request->send(401, "text/html", SweetAlert("LogOut", "Salir", "Logout", "success", "logout"));
    });

    //Iniciar el servidor
    server.begin();
    log("Info: Servidor HTTP iniciando");
}
