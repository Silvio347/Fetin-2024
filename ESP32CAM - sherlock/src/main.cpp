/*
 * Copyright (c) 2024 Sher-lock
 * https://github.com/Fiddelis/sher-lock
 */

#include <Arduino.h>
#include "leitorQRCode.h"
#include <myWeb.h>
#include <globais.h>
#include <variaveis.h>
#include "esp_task_wdt.h"
#include <sdcard.h>
#include <esp_now.h>
#include <EEPROM.h>
#include <HTTPClient.h>
#include "fb_gfx.h"
#include "soc/soc.h"					//disable brownout problems
#include "soc/rtc_cntl_reg.h" //disable brownout problems
#include "esp_http_server.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "esp_camera.h"

esp_now_peer_info_t peerInfo;
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len);

void setup()
{
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // Desabilita o detector de brownout
	Serial.setDebugOutput(false);

	Serial.begin(115200);

	// Inicializa o watchdog timer com um timeout de 10 segundos
	esp_task_wdt_init(WDT_TIMER, true);
	esp_task_wdt_add(NULL); // Adiciona o loopTask ao watchdog timer

	setupSDCard(); // Configurando cartão SD

	// Configurando câmera e task para ler QR Code
	setupCamera();
	createQRCodeReaderTask();

	setupEspNow();
}

void loop()
{
	if (!pareado) // Se não tiver pareado
	{
		Serial.println("enviando para parear...");
		mySendEspNow("FPLFWQL,ESP32CAMTOESP32");
		myDelayMillis(2000);
	}

	if (Serial.available() > 0)
	{
		char comando = Serial.read();
		if (comando == 'a')
		{
			Serial.println("enviando passcode de teste...");
			mySendEspNow("/aaa");
		}
		else if (comando == 'b')
		{
			Serial.print("enviando ip: ");
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
			setupWifi();					// Tenta conectar nesse WiFi
			setupWeb();						// Configurando a página Web
			initTime(myTimezone); // Initialize time with timezone
		}
		else if (comando == 'e')
		{
			createAP();
			mySendEspNow(CREATE_AP_COMMAND);
		}
	}

	taskUpdate();
}

void CheckPassCode(const struct quirc_data *data)
{
	if (!pareado) // Se não tiver pareado, sai da função
	{
		Serial.println("detectou QRCode mas não está pareado com o ESP32!");
		myDelayMillis(2000);
		return;
	}

	String passCode = (const char *)data->payload;
	passCode.trim();

	Serial.println(passCode);

	if (passCode == SHOW_IP_COMMAND) // Se for o QRCode com "mostreip" para exibir os IPs dos ESPs no LCD
	{
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
			Serial.println(ip);
		}
		mySendEspNow(ip + SHOW_IP_COMMAND);
		return;
	}
	else if (passCode == SHOW_DISTANCE_SENSOR_COMMAND) // Mostra a distância medida dos sensores no display
	{
		mySendEspNow(SHOW_DISTANCE_SENSOR_COMMAND);
		return;
	}
	else if ((passCode == CREATE_AP_COMMAND) && (WiFi.status() != WL_CONNECTED)) // Cria AP nos dois ESPs
	{
		createAP();
		return;
	}
	else if ((passCode == DELETE_AP_COMMAND) && (WiFi.status() != WL_CONNECTED)) // Deleta AP nos dois ESPs
	{
		WiFi.softAPdisconnect(true); // Desconecta o AP
		mySendEspNow(DELETE_AP_COMMAND);
		return;
	}
	else if ((passCode == WIFI_BEGIN_COMMAND) && (WiFi.status() != WL_CONNECTED)) // tenta wifi dnv
	{
		mySendEspNow(WIFI_BEGIN_COMMAND);
		return;
	}

	passCode += PASSCODE_COMMAND; // Adicionando comando
	mySendEspNow(passCode);

	QRCodeResult = (const char *)data->payload; // Limpando buffer

	myDelayMillis(2000);
}

