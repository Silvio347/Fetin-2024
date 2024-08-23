#ifndef __myWeb__
#define __myWeb__

#include <WebServer.h>
#include <Update.h>
#include <globais.h>
#include <pagesWeb.h>
#include <variaveis.h>

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

  // Configuração do servidor
  server.on("/", HTTP_GET, []()
            { server.send(200, "text/html", serverIndex); });

  server.on("/uploadFotos", HTTP_POST, []()
            { uploadFotos(); });

  server.on("/InitTransmition", HTTP_POST, []()
            {
    vTaskDelete(QRCodeReader_Task);  // Apagar a task de ler QR Code
    QRCodeReader_Task = NULL;        // Limpar o handle da task
    transmissaoAtiva = true;
    startCameraServer();
    server.send(200, "text/plain", "Transmissão iniciada"); });

  server.on("/EndTransmition", HTTP_POST, []()
            {
    createQRCodeReaderTask();
    transmissaoAtiva = false;
    server.send(200, "text/plain", "Transmissão encerrada"); });

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

  server.begin(); // inicia o servidor
}

#endif