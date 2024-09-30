#include <Arduino.h>
#include "leitorQRCode.h"
#include <myWeb.h>
#include <globais.h>
#include <variaveis.h>
#include "esp_task_wdt.h"
#include <sdcard.h>
#include <myEspNow.h>
#include <esp_wifi.h>
#include <ArduinoJson.h>

void setup()
{
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Desabilita o detector de brownout

	Serial.begin(115200);
	WiFi.mode(WIFI_MODE_APSTA);
	esp_wifi_set_promiscuous(true);
	esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
	esp_wifi_set_promiscuous(false);

	esp_task_wdt_init(WDT_TIMER, true); // Iniciando WDT
	esp_task_wdt_add(NULL);							// Adiciona o loopTask ao watchdog timer

	setupSDCard(); // Configurando cartão SD

	setupCamera();
	createQRCodeReaderTask();

	setupEspNow();
}

void loop()
{
	if (!pareado) // Se não tiver pareado
	{
		// Serial.println("enviando para parear no canal: " + String(peerInfo.channel));
		mySendEspNow("FPLFWQL,ESP32CAMTOESP32");

		myDelayMillis(3000);
	}

	if (Serial.available() > 0)
	{
		char comando = Serial.read();
		if (comando == 'a')
		{
			ESP.restart();
		}
		else if (comando == 'b')
		{
			// Serial.print("enviando ip: ");
			String ip;
			if (WiFi.status() == WL_CONNECTED)
			{
				ip = WiFi.localIP().toString();
			}
			else
			{
				IPAddress IP = WiFi.softAPIP();
				Serial.print("Endereço IP do AP: ");
				ip = IP.toString();
			}
			Serial.println(ip);

			ip += SHOW_IP_COMMAND; // Adicionando o comando
			mySendEspNow(ip);
		}
		else if (comando == 'c')
		{
			mySendEspNow(WIFI_BEGIN_COMMAND);
		}
		else if (comando == 'd')
		{
			setupWifi(); // Tenta conectar nesse WiFi
		}
		else if (comando == 'e')
		{
			mySendEspNow(CREATE_AP_COMMAND);
		}
	}

	taskUpdate();
}

void CheckPassCode(const struct quirc_data *data)
{
	if (!pareado) // Se não tiver pareado, sai da função
	{
		// Serial.println("Detectou QRCode, mas não está pareado com o ESP32!");
	}
	else
	{
		String passCode = (const char *)data->payload;
		passCode.trim();

		// Serial.println("QR CODE: " + passCode);

		if (passCode == SHOW_IP_COMMAND) // Se for o QRCode com "mostreip" para exibir os IPs dos ESPs no LCD
		{
			// String ip = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : WiFi.softAPIP().toString();
			// Serial.println("Endereço IP: " + ip);
			// mySendEspNow(ip + SHOW_IP_COMMAND);
		}
		else if (passCode == SHOW_DISTANCE_SENSOR_COMMAND) // Mostra a distância medida dos sensores no display
		{
			mySendEspNow(SHOW_DISTANCE_SENSOR_COMMAND);
		}
		else if (passCode == RESET_ESP_COMMAND)
		{
			mySendEspNow(RESET_ESP_COMMAND);
			ESP.restart();
		}
		else if ((passCode == DELETE_AP_COMMAND) && (WiFi.status() != WL_CONNECTED)) // Deleta AP nos dois ESPs
		{
			// WiFi.softAPdisconnect(true); // Desconecta o AP
			mySendEspNow(DELETE_AP_COMMAND);
		}
		else if ((passCode == WIFI_BEGIN_COMMAND) && (WiFi.status() != WL_CONNECTED)) // Tenta WiFi novamente
		{
			mySendEspNow(WIFI_BEGIN_COMMAND);
		}
		else if (passCode == CREATE_AP_COMMAND)
		{
			mySendEspNow(CREATE_AP_COMMAND);
		}
		else
		{
			passCode += PASSCODE_COMMAND; // Adiciona o comando padrão
			mySendEspNow(passCode);
		}

		QRCodeResult = (const char *)data->payload; // Limpando buffer
	}

	myDelayMillis(2000);
}