void myDelayMillis(uint16_t tempo)
{
	uint32_t startTime = millis(); // Marca o tempo atual para o delay

	while ((millis() - startTime) < tempo)
		;
}

void setupEspNow()
{
	WiFi.mode(WIFI_STA);
	Serial.println(WiFi.macAddress()); // Imprime o MAC desse ESP

	esp_now_peer_num_t espnum;
	espnum.total_num = 1; // Definindo quantidade de dispositivos limite pareados

	esp_now_deinit(); // apaga o pareamento
	myDelayMillis(1000);

	if (esp_now_init() != ESP_OK) // Inicia o ESP-NOW
	{
		Serial.println("ESP-NOW initialization failed");
		return;
	}

	esp_now_register_recv_cb(OnDataRecv); // Função de callback
	esp_now_register_send_cb(OnDataSent); // Função de callback

	uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Endereço MAC de broadcast

	memcpy(peerInfo.peer_addr, broadcastAddress, 6);
	memcpy(receiverMAC, broadcastAddress, 6);
	peerInfo.channel = 0;

	// esp_now_set_pmk((uint8_t *)PMK_KEY_STR);
	// for (uint8_t i = 0; i < 16; i++)
	// {
	//   peerInfo.lmk[i] = LMK_KEY_STR[i];
	// }
	peerInfo.encrypt = false;

	// Add peer
	if (esp_now_add_peer(&peerInfo) != ESP_OK)
	{
		Serial.println("Erro no pareamento");
		return;
	}
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
	if (status != ESP_NOW_SEND_SUCCESS)
	{
		Serial.println("não enviou certo (callback).");
	}
	else
	{
		// Serial.println("enviou certo (callback).");
	}
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int len)
{
	char receivedData[len + 1]; // +1 para o caractere nulo
	memcpy(receivedData, data, len);
	receivedData[len] = '\0'; // Garantir a terminação com null
	String receivedDataStr = String(receivedData);
	receivedDataStr.trim();

	Serial.println("Chegou: " + receivedDataStr);

	// Se chegar o comando certo para parear
	if ((receivedDataStr == PAREAR_ESP32_TO_ESP32CAM) && !pareado)
	{
		// Armazena o MAC do receptor
		memcpy(receiverMAC, mac_addr, 6);

		memcpy(peerInfo.peer_addr, mac_addr, 6); // Para parear
		peerInfo.channel = 0;

		// Criptografia
		esp_now_set_pmk((uint8_t *)PMK_KEY_STR);
		for (uint8_t i = 0; i < 16; i++)
		{
			peerInfo.lmk[i] = LMK_KEY_STR[i];
		}
		peerInfo.encrypt = false; // Define se criptografa ou n

		if (esp_now_add_peer(&peerInfo) != ESP_OK) // Pareando
		{
			Serial.println("Failed to add peer");
			return;
		}
		Serial.println("Pareado com o ESP32!");
		pareado = true;
		return;
	}

	if (pareado) // Se tiver pareado
	{
		if (receivedDataStr == KEEP_ALIVE_COMMAND) // Zera o keep alive
		{
			keepAlive = 0;
		}
		else if (receivedDataStr.endsWith(PARAMETERS_COMMAND)) // Obtém o ssid para  o wifi
		{
			receivedDataStr.replace(PARAMETERS_COMMAND, "");

			int slashIndex = receivedDataStr.indexOf('/');
			int hashIndex = receivedDataStr.indexOf('#');
			int atIndex = receivedDataStr.indexOf('@');

			String name = receivedDataStr.substring(0, slashIndex);
			String password = receivedDataStr.substring(slashIndex + 1, hashIndex);
			String serverIP = receivedDataStr.substring(hashIndex + 1, atIndex);
			String serverPort = receivedDataStr.substring(atIndex + 1);

			setupWifi();
			setupWeb();
			initTime(myTimezone);
		}
		else if (receivedDataStr == TAKE_PHOTO_COMMAND) // Tira e salva foto no cartão SD
		{
			SaveSDCard();
		}
	}
}

