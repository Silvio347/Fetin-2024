#ifndef __myWeb__
#define __myWeb__

#include <WebServer.h>
#include <Update.h>
#include <globais.h>
#include <variaveis.h>
#include <pagesWeb.h>
#include <ArduinoJson.h>

// Parâmetros de rede
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
      Serial.println("Em server.on /avalia: args= " + String(server.arg("autorizacao"))); // somente para debug

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
        Serial.println("ssid salvo: " + ssid);
        resetaESP1 = true;
    }
    if (newPassword.length() > 0) {
        password = newPassword;
        Serial.println("senha salva: " + password);
        resetaESP2 = true;
    }
    if (newServerIP.length() > 0) {
        serverIP = newServerIP;
        Serial.println("ip salvo: " + serverIP);
        resetaESP3 = true;
    }
    if(newServerPort.length() > 0)
    {
        serverPort = newServerPort;
        Serial.println("porta salva: " + serverPort);
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
    } });

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
        Serial.printf("Atualizando: %s\n", upload.filename.c_str());
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
          Serial.printf("Atualização bem sucedida! %u\nReiniciando...\n", upload.totalSize);
        }
        else
        {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      else
      {
        Serial.printf("Atualização falhou inesperadamente! (possivelmente a conexão foi perdida.): status=%d\n", upload.status);
      } });

  // Handle button 1 click
  server.on("/gaveta1", HTTP_POST, []()
            { drawer1Function(); });

  // Handle button 2 click
  server.on("/gaveta2", HTTP_POST, []()
            { drawer2Function(); });

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

#endif