void SaveSDCard(String info)
{
	// Divida a string de entrada em ID e payload
	int separatorIndex = info.indexOf('/');
	if (separatorIndex == -1)
	{
		// Serial.println("Formato inválido. A string deve conter '/' para separar ID e payload.");
		return;
	}

	String id = info.substring(0, separatorIndex);
	String payload = info.substring(separatorIndex + 1);

	// Crie o nome do arquivo com base no ID
	String filename = "/Cliente_" + id + ".txt";

	// Serial.println("Chegou isso pra salvar no cartão SD: " + payload);
	// Serial.printf("File name: %s\n", filename.c_str());

	// Gravar informação no microSD
	fs::FS &fs = SD_MMC;
	File file = fs.open(filename.c_str(), FILE_WRITE);
	if (!file)
	{
		// Serial.printf("Failed to open file in writing mode");
	}
	else
	{
		file.println(payload); // Escreve a informação no arquivo
		// Serial.printf("Saved: %s\n", filename.c_str());
	}
	file.close();

	myDelayMillis(1000);
}

//---------------------------------------------------API----------------------------------------------------

void uploadDados()
{
	// Serial.println("Enviando dados...");
	HTTPClient http;

	if (WiFi.status() == WL_CONNECTED)
	{
		// Abre o cartão SD
		File root = SD_MMC.open("/");
		File file = root.openNextFile();
		while (file)
		{
			String filename = file.name();
			// Serial.print("Enviando: ");
			// Serial.println(filename);

			// Abre o arquivo
			File uploadFile = SD_MMC.open(filename);
			if (!uploadFile)
			{
				// Serial.println("Erro ao abrir o arquivo");
				file = root.openNextFile(); // Continua para o próximo arquivo
				continue;
			}

			// Lê o conteúdo do arquivo
			String fileContent = "";
			while (uploadFile.available())
			{
				fileContent += char(uploadFile.read());
			}

			String url = String("http://") + serverIP + ":" + String(serverPort) + "/api/drawer";
			http.begin(url);
			http.addHeader("Content-Type", "application/json");

			// Envia o JSON
			int httpResponseCode = http.POST(fileContent);
			if (httpResponseCode > 0)
			{
				String response = http.getString();
				// Serial.println("Resposta do servidor:");
				// Serial.println(response);

				// Deleta o arquivo se o upload for bem-sucedido
				if (SD_MMC.remove(filename))
				{
					// Serial.println("Arquivo deletado: " + filename);
				}
				else
				{
					// Serial.println("Falha ao deletar o arquivo: " + filename);
				}
			}
			else
			{
				// Serial.println("Erro ao enviar os dados");
			}

			uploadFile.close();
			file = root.openNextFile(); // Continua para o próximo arquivo
		}

		root.close();
		http.end(); // Fecha a conexão HTTP
	}
	else
	{
		// Serial.println("Falha na conexão Wi-Fi");
	}
}

//----------------------------------------------Tasks de tempo----------------------------------------------

void myDelayMillis(uint16_t tempo)
{
	uint32_t startTime = millis(); // Marca o tempo atual para o delay

	while ((millis() - startTime) < tempo)
		;
}

void task_10ms()
{
}

uint8_t contadorWifi = 0; // Contador para o WiFi

// WDT e KeepAlive
void task_1000ms()
{
	if (pareado) // Se tiver pareado, conta e envia o keep alive
	{
		if (wificonnect && (WiFi.status() != WL_CONNECTED))
		{
			setupWifi();
			wificonnect = false;

			// Despareando e pareando pois quando conecta ao WiFi, o canal muda
			if (WiFi.isConnected())
			{
				esp_now_del_peer(receiverMAC); // Despareando

				memcpy(peerInfo.peer_addr, receiverMAC, 6); // Pareando no novo canal
				peerInfo.channel = WiFi.channel();
				peerInfo.encrypt = false;

				if (esp_now_add_peer(&peerInfo) != ESP_OK)
				{
					// Serial.println("Failed to add peer");
				}
			}
		}

		contadorSendKPALV++;
		if (contadorSendKPALV >= 2)
		{
			mySendEspNow(KEEP_ALIVE_COMMAND);
			contadorSendKPALV = 0;
		}

		keepAlive++;
		if (keepAlive >= TIMEOUT_KEEPALIVE && !wificonnect)
		{
			// Serial.println("Perda de comunicação com o ESP32, reiniciando...");
			ESP.restart();
		}
	}

	// contadorAPI++;
	// if (contadorAPI >= TIME_UPDT_API)
	// {
	// 	uploadDados();
	// 	contadorAPI = 0;
	// }

	esp_task_wdt_reset(); // Reseta o watchdog timer
}