void mySendEspNow(String data)
{
	const char *dataStr = data.c_str();
	uint8_t *buffer = (uint8_t *)data.c_str();
	size_t sizeBuff = sizeof(buffer) * data.length();
	esp_now_send(receiverMAC, buffer, sizeBuff);
}

//-------------------------------------------------Cartão SD--------------------------------------------------

void setTimezone(String timezone)
{
	Serial.printf("  Setting Timezone to %s\n", timezone.c_str());
	setenv("TZ", timezone.c_str(), 1); //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
	tzset();
}

void initTime(String timezone)
{
	if (WiFi.status() == WL_CONNECTED) // Se tiver com internet
	{
		struct tm timeinfo;
		Serial.println("Setting up time");
		configTime(0, 0, "pool.ntp.org"); // First connect to NTP server, with 0 TZ offset
		if (!getLocalTime(&timeinfo))
		{
			Serial.println(" Failed to obtain time");
			return;
		}
		Serial.println("Got the time from NTP");
		// Now we can set the real timezone
		setTimezone(timezone);
	}
}

String getPictureFilename()
{
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo))
	{
		Serial.println("Failed to obtain time");
		return "";
	}
	char timeString[20];
	strftime(timeString, sizeof(timeString), "%Y-%m-%d_%H-%M-%S", &timeinfo);
	Serial.println(timeString);
	String filename = "/picture_" + String(timeString) + ".jpg";
	return filename;
}

void SaveSDCard()
{
	digitalWrite(LED_PIN, HIGH);

	// Take Picture with Camera
	camera_fb_t *fb = esp_camera_fb_get();

	digitalWrite(LED_PIN, LOW);

	if (!fb)
	{
		Serial.println("Camera capture failed");
		myDelayMillis(1000);
		mySendEspNow(RESET_ESP_COMMAND);
		ESP.restart();
	}

	String path; // Path where new picture will be saved in SD Card (name of file)

	path = getPictureFilename();

	// Se estiver vazio, significa que não pegou o tempo corretamente
	if (path == "")
	{
		path = "/picture_unknown_time.jpg";
	}

	Serial.printf("Picture file name: %s\n", path.c_str());

	// Save picture to microSD card
	fs::FS &fs = SD_MMC;
	File file = fs.open(path.c_str(), FILE_WRITE);
	if (!file)
	{
		Serial.printf("Failed to open file in writing mode");
	}
	else
	{
		file.write(fb->buf, fb->len); // payload (image), payload length
		Serial.printf("Saved: %s\n", path.c_str());
	}
	file.close();
	esp_camera_fb_return(fb);

	myDelayMillis(1000);
}

//---------------------------------------------------Wifi----------------------------------------------------

void createAP()
{
	WiFi.mode(WIFI_MODE_APSTA);							// Comfigura o ESP32CAM como ponto de acesso e estação
	WiFi.softAP(SSID_ESP, PASSWORD_ESP, 0); // Nome da rede, senha e canal

	IPAddress IP = WiFi.softAPIP();
	String ip = IP.toString();
	mySendEspNow(ip + CREATE_AP_COMMAND);
	Serial.println("ap: " + ip);
}

void setupWifi()
{
	WiFi.mode(WIFI_MODE_APSTA);
	WiFi.begin(ssid, password, 0); // inicia a conexão com o WiFi

	uint8_t contador_wifi = 0; // Contador do time-out

	// Wait for connection
	while (WiFi.status() != WL_CONNECTED)
	{
		contador_wifi++;
		if (contador_wifi == 4) // 4 segundos
		{
			Serial.println("Sem conexão WiFi. Gerando AP (Access Point)...");
			createAP();
			break;
		}
		myDelayMillis(500);
	}

	if (WiFi.status() == WL_CONNECTED)
	{
		String ip = WiFi.localIP().toString();
		Serial.print("Conectado em ");
		Serial.println(ssid);
		Serial.print("Servidor em: ");
		Serial.println(ip);
		mySendEspNow(ip + SHOW_IP_COMMAND);
	}
}

