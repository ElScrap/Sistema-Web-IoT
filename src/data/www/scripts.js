let pathname = window.location.pathname;/* capturar el url del navegador  http://server[/pathname] */
var connection = new WebSocket('ws://' + location.hostname + '/ws', ['arduino']);
//var connection = new WebSocket('ws://192.168.0.109/ws', ['arduino']);
/* Conexion Abierta */
let listaDispositivos = [];
let listaDispositivos2 = [];
let elementoscreados = false;
let vareliminar = 1;
let nuevoDispositivo = {};
var nuevolo;
let varcoordenadas;
var mensajecoordenadas;  
// Declaración de listaDispositivos fuera del bloque condicional
connection.onopen = function () {
    console.log('Conexión al WebSocket abierta');
};
/* Conexión Error */
connection.onerror = function (error) {
    console.log('Error en el WebSocket', error);
};
/* Manejo de mensajes entrantes por WS */
connection.onmessage = function (e) {
    /* Precesar mensajes*/
    processData(e.data);

};
/* Conexión Cerrada */
connection.onclose = function () {
    console.log('Conexión al WebSocket cerrada');
};
function processData(data) {
    let json = JSON.parse(data);
    console.log("Salida :" + pathname);
    //Mensajes de retorno en un SweetAlert si retorna un status válido
    if (json.type == "info") {
        Swal.fire({
            position: "top-end",
            icon: json.icon,
            title: json.texto,
            showConfirmButton: false,
            timer: 2000
        });
    }
    if (pathname == "/") {
        if (json.type == "data") {
            // console.log(json);
            document.getElementById('wifi_status').innerHTML = json.wifi_status; // Estado del WIFI
            document.getElementById('mqtt_status').innerHTML = json.mqtt_status; // Estado del MQTT
            document.getElementById('mqtt_on').innerHTML = json.mqtt_on; // Estado del MQTT
            document.getElementById('mqtt_server').innerHTML = json.mqtt_server; // Broker MQTT
            document.getElementById('wifi_dbm').innerHTML = json.wifi_dbm + " dBm"; // dbm Señal WIFI
            document.getElementById('dbm_label').innerHTML = json.wifi_dbm + " dBm"; // dbm Señal WIFI
            document.getElementById('wifi_percent').innerHTML = json.wifi_percent + " %"; // % de la señal WIFI

            // preogressbar
            document.getElementById('wifi_Signal').style.width = json.wifi_percent + "%";
            document.getElementById('wifi_label').innerHTML = json.wifi_percent + "%";
            document.getElementById('spiffs_Signal').style.width = json.spiffs_used + "%";
            document.getElementById('spiffs_label').innerHTML = json.spiffs_used + "%";
            document.getElementById('ram_Signal').style.width = json.ram_available + "%";
            document.getElementById('ram_label').innerHTML = json.ram_available + "%";
            document.getElementById('temp_cpu').innerHTML = json.temp_cpu + " °C"; // Muestra Temperatura del CPU
            document.getElementById('active').innerHTML = json.active; // Tiempo Activo del Dispositivo
            document.getElementById('time_active').innerHTML = json.active; // Tiempo Activo del Dispositivo
            // Capturar los estados de los relays en tiempo real
            let RELAY1_Status = document.getElementById('RELAY1_Status');
            let RELAY2_Status = document.getElementById('RELAY2_Status');
            let divRELAY1 = document.getElementById('RELAY1').parentNode;
            let divRELAY2 = document.getElementById('RELAY2').parentNode;
            if (json.relay1_status) {
                divRELAY1.classList.remove('switch-off');
                divRELAY1.classList.add('switch-on');
                RELAY1_Status.innerText = 'Activo';
                RELAY1_Status.classList.remove('label-info');
                RELAY1_Status.classList.add('label-success');
            } else {
                divRELAY1.classList.remove('switch-on');
                divRELAY1.classList.add('switch-off');
                RELAY1_Status.innerText = 'Apagado';
                RELAY1_Status.classList.remove('label-success');
                RELAY1_Status.classList.add('label-info');
            }
            if (json.relay2_status) {
                divRELAY2.classList.remove('switch-off');
                divRELAY2.classList.add('switch-on');
                RELAY2_Status.innerText = 'Activo';
                RELAY2_Status.classList.remove('label-info');
                RELAY2_Status.classList.add('label-primary');
            } else {
                divRELAY2.classList.remove('switch-on');
                divRELAY2.classList.add('switch-off');
                RELAY2_Status.innerText = 'Apagado';
                RELAY2_Status.classList.remove('label-primary');
                RELAY2_Status.classList.add('label-info');
            }
        }
    }
    // Verificar si el JSON es de tipo "data"
    if (json.type == "data") {
        // Verificar la ruta del pathname
        if (pathname == "/esp-dispositivos") {
            // Crear un objeto literal para representar cada dispositivo
            nuevoDispositivo = { modelo: json.modelomovil, emei: json.emeidispomovil, coordenadas: json.mcoordenadas, estadom: json.mestado};
            if (vareliminar != nuevoDispositivo.emei) {
                if (nuevoDispositivo && nuevoDispositivo.modelo.trim() !== "" && nuevoDispositivo.emei != null && nuevoDispositivo.emei.trim() !== "" && nuevoDispositivo.modelo != null) {
                    // Verificar si el nuevo dispositivo ya existe en la lista
                    let emeiExistente = listaDispositivos.find(dispositivo => dispositivo.emei === nuevoDispositivo.emei);
                    if (emeiExistente) {
                        emeiExistente.coordenadas = nuevoDispositivo.coordenadas;
                        emeiExistente.estadom = nuevoDispositivo.estadom;

                        let nuevoDispositivo2 = { modelo: nuevoDispositivo.modelo, emei: nuevoDispositivo.emei, coordenadas: nuevoDispositivo.coordenadas, estado: nuevoDispositivo.estadom};
                        listaDispositivos2.push(nuevoDispositivo2);

                        listaDispositivos2.forEach(dispositivo => {
                            console.log("DENTRO ESTADO: " + dispositivo.estado);
                        });
                        guardarListaDispositivos();
                    } else {
                        if (nuevoDispositivo.emei != "null") {
                            // Agregar el nuevo dispositivo a la lista
                            listaDispositivos.push(nuevoDispositivo);
                            // Mostrar datos en elementos HTML
                            console.log("Nuevo dispositivo agregado");
                            // Crear los elementos HTML para todos los dispositivos en la lista
                            crearElementosDispositivos();
                            listaDispositivos.forEach(dispositivo => {
                                console.log("DENTRO M: " + dispositivo.modelo + " E: " + dispositivo.emei + " C: " + dispositivo.coordenadas
                                + dispositivo.estado);
                            });
                            // Llamar a la función para cargar la lista cuando se cargue la página
                            document.addEventListener('DOMContentLoaded', cargarListaDispositivos);
                        }
                    }
                }
            }

        }
    }
}
// Cuando se crea un nuevo dispositivo, guardar la lista en LocalStorage
function guardarListaDispositivos() {
    localStorage.setItem('listaDispositivos', JSON.stringify(listaDispositivos));
}
// Cuando se carga la página, recuperar la lista del LocalStorage (si existe)
function cargarListaDispositivos() {
    const listaGuardada = localStorage.getItem('listaDispositivos');
    if (listaGuardada) {
        listaDispositivos = JSON.parse(listaGuardada);
        crearElementosDispositivos(); // Reconstruir la lista de dispositivos en la página
    }
}
function crearElementosDispositivos() {
    const container1 = document.getElementById("container1");
    // obtener todos los dispositivos de la lista
    const valoresInputs={};
    container1.querySelectorAll('.inputnombreestudiante').forEach(input=>{
        valoresInputs[input.parentElement.parentElement.querySelector('#lblcodigo').textContent]=input.value;
    });
    container1.innerHTML = "";
    listaDispositivos.forEach(dispositivo => {
        const divItem = document.createElement("div");
        divItem.className = "info-product";
        divItem.style.textAlign = "center";
        divItem.innerHTML = `
    <h2>Dispositivo <button class="eliminar-dispositivo" data-emei="${dispositivo.emei}">X</button></h2>
    <hr>
    <label>Estado: <span class="spanestado"></span></label>
    <label>Modelo:</label>
    <label>${dispositivo.modelo}</label><br>
    <label>Codigo:</label>
    <label  id="lblcodigo">${dispositivo.emei}</label><br>
    <button onclick="abrirMapa('${dispositivo.emei}')"><i class="icon-map-marker"></i></button>
    <button onclick="tiempodispositivo('${dispositivo.emei}','${dispositivo.modelo}','Apagar')"><i class="icon-off"></i></button>
    <label>Nombre del Estudiante:<input class="inputnombreestudiante"></input></label><br>
    <label>Tiempo/Mensaje:<input class="inputtiempo"></input></label><br>
    <button class="asignar-tiempo" data-emei="${dispositivo.emei}" data-modelo="${dispositivo.modelo}">Enviar</button>`;
        const asignarTiempoButtons = divItem.querySelectorAll('.asignar-tiempo');
        for (let button of asignarTiempoButtons) {
            button.addEventListener('click', function (event) {
                const inputTiempoElement = button.parentElement.querySelector('.inputtiempo');
                if (inputTiempoElement) {
                    const inputTiempo = inputTiempoElement.value;
                    const emei = button.getAttribute('data-emei');
                    const modelo = button.getAttribute('data-modelo');
                    tiempodispositivo(emei, modelo, inputTiempo);
                } else {
                    console.error("No se encontró ningún input de tiempo asociado al botón.");
                }
            });
        }
        const eliminarDispositivoB = divItem.querySelectorAll('.eliminar-dispositivo');
        for (let button of eliminarDispositivoB) {
            button.addEventListener('click', function (event) {
                const emei = button.getAttribute('data-emei');
                eliminarDispositivo(emei);
            });
        }
        container1.appendChild(divItem);
        function actualizarEstadoyColor(){
            const estado = divItem.querySelector('.spanestado');
            var estadovar=dispositivo.estadom;
            if (estadovar=="Estado_Activado") {
                estado.textContent="Activado";
                divItem.style.backgroundColor = 'green';
            }else if(estadovar=="Estado_Enuso"){
                estado.textContent="Finalizando";
                divItem.style.backgroundColor = 'yellow';
            }else if(estadovar=="Estado_Inactivo"){
                estado.textContent="Inactivo";
                divItem.style.backgroundColor = '';
            }
        }
        actualizarEstadoyColor();
        setInterval(actualizarEstadoyColor,2000);
    });
} 
function abrirMapa(emeis) {
    listaDispositivos2.forEach(dispositvo => {
        if (dispositvo.emei == emeis) {
            mensajecoordenadas = dispositvo.coordenadas;
        }
    });
    var urlMapa = 'https://www.google.com/maps?q=' + mensajecoordenadas;
    window.open(urlMapa, '_blank', 'width=600,height=400');
}
function eliminarDispositivo(emei) {
    vareliminar = emei;
    var datoEliminado = listaDispositivos.findIndex(function (dispositivo) {
        return dispositivo.emei == emei;
    });
    console.log("A ELIMINAR: " + datoEliminado);
    if (datoEliminado >= 0) {
        vareliminar = emei;
        listaDispositivos.splice(datoEliminado, 1);
        $('#container1').empty();
        elementoscreados = true;
        localStorage.removeItem(datoEliminado);
        nuevolo = "null";
        console.log("Dispositivo Eliminado" + localStorage.length);
        if (localStorage.length == 1) {
            localStorage.clear();
        }
    } else {
        console.log("No se encuentra el dispositivo");
    }
    guardarListaDispositivos();
    crearElementosDispositivos();
}
function envioTodosDispositivos() {
    const inputTiempo = document.getElementById('inputtodosDispositivos').value;
    tiempodispositivoglobal(inputTiempo);
}
//evento para asignar tiempo
let tiempodispositivo = (emei, modelo, dato) => {
    var parametros = {
        jemei: emei,
        jmodelo: modelo,
        jmensaje: dato
    };
    connection.send(JSON.stringify(parametros));
}//evento para asignar tiempo glbal
let tiempodispositivoglobal = (dato) => {
    var parametros = {
        jmensajeGlobal: dato
    };
    connection.send(JSON.stringify(parametros));
}
/* habilitar o deshabilitar los input a la hora de la carga del html */
function mqtt() {
    var x = document.getElementById("mqtten");
    if (x.value == "1") {
        formDisableFalse("mqtt");
    } else {
        formDisableTrue("mqtt");
    }
}
function crearReporte(){
    var docente =document.getElementById("inputDocente").value;
    var materia =document.getElementById("inputMateria").value;
    var semestre =document.getElementById("inputSemestre").value;
    var fecha =document.getElementById("fecha").value;
 var data = [['Docente', 'Materia', 'Semestre', 'Fecha', 'Dispositivo', 'Estudiante']];
  // Obtener todos los dispositivos del contenedor
  const dispositivos = document.querySelectorAll(".info-product");
  dispositivos.forEach(dispositivo => {
    const nombreEstudiante = dispositivo.querySelector('.inputnombreestudiante').value;
    const codigo=dispositivo.querySelector('#lblcodigo').textContent;
    data.push([docente, materia, semestre,fecha,codigo,nombreEstudiante]);
  });
  // Crear una nueva hoja de cálculo y agregar los datos
  var wb = XLSX.utils.book_new();
  var ws = XLSX.utils.aoa_to_sheet(data);
  XLSX.utils.book_append_sheet(wb, ws, "Datos");
  // Guardar el libro de trabajo como archivo XLSX
  XLSX.writeFile(wb, 'datos.xlsx');
};
/* Habilitar input WIFI/AP segun estado */
function wifi() {
    var ip = document.getElementById("wifi_staticIPen");
    var ap = document.getElementById("ap_AP_en");
    if (ip.value == "1") {
        formDisableFalse("ip");
    } else {
        formDisableTrue("ip");
    }
    if (ap.value == "1") {
        formDisableFalse("ap");
    } else {
        formDisableTrue("ap");
    }
}
/* ------------------- Habilitar MQTT -------------------- */
function mqttenable() {
    if (document.getElementById("mqtt_enable").checked) {
        document.getElementById("mqtten").value = 1;
        formDisableFalse("mqtt");
    } else {
        document.getElementById("mqtten").value = 0;
        formDisableTrue("mqtt");
    }
}
//Habilitar IP Estática ------------------ */
function staticIPen() {
    if (document.getElementById("wifi_staticIP").checked) {
        document.getElementById('wifi_staticIPen').value = 1;
        formDisableFalse("ip");
    } else {
        document.getElementById('wifi_staticIPen').value = 0;
        formDisableTrue("ip");
    }
}
/* ------------------- Habilitar AP -------------------- */
function apenable() {
    if (document.getElementById("ap_accessPoint").checked) {
        document.getElementById('ap_AP_en').value = 1;
        formDisableFalse("ap");
    } else {
        document.getElementById('ap_AP_en').value = 0;
        formDisableTrue("ap");
    }
}
/* -------- Habilitar AP Visible (0) No (1) ------------ */
function visibilityen() {
    if (document.getElementById("ap_hiddenap").checked) {
        document.getElementById('ap_visibility').value = 0;
    } else {
        document.getElementById('ap_visibility').value = 1;
    }
}
/* Bloquear los input de mqtt */
function formDisableFalse(clase) {
    var formElement = document.getElementsByClassName(clase);
    for (var i = 0; i < formElement.length; i++)
        formElement[i].disabled = false;
}
/* Habilitar los input de mqtt */
function formDisableTrue(clase) {
    var formElement = document.getElementsByClassName(clase);
    for (var i = 0; i < formElement.length; i++)
        formElement[i].disabled = true;
}
/* Limpiar Formulario */
function clearAdmin() {
    localStorage.clear();
    document.getElementById("formulario").reset();
}
//Evento para encender le relay
let relay = (e) => {
    let miCheckbox = document.getElementById(e.id);
    let msg = document.getElementById(e.id + "_Status");
    if (miCheckbox.checked) {
        msg.innerText = "Activo";
        msg.classList.remove("label-info");
        msg.classList.add("label-success");
        OnOffRelay(e.id, true);
    } else {
        msg.innerText = "Apagado";
        msg.classList.remove("label-success");
        msg.classList.add("label-info");
        OnOffRelay(e.id, false);
    }
}
//Funcion para enviar el JSON por ws
function OnOffRelay(relay, comando) {
    // Ejemplo: {"protocol": "WS", "output": "RELAY1", "value": true }
    connection.send(`{"protocol": "WS", "output": "${relay}", "value": ${comando} }`);
}
/* ------------ Función para restablecer el dispositivo ------ */
function restablecer() {
    Swal.fire({
        title: 'Restablecer!',
        text: ' Toda configuración que haya guardado se perdera si restablece a los valores de fábrica. ¿Está seguro de continuar?',
        icon: 'warning',
        showCancelButton: true,
        confirmButtonColor: '#3085d6',
        cancelButtonColor: '#d33',
        confirmButtonText: 'Si, restablecer',
        cancelButtonText: 'Cancelar',
        reverseButtons: true
    }).then((result) => {
        if (result.isConfirmed) {
            RestoreRestart("restore");
        } else if (
            result.dismiss === Swal.DismissReason.cancel
        ) {
            history.back();
        }
    })
}
/* ------------ Función para Reiniciar el Dispositivo ------ */
function reiniciar() {
    Swal.fire({
        title: 'Reiniciar!',
        text: ' ¿Está seguro de reiniciar el Dispositivo?',
        icon: 'warning',
        showCancelButton: true,
        confirmButtonColor: '#3085d6',
        cancelButtonColor: '#d33',
        confirmButtonText: 'Si, reiniciar',
        cancelButtonText: 'Cancelar',
        reverseButtons: true
    }).then((result) => {
        if (result.isConfirmed) {
            RestoreRestart("restart");
        } else if (
            result.dismiss === Swal.DismissReason.cancel
        ) {
            history.back();
        }
    })
}
/* conteo de reinicio 10s */
let t = 10;
/* Ocultar Visualizador de Progress Bar */
function visualizar() {
    var x = document.getElementById("visualizar");
    x.style.display = "none";
}
function RestoreRestart(type) {
    t--;
    document.getElementById('visualizar').style.display = 'block';
    document.getElementById("time").innerHTML = " " + t;
    document.getElementById('progress').style.width = t * 10 + '%';
    document.getElementById("pregressvalue").innerHTML = " " + t * 10 + '%';
    if (type == "restart") {
        if (t == 0) {
            document.getElementById('visualizar').style.display = 'none';
            document.getElementById('btn').disabled = false;
            window.location = "/";
            t = 10;
        } else if (t == 9) {
            connection.send("restart");
            document.getElementById('btn').disabled = true;
            window.setTimeout("RestoreRestart('restart')", 1000);
        } else {
            document.getElementById('btn').disabled = true;
            window.setTimeout("RestoreRestart('restart')", 1000);
        }
    } else {
        if (t == 0) {
            document.getElementById('visualizar').style.display = 'none';
            document.getElementById('btn').disabled = false;
            window.location = "/";
            t = 10;
        } else if (t == 9) {
            connection.send("restore");
            document.getElementById('btn').disabled = true;
            window.setTimeout("RestoreRestart('restore')", 1000);
        } else {
            document.getElementById('btn').disabled = true;
            window.setTimeout("RestoreRestart('restore')", 1000);
        }
    }
}
// Mansaje para confirmar el Guardado con el Evento Submit
function SweetAlert(title, text, icon, e) {
    Swal.fire({
        title: title,
        text: text,
        icon: icon,
        showCancelButton: true,
        confirmButtonColor: '#3085d6',
        cancelButtonColor: '#d33',
        confirmButtonText: 'Si, guardar',
        cancelButtonText: 'Cancelar',
        reverseButtons: true
    }).then((result) => {
        if (result.isConfirmed) {
            e.submit();
        } else if (
            result.dismiss === Swal.DismissReason.cancel
        ) {
            history.back();
        }
    })
}
// Mansaje de Error al Validar Input
let mensajeFormError = () => {
    Swal.fire({
        position: 'top-end',
        icon: 'error',
        title: '¡No se puede Guardar!',
        showConfirmButton: false,
        timer: 2000
    });
};
// Escaneo de redes
let escanear = () => {
    const url = 'http://' + location.hostname + '/scan';
    fetch(url)
        .then(respuesta => respuesta.json())
        .then(data => {
            if (pathname == "/esp-wifi") {
                // capturamos el id de redes
                let redes = document.getElementById('redes');
                //limpiamos redes
                redes.innerHTML = "";
                // mostrar json por consola
                console.log(data);
                // Mensaje de retorno Respuesta del Scan de redes WIFI
                if (data.code == 1) {
                    // Count > 0
                    //capturamos el id del Body para dibujar la tabla
                    let mitabla = document.getElementById('mi_tabla');
                    //limpiamos tabla
                    mitabla.innerHTML = "";
                    // Agregamos cantidad de redes
                    redes.innerHTML = data.meta.count;
                    // Recorremos toda la lonjitud del data key DATA
                    for (let valor of data.data) {
                        if (parseInt(valor.rssi) >= -67) {
                            mitabla.innerHTML += `  <tr class="success">
                                                            <td align="center">${valor.n}</td>
                                                            <td>${valor.ssid}</td>
                                                            <td align="center">${valor.rssi}</td>
                                                            <td>${valor.bssid}/<br>${valor.secure}</td>
                                                            <td align="center">${valor.channel}</td>                  
                                                            <td><button class="btn btn-primary" onclick="addWiFiInput();">Conectar</button></td>
                                                        </tr>`;
                        } else if (parseInt(valor.rssi) <= -67 && parseInt(valor.rssi) > -80) {
                            mitabla.innerHTML += `<tr class="info">
                                                            <td align="center">${valor.n}</td>
                                                            <td>${valor.ssid}</td>
                                                            <td align="center">${valor.rssi}</td>
                                                            <td>${valor.bssid}/<br>${valor.secure}</td>
                                                            <td align="center">${valor.channel}</td>
                                                            <td><button class="btn btn-primary" onclick="addWiFiInput();">Conectar</button></td>
                                                        </tr>`;
                        } else if (parseInt(valor.rssi) <= -80 && parseInt(valor.rssi) > -90) {
                            mitabla.innerHTML += `<tr class="warning">
                                                            <td align="center">${valor.n}</td>
                                                            <td>${valor.ssid}</td>
                                                            <td align="center">${valor.rssi}</td>
                                                            <td>${valor.bssid}/<br>${valor.secure}</td>
                                                            <td align="center">${valor.channel}</td>
                                                            <td><button class="btn btn-primary" onclick="addWiFiInput();">Conectar</button></td>
                                                        </tr>`;
                        } else {
                            mitabla.innerHTML += `<tr class="danger">
                                                            <td align="center">${valor.n}</td>
                                                            <td>${valor.ssid}</td>
                                                            <td align="center">${valor.rssi}</td>
                                                            <td>${valor.bssid}/<br>${valor.secure}</td>
                                                            <td align="center">${valor.channel}</td>
                                                            <td><button class="btn btn-primary" onclick="addWiFiInput();">Conectar</button></td>
                                                        </tr>`;
                        }
                    };
                } else {
                    // Count = 0
                    // Agregamos cantidad de redes
                    redes.innerHTML = data.meta.count;
                }
            };
        });
}
// Agregar el Nombre de la Red al Input segun Click en el Botón en la fila correspondiente
let addWiFiInput = () => {
    const table = document.getElementById("tableId");
    const rows = table.getElementsByTagName("tr");
    for (let i = 0; i < rows.length; i++) {
        let currentRow = table.rows[i];
        let createClickHandler =
            function (row) {
                return function () {
                    let cell = row.getElementsByTagName("td")[1];
                    let ssid = cell.innerHTML;
                    //alert(ssid);
                    let network = document.getElementById("wifi_ssid");
                    network.value = ssid;
                    $('#myModal').modal('hide');
                };
            };
        currentRow.onclick = createClickHandler(currentRow);
    }
}