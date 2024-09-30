#include <myWeb.h>

IPAddress local_ip(192, 168, 0, 121);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
boolean OTA_AUTORIZADO = false; // Sinalizador de autorização do OTA
WebServer server(80);           // inicia o servidor na porta selecionada

void setupWeb()
{
  server.on("/", HTTP_GET, []()
            {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", verifica); });

  // Atende uma solicitação para a página avalia
  server.on("/avalia", HTTP_POST, []()
            {
      // Serial.println("Em server.on /avalia: args= " + String(server.arg("autorizacao"))); // somente para debug

      if (server.arg("autorizacao") != PASSWORD_SITE) // confere se o dado de autorização atende a avaliação
      {
        // Se não atende, serve a página indicando uma falha
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", Resultado_Falha);
      }
      else
      {
        // Se atende, solicita a página de índice do servidor
        // e sinaliza que o OTA está autorizado
        OTA_AUTORIZADO = true;
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", serverIndex);
      } });

  // Serve a página de índice do servidor para seleção do arquivo
  server.on("/serverIndex", HTTP_GET, []()
            {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", serverIndex); });

  server.on("/saveParams", HTTP_POST, []()
            {
    String newSSID = server.arg("ssid");
    String newPassword = server.arg("password");
    String newServerIP = server.arg("serverIP");
    String newServerPort = server.arg("serverPort");
    newSSID.trim();
    newPassword.trim();
    newServerIP.trim();
    newServerPort.trim();
    bool resetaESP1 = false, resetaESP2 = false, resetaESP3 = false;

    // Atualiza as variáveis
    if (newSSID.length() > 0) {
        ssid = newSSID;
        // Serial.println("ssid salvo: " + ssid);
        resetaESP1 = true;
    }
    if (newPassword.length() > 0) {
        password = newPassword;
        // Serial.println("senha salva: " + password);
        resetaESP2 = true;
    }
    if (newServerIP.length() > 0) {
        serverIP = newServerIP;
        // Serial.println("ip salvo: " + serverIP);
        resetaESP3 = true;
    }
    if(newServerPort.length() > 0)
    {
        serverPort = newServerPort;
        // Serial.println("porta salva: " + serverPort);
    }

    if (resetaESP1 && resetaESP2 && resetaESP3)
    {
        // Serve uma página de confirmação
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", "Parâmetros atualizados com sucesso!");
        
        salvarParametrosNaEEPROM();
        myDelayMillis(1000);
        mySendEspNow(RESET_ESP_COMMAND);
        ESP.restart();
    }
    else
     server.send(200, "text/html", "Preencha todos os campos!"); });

  // Atualiza o firmware
  server.on("/update", HTTP_POST, []()
            {
      // Verifica se a autorização é falsa
      if (OTA_AUTORIZADO == false)
      {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", Resultado_Falha);
        return;
      }
      
      // Serve uma página final que depende do resultado da atualização
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", (Update.hasError()) ? Resultado_Falha : Resultado_Ok);
      myDelayMillis(1000);
      mySendEspNow(RESET_ESP_COMMAND);
      ESP.restart(); }, []()
            {
      // Inicia a atualização
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START)
      {
        Serial.setDebugOutput(true);
        // Serial.printf("Atualizando: %s\n", upload.filename.c_str());
        if (!Update.begin())
        {
          Update.printError(Serial);
        }
      }
      else if (upload.status == UPLOAD_FILE_WRITE)
      {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
        {
          Update.printError(Serial);
        }
      }
      else if (upload.status == UPLOAD_FILE_END)
      {
        if (Update.end(true))
        {
          updateLCD("Firmware atualizado via OTA. Reiniciando.");
          // Serial.printf("Atualização bem sucedida! %u\nReiniciando...\n", upload.totalSize);
        }
        else
        {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      else
      {
        // Serial.printf("Atualização falhou inesperadamente! (possivelmente a conexão foi perdida.): status=%d\n", upload.status);
      } });

  // Handle button 1 click
  server.on("/gaveta1", HTTP_POST, []()
            { drawer1Function(); });

  // Handle button 2 click
  server.on("/gaveta2", HTTP_POST, []()
            { drawer2Function(); });

  // Handle button 3 click
  server.on("/updateImages", HTTP_POST, []()
            { mySendEspNow(UPDATE_COMMAND); 
            server.send(200, "text/html", "Preencha todos os campos!"); });

  // Atualizar informações das distância dos sensores
  server.on("/getSensorData", HTTP_GET, []()
            {
        StaticJsonDocument<200> doc;
        doc["distance1"] = String(distanceSensor[0], 2);
        doc["distance2"] = String(distanceSensor[1], 2);

        String jsonResponse;
        serializeJson(doc, jsonResponse);

        // Envia a resposta ao cliente
        server.send(200, "application/json", jsonResponse); });

  server.begin(); // inicia o servidor
}

void taskUpdate()
{
  static uint16_t _tic1000ms = 0, _tic500ms = 0, _tic250ms = 0;
  static uint8_t _tic10ms = 0, _tic25ms = 0, _tic50ms = 0;
  static uint32_t updateTime = 0;
  uint32_t currentMillis = millis();
  uint32_t elapsedTime = 0;

  if (updateTime == 0)
  {
    updateTime = currentMillis;
  }
  elapsedTime = currentMillis - updateTime;
  if (elapsedTime > 4) // 5ms
  {

    updateTime = currentMillis;

    _tic10ms += elapsedTime;
    _tic25ms += elapsedTime;
    _tic50ms += elapsedTime;
    _tic250ms += elapsedTime;
    _tic500ms += elapsedTime;
    _tic1000ms += elapsedTime;

    if (_tic10ms > 9)
    {
      _tic10ms -= 10;
      task_10ms();
    }
    if (_tic500ms > 499)
    {
      _tic500ms -= 500;
      task_500ms();
    }
    if (_tic1000ms > 999)
    {
      _tic1000ms -= 1000;
      task_1000ms();
    }
  }
}

void createAP()
{
  WiFi.softAP(SSID_ESP, PASSWORD_ESP);
  IPAddress IP = WiFi.softAPIP();
  String ip = IP.toString();
  updateLCD("AP Criado!", 34, 25);
  updateLCD(ip, 30, 40, false);
  // mySendEspNow(CREATE_AP_COMMAND);
  // updateLCD("IP do ESP32: " + ip);
  apActive = true;
}

void setupWifi()
{
  // Serial.println("ssid: " + ssid + ", password: " + password);
  WiFi.begin(ssid, password); // inicia a conexão com o WiFi

  uint8_t contador_wifi = 0;

  while (WiFi.status() != WL_CONNECTED) // Wait for connection
  {
    contador_wifi++;
    if (contador_wifi >= TIMEOUT_WIFI)
    {
      if (!apActive)
        updateLCD("Sem conexao WiFi.", 13, 32);

      WiFi.disconnect();
      WiFi.channel(1);
      myDelayMillis(50);
      contador_wifi = 0;
      break;
    }
    myDelayMillis(500);
  }

  if (WiFi.isConnected())
  {
    jaSalvou = false;
    String ip = WiFi.localIP().toString();
    // Serial.println(ssid);
    updateLCD("Conectado em: " + ssid, 0, 1);
    // Serial.println(ip);
    updateLCD("Servidor em:", 34, 25, false);
    updateLCD(ip, 23, 40, false);
    
    updateTime(); // Pega o tempo na API
    myDelayMillis(1000);
    mySendEspNow(UPDATE_COMMAND);
    delPeerAndPair();
    getAPI();
  }
  setupWeb(); // Configurando a página Web
}

void drawer1Function()
{
  if (drawer1Opened)
  {
    updateLCD("Gaveta 1 ja esta aberta!", 0, 32);
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", "Gaveta 1 já está aberta!");
  }
  else
  {
    server.sendHeader("Connection", "close");
    // server.send(200, "text/plain", "Gaveta 1 aberta!");
    updateLCD("Gaveta 1 aberta!", 0, 32);
    digitalWrite(RELAY_2, 1);
    myDelayMillis(200);
    digitalWrite(RELAY_1, 1);
    myDelayMillis(5000);
    digitalWrite(RELAY_2, 0);
    digitalWrite(RELAY_1, 0);
    drawer1Opened = true;
  }
}

void drawer2Function()
{
  if (drawer2Opened)
  {
    updateLCD("Gaveta 2 ja esta aberta!", 0, 32);
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", "Gaveta 2 já está aberta!");
  }
  else
  {
    server.sendHeader("Connection", "close");
    // server.send(200, "text/plain", "Gaveta 2 aberta!");
    updateLCD("Gaveta 2 aberta!", 0, 32);
    drawer2Opened = true;
    digitalWrite(RELAY_2, 1);
    myDelayMillis(200);
    digitalWrite(RELAY_1, 1);
    myDelayMillis(5000);
    digitalWrite(RELAY_2, 0);
    digitalWrite(RELAY_1, 0);
  }
}