void uploadFotos()
{
	Serial.println("Enviando fotos...");
	HTTPClient http;
	bool falha = false;

	if (WiFi.status() == WL_CONNECTED)
	{
		// Abre o cartão SD
		File root = SD_MMC.open("/");
		File file = root.openNextFile();
		while (file)
		{
			String filename = file.name();
			Serial.print("Enviando: ");
			Serial.println(filename);

			// Abre o arquivo
			File uploadFile = SD_MMC.open(filename);
			if (!uploadFile)
			{
				Serial.println("Erro ao abrir o arquivo");
				falha = true;
				file = root.openNextFile(); // Continua para o próximo arquivo
				continue;
			}

			// Construa a solicitação multipart/form-data
			String boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
			String contentType = "multipart/form-data; boundary=" + boundary;
			String url = String("http://") + serverIP + ":" + String(serverPort) + "/api/endpoint";
			http.begin(url);
			http.addHeader("Content-Type", contentType);

			// Envio do arquivo
			String postData = "--" + boundary + "\r\n";
			postData += "Content-Disposition: form-data; name=\"file\"; filename=\"" + filename + "\"\r\n";
			postData += "Content-Type: application/octet-stream\r\n\r\n";
			http.sendRequest("POST", postData);

			// Envia o conteúdo do arquivo em chunks
			size_t bufferSize = 1024;
			uint8_t buffer[bufferSize];
			size_t bytesRead;

			while ((bytesRead = uploadFile.read(buffer, sizeof(buffer))) > 0)
			{
				http.sendRequest("POST", String((char *)buffer, bytesRead));
			}

			// Envia a parte final do arquivo
			String endData = "\r\n--" + boundary + "--\r\n";
			http.sendRequest("POST", endData);

			// Verifica a resposta do servidor
			int httpResponseCode = http.GET();
			if (httpResponseCode > 0)
			{
				String response = http.getString();
				Serial.println("Resposta do servidor:");
				Serial.println(response);

				// Deleta o arquivo se o upload for bem-sucedido
				if (SD_MMC.remove(filename))
				{
					Serial.println("Arquivo deletado: " + filename);
				}
				else
				{
					Serial.println("Falha ao deletar o arquivo: " + filename);
					server.send(200, "text/plain", "Falha ao apagar arquivos do cartão SD!");
				}
			}
			else
			{
				Serial.println("Erro ao enviar a foto");
				falha = true;
			}

			uploadFile.close();
			file = root.openNextFile(); // Continua para o próximo arquivo
		}

		root.close();
		http.end(); // Fecha a conexão HTTP
	}
	else
	{
		Serial.println("Falha na conexão Wi-Fi");
	}

	if (!falha)
	{
		server.send(200, "text/plain", "Fotos salvas do cartão SD no banco de dados!");
	}
}

typedef struct
{
	httpd_req_t *req;
	size_t len;
} jpg_chunking_t;

static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

static size_t jpg_encode_stream(void *arg, size_t index, const void *data, size_t len)
{
	jpg_chunking_t *j = (jpg_chunking_t *)arg;
	if (!index)
	{
		j->len = 0;
	}
	if (httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK)
	{
		return 0;
	}
	j->len += len;
	return len;
}

static esp_err_t capture_handler(httpd_req_t *req)
{
	camera_fb_t *fb = NULL;
	esp_err_t res = ESP_OK;
	int64_t fr_start = esp_timer_get_time();

	fb = esp_camera_fb_get();
	if (!fb)
	{
		Serial.println("Camera capture failed");
		httpd_resp_send_500(req);
		return ESP_FAIL;
	}

	httpd_resp_set_type(req, "image/jpeg");
	httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
	httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

	size_t out_len, out_width, out_height;
	uint8_t *out_buf;
	bool s;
	bool detected = false;
	int face_id = 0;
	if (true)
	{
		size_t fb_len = 0;
		if (fb->format == PIXFORMAT_JPEG)
		{
			fb_len = fb->len;
			res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
		}
		else
		{
			jpg_chunking_t jchunk = {req, 0};
			res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk) ? ESP_OK : ESP_FAIL;
			httpd_resp_send_chunk(req, NULL, 0);
			fb_len = jchunk.len;
		}
		esp_camera_fb_return(fb);
		int64_t fr_end = esp_timer_get_time();
		Serial.printf("JPG: %uB %ums\n", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start) / 1000));
		return res;
	}
}

static esp_err_t stream_handler(httpd_req_t *req)
{
	camera_fb_t *fb = NULL;
	esp_err_t res = ESP_OK;
	size_t _jpg_buf_len = 0;
	uint8_t *_jpg_buf = NULL;
	char *part_buf[64];

	res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
	if (res != ESP_OK)
	{
		return res;
	}

	while (true)
	{
		fb = esp_camera_fb_get();
		if (!fb)
		{
			Serial.println("Camera capture failed");
			res = ESP_FAIL;
		}
		else
		{
			if (fb->width > 400)
			{
				if (fb->format != PIXFORMAT_JPEG)
				{
					bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
					esp_camera_fb_return(fb);
					fb = NULL;
					if (!jpeg_converted)
					{
						Serial.println("JPEG compression failed");
						res = ESP_FAIL;
					}
				}
				else
				{
					_jpg_buf_len = fb->len;
					_jpg_buf = fb->buf;
				}
			}
		}
		if (res == ESP_OK)
		{
			size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
			res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
		}
		if (res == ESP_OK)
		{
			res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
		}
		if (res == ESP_OK)
		{
			res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
		}
		if (fb)
		{
			esp_camera_fb_return(fb);
			fb = NULL;
			_jpg_buf = NULL;
		}
		else if (_jpg_buf)
		{
			free(_jpg_buf);
			_jpg_buf = NULL;
		}
		if (res != ESP_OK)
		{
			break;
		}
		// Serial.printf("MJPG: %uB\n",(uint32_t)(_jpg_buf_len));
	}
	return res;
}

void startCameraServer()
{
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.server_port = 80;

	httpd_uri_t index_uri = {
			.uri = "/",
			.method = HTTP_GET,
			.handler = stream_handler,
			.user_ctx = NULL};

	httpd_uri_t capture_uri = {
			.uri = "/stream",
			.method = HTTP_GET,
			.handler = capture_handler,
			.user_ctx = NULL};

	if (httpd_start(&stream_httpd, &config) == ESP_OK)
	{
		httpd_register_uri_handler(stream_httpd, &index_uri);
	}

	config.server_port += 1;
	config.ctrl_port += 1;
	if (httpd_start(&camera_httpd, &config) == ESP_OK)
	{
		httpd_register_uri_handler(camera_httpd, &capture_uri);
	}
}

//----------------------------------------------Tasks de tempo----------------------------------------------

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
		task_5ms();

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
		if (_tic25ms > 24)
		{
			_tic25ms -= 25;
			task_25ms();
		}
		if (_tic50ms > 49)
		{
			_tic50ms -= 50;
			task_50ms();
		}

		if (_tic250ms > 249)
		{
			_tic250ms -= 250;
			task_250ms();
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

void task_5ms()
{
}

// Server handle
void task_10ms()
{
	server.handleClient();
}

void task_25ms()
{
}

void task_50ms()
{
}

void task_250ms()
{
}

void task_500ms()
{
}

// WDT e KeepAlive
void task_1000ms()
{
	if (pareado) // Se tiver pareado, conta e envia o keep alive
	{
		mySendEspNow(KEEP_ALIVE_COMMAND);
		// keepAlive++;
		// Serial.println(keepAlive);
		if (keepAlive >= TIMEOUT_KEEPALIVE)
		{
			Serial.println("Perda de comunicação com o ESP32, reiniciando...");
			ESP.restart();
		}
	}
	esp_task_wdt_reset(); // Reseta o watchdog timer
}