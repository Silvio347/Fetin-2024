#include <Arduino.h>
#include <myWeb.h>
#include "variaveis.h"
#include <globais.h>
#include "esp_task_wdt.h"
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include <bsp.h>
#include <myEspNow.h>
#include <esp_wifi.h>
#include <time.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Cliente produtos[NUM_PRODUCTS];

void setup()
{
	Serial.begin(115200);

	WiFi.mode(WIFI_MODE_APSTA);
	esp_wifi_set_promiscuous(true);
	esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
	esp_wifi_set_promiscuous(false);

	if (!Wire.begin(SDA_PIN, SCL_PIN, 400000))
	{
		// Serial.println("I2C não inicializou, tentando novamente...");
		myDelayMillis(500);
		return;
	}

	display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

	display.display();
	updateLCD("SHERLOCK", 40, 32);

	setupEEPROM();

	lerParametrosDaEEPROM(); // Obtendo os parâmetros de WiFi e API salvos na EEPROM

	setupEspNow(); // Iniciando espnow
	mySendEspNow(RESET_ESP_COMMAND);

	// Inicializa o watchdog timer com um timeout de 10 segundos
	esp_task_wdt_init(WDT_TIMER, true);
	esp_task_wdt_add(NULL); // Adiciona o loopTask ao watchdog timer

	// Pino dos sensores, buzzer e relés
	pinMode(TRIG_PIN1, OUTPUT);
	pinMode(ECHO_PIN1, INPUT);
	pinMode(TRIG_PIN2, OUTPUT);
	pinMode(ECHO_PIN2, INPUT);
	pinMode(END_COURSE1, INPUT_PULLUP);
	pinMode(END_COURSE2, INPUT_PULLUP);
	pinMode(BEEP, OUTPUT);
	pinMode(RELAY_1, OUTPUT);
	pinMode(RELAY_2, OUTPUT);
	digitalWrite(BEEP, 0);
	digitalWrite(RELAY_1, 0);
	digitalWrite(RELAY_2, 0);

	produtos[0].getEEPROM(CLIENT_ADDRESS_1);
	produtos[1].getEEPROM(CLIENT_ADDRESS_2);
}

void loop()
{
	if (Serial.available() > 0)
	{
		char comando = Serial.read();
		if (comando == 'b')
		{
			ESP.restart();
		}
		else if (comando == 'c')
		{
			serverPort = "8080";
			serverIP = "192.168.131.219";
			ssid = "Fidelis";
			password = "40028922";
			serverPort.trim();
			serverIP.trim();
			ssid.trim();
			password.trim();
			salvarParametrosNaEEPROM();
		}
		else if (comando == 'd')
		{
			Serial.println(getTime());
		}
		else if (comando == 'f')
		{
			digitalWrite(RELAY_1, !digitalRead(RELAY_1));
		}
		else if (comando == 'g')
		{
			digitalWrite(RELAY_2, !digitalRead(RELAY_2));
		}
		else if (comando == 'h')
		{
			produtos[0].setRAM(1, "camisa x", "oii", "teste", 1);
			produtos[0].setEEPROM(CLIENT_ADDRESS_1); // Salvando os trem na EEPROM
			Serial.println("salvo na eeprom");
		}
		else if (comando == 'i')
		{
			setupWifi(); // Configurando acesso ao WiFi
		}
		else if (comando == 'j')
		{
			digitalWrite(BEEP, !digitalRead(BEEP));
		}
		else if (comando == 'k')
		{
			getAPI();
		}
		else if (comando == 'l')
		{
			updateTime();
			Serial.println(getTime());
		}
	}

	taskUpdate();
}

void checkPassCode(String recebido)
{
	bool qrInvalido = false;
	uint8_t drawerIdsOpened[NUM_PRODUCTS] = {drawer1Opened, drawer2Opened};
	uint8_t productsAddrs[NUM_PRODUCTS] = {CLIENT_ADDRESS_1, CLIENT_ADDRESS_2};

	for (uint8_t i = 0; i < NUM_PRODUCTS; i++)
	{
		// Serial.println("qr code: " + String(recebido) + ", na RAM produto: " + produtos[i].getWithdrawnCode());

		if ((recebido == produtos[i].getWithdrawnCode()) && foiEntregue[i])
		{
			updateLCD("Retire o Produto: ", 32, 0);
			updateLCD(produtos[i].getNome(), 0, 32, false);
			myDelayMillis(1000);
			produtos[i].setTimeWithdrawn(getTime());

			digitalWrite(BEEP, 1);
			myDelayMillis(500);
			digitalWrite(BEEP, 0);

			digitalWrite(RELAY_2, 1);
			myDelayMillis(200);
			digitalWrite(RELAY_1, 1);

			// Serial.println("abrindo gaveta " + String(i + 1));

			drawerIdsOpened[i] = true;
			qrInvalido = true;

			myDelayMillis(5000);
			digitalWrite(RELAY_2, 0);
			digitalWrite(RELAY_1, 0);

			if (i == 0)
				atualizaAPI1 = true;
			else
				atualizaAPI2 = true;

			foiEntregue[i] = false;
			produtos[i].apagarNaEEPROM(productsAddrs[i]);
		}
		else if (recebido == produtos[i].getDeliveryCode())
		{
			produtos[i].setTimeInserted(getTime());
			updateLCD("Oi entregador :)", 0, 0);
			updateLCD("Insira o produto:", 0, 20, false);
			updateLCD(produtos[i].getNome(), 0, 50, false);
			digitalWrite(BEEP, 1);
			myDelayMillis(500);
			digitalWrite(BEEP, 0);

			digitalWrite(RELAY_2, 1);
			myDelayMillis(200);
			digitalWrite(RELAY_1, 1);
			drawerIdsOpened[i] = true;
			qrInvalido = true;

			myDelayMillis(5000);
			digitalWrite(RELAY_2, 0);
			digitalWrite(RELAY_1, 0);
			foiEntregue[i] = true;
		}
	}
	if (!qrInvalido)
		updateLCD("QR Code invalido!", 0, 32);
}

void updateLCD(String messageLCD, uint8_t posX, uint8_t posY, bool clearScreen)
{
	// Serial.println(messageLCD);

	if (clearScreen)
		display.clearDisplay();

	display.setTextSize(1);
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(posX, posY);
	display.println(messageLCD);
	display.display();
	contadorLCD = 0; // Zerando contador de inatividade
}

void sensorReader(uint8_t trigPin, uint8_t echoPin, uint8_t drawerID, bool &drawerOpened)
{
	uint8_t indiceSensor = drawerID - 1; // Para começar em 0

	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);

	// Mede a duração do pulso no pino Echo
	unsigned long duration = pulseIn(echoPin, HIGH, 500000);

	// Calcula a distância em centímetros
	distanceSensor[indiceSensor] = duration * 0.0344 / 2;

	// Serial.print("Distancia Sensor " + String(drawerID) + " :");
	// Serial.print(distanceSensor[indiceSensor]);
	// Serial.println(" cm");

	// // Verifica se o produto foi retirado, a gaveta tem que estar aberta e o produto marcado como disponível na memória
	// if ((distanceSensor[indiceSensor] >= MAX_DISTANCE_SENSOR) && (produtos[indiceSensor].getWithdrawnCode() != ""))
	// {
	// 	updateLCD("O produto " + String(produtos[indiceSensor].getNome()) + " foi retirado!", 0, 30);
	// }
	// // Verifica se o produto foi inserido pelo entregador
	// else if ((distanceSensor[indiceSensor] >= MIN_DISTANCE_SENSOR) && (distanceSensor[indiceSensor] <= MAX_DISTANCE_SENSOR) &&
	// 				 (produtos[indiceSensor].getDeliveryCode() == ""))
	// {
	// 	updateLCD("Entregador inseriu o produto " + String(produtos[indiceSensor].getNome()), 0, 30);
	// }
}

//------------------------------------------------API--------------------------------------------------------

void getAPI()
{
	if (!wificonnect && !WiFi.isConnected())
	{
		conetado = false;
		if ((lerStringDaEEPROM(33, 32) != produtos[0].getWithdrawnCode()) && !jaSalvou)
		{
			jaSalvou = true;
			produtos[0].setEEPROM(0);
		}
		return;
	}
	apActive = false;
	conetado = true;

	HTTPClient http;
	String urlProduct = "http://" + serverIP + ":" + serverPort + "/api/product/by_locker/1";
	// Serial.println(urlProduct);
	http.begin(urlProduct);
	int httpCode = http.GET();
	http.setTimeout(500); // timeout de resposta da API

	if (httpCode > 0)
	{
		if (httpCode == HTTP_CODE_OK)
		{
			String payload = http.getString();
			// Serial.println("Resposta da API:");
			// Serial.println(payload);

			// Cria um objeto JsonDocument
			DynamicJsonDocument doc(4096); // Ajuste o tamanho conforme necessário

			// Desserializa o JSON
			DeserializationError error = deserializeJson(doc, payload);

			if (!error)
			{
				JsonArray products = doc.as<JsonArray>();

				// Itera sobre cada produto no vetor de JSON
				for (uint8_t i = 0; i < products.size(); i++)
				{
					JsonObject product = products[i];

					// Define os atributos do produto no objeto Cliente
					produtos[i].setRAM(
							product["id"].as<uint8_t>(),
							product["name"].as<String>(),
							product["withdrawn_code"].as<String>(), // Aqui você pode escolher qual código usar
							product["delivery_code"].as<String>(),	// Aqui você pode escolher qual código usar
							product["drawer_id"].as<uint8_t>());

					// Serial.println("Informações dos produtos obtidas.");
					// Serial.println("ID: " + String(produtos[i].getId()));
					// Serial.println("Nome: " + produtos[i].getNome());
					// Serial.println("PassCode Entregador: " + produtos[i].getDeliveryCode());
					// Serial.println("PassCode Cliente: " + produtos[i].getWithdrawnCode());
					// Serial.println("DrawerID: " + String(produtos[i].getDrawerID()));
				}
			}
			else
			{
				// Serial.print("Falha na desserialização: ");
				// Serial.println(error.c_str());
			}
		}
		else
		{
			// Serial.print("Erro na conexão HTTP: ");
			// Serial.println(httpCode);
		}
	}
	else
	{
		// Serial.print("Erro na conexão HTTP: ");
		// Serial.println(httpCode);
	}

	http.end();
}

void updateAPI(uint8_t id)
{
	if (!wificonnect && !WiFi.isConnected())
		return;

	// Inicializa o objeto JSON e monta o payload
	DynamicJsonDocument jsonDoc(512);
	String payload;

	jsonDoc["id"] = produtos[id].getId();
	jsonDoc["inserted_date"] = produtos[id].getTimeInserted();
	jsonDoc["withdrawn_date"] = produtos[id].getTimeWithdrawn();
	jsonDoc["opening_date"] = produtos[id].getTimeOpening();
	jsonDoc["closing_date"] = produtos[id].getTimeClosing();

	// Serializa o objeto JSON para string
	serializeJson(jsonDoc, payload);

	// Inicializa o cliente HTTP
	HTTPClient http;
	WiFiClient client; // Usa WiFiClient para HTTP simples

	// Monta a URL usando o IP do servidor e a porta
	String url = String("http://") + serverIP + ":" + String(serverPort) + "/api/product";
	http.begin(client, url); // Usa o cliente HTTP com a URL construída
	// Serial.println("Update no link: " + url);

	// Adiciona cabeçalho de tipo de conteúdo
	http.addHeader("Content-Type", "application/json");

	// Envia a requisição HTTP PUT com o payload JSON
	int httpResponseCode = http.PUT(payload);

	// Exibe o payload no console para debug
	// Serial.println("Payload enviado:");
	// Serial.println(payload);

	// Verifica o código de resposta e imprime o resultado
	if (httpResponseCode > 0)
	{
		String response = http.getString(); // Obtém a resposta da API
		// Serial.println("Resposta da API:");
		// Serial.println(response);
	}
	else
	{
		// Serial.printf("Erro no updateAPI: %d\n", httpResponseCode);
	}

	// Finaliza a conexão HTTP
	http.end();
}

void updateTime()
{
	if (!wificonnect && !WiFi.isConnected())
		return;

	HTTPClient http;
	const char *timeUrl = "https://worldtimeapi.org/api/timezone/America/Sao_Paulo";

	http.begin(timeUrl);
	int httpCode = http.GET();

	if (httpCode == HTTP_CODE_OK)
	{
		String payload = http.getString();
		esp_task_wdt_reset(); // Reseta o watchdog timer

		// Encontre o índice da data e hora na resposta JSON
		int dtIndex = payload.indexOf("\"datetime\":\"") + 12;
		if (dtIndex > 12)
		{
			int dtEndIndex = payload.indexOf("\"", dtIndex);
			String datetime = payload.substring(dtIndex, dtEndIndex);

			// Converta a string de data e hora para o formato usado pela função mktime
			datetime.replace("T", " ");
			datetime.replace("Z", "");

			struct tm timeinfo;
			strptime(datetime.c_str(), "%Y-%m-%d %H:%M:%S", &timeinfo);
			time_t t = mktime(&timeinfo);

			timeval now = {.tv_sec = t};
			settimeofday(&now, NULL);
			// Serial.println("Tempo atual: " + getTime());
		}
	}
	else
	{
		// Serial.println("Erro ao obter o tempo.");
		WiFi.disconnect(); // Acelerando a reconexão
		WiFi.channel(1);
	}
	http.end();
}

String getTime()
{
	if (!wificonnect && !WiFi.isConnected())
		return "00/00/00 00:00:00"; // Criar um relógio virtual futuramente

	time_t now = time(nullptr);
	struct tm *timeinfo = localtime(&now);

	// Defina um buffer com tamanho suficiente
	char timeString[20];
	strftime(timeString, sizeof(timeString), "%d/%m/%y %H:%M:%S", timeinfo);

	// escreverStringNaEEPROM(DATE_TIME_ADDRESS, timeString, DATE_TIME_LENGHT);
	return String(timeString);
}

//-------------------------------------------Tasks de tempo--------------------------------------------------

void myDelayMillis(uint16_t tempo)
{
	uint32_t startTime = millis(); // Marca o tempo atual para o delay

	while ((millis() - startTime) < tempo)
		;
}

// Handle server
void task_10ms()
{
	server.handleClient(); // Manipula clientes conectados na Web
}

void task_500ms()
{
	if (!digitalRead(END_COURSE1) && !drawer1Opened)
	{
		updateLCD("Gaveta " + String(1) + " aberta!", 18, 32);
		drawer1Opened = true; // Se o fim de curso fechou contato, então a gaveta fechou
		produtos[0].setTimeOpening(getTime());
	}
	else if (digitalRead(END_COURSE1) && drawer1Opened)
	{
		updateLCD("Gaveta " + String(1) + " fechada!", 18, 32);
		drawer1Opened = false;
		produtos[0].setTimeClosing(getTime());
		digitalWrite(BEEP, 0);
	}

	if (!digitalRead(END_COURSE2) && !drawer2Opened)
	{
		updateLCD("Gaveta " + String(2) + " aberta!", 18, 32);
		drawer2Opened = true; // Se o fim de curso fechou contato, então a gaveta fechou
	}
	else if (digitalRead(END_COURSE2) && drawer2Opened)
	{
		updateLCD("Gaveta " + String(2) + " fechada!", 18, 32);
		drawer2Opened = false;
		produtos[1].setTimeClosing(getTime());
	}
}

// WDT, Sensores, API, beep e desligar LCD
void task_1000ms()
{
	// Dando problemas ao chamar a função diretamente
	if (atualizaAPI1)
	{
		updateAPI(0);
		atualizaAPI1 = false;
	}
	if (atualizaAPI2)
	{
		updateAPI(1);
		atualizaAPI2 = false;
	}

	if (statePair > 0)
	{
		if (wificonnect && (WiFi.status() != WL_CONNECTED))
		{
			setupWifi();
			wificonnect = false;

			// Despareando e pareando pois quando conecta ao WiFi, o canal muda
			if (WiFi.isConnected())
			{
				jaConectouUmaVez = true;
				delPeerAndPair();
			}
		}

		keepAlive++;
		if (keepAlive >= TIMEOUT_KEEPALIVE && !wificonnect && conetado)
		{
			// Serial.println("Perda de comunicação com o ESP32-CAM, reiniciando...");
			ESP.restart();
		}
	}

	contadorAPI++;
	if (contadorAPI >= (TIME_UPDT_API))
	{
		if (WiFi.isConnected())
		{
			updateTime();
			contadorAPI = 0;
			getAPI();
		}
	}

	contadorLCD++;
	if (contadorLCD >= TIME_TURNOFF_LCD)
	{
		display.clearDisplay();
		display.display();
		contadorLCD = 0;
	}

	if (drawer1Opened)
	{
		contadorBeep++;
		if (contadorBeep >= TIMEOUT_BUZZER)
		{
			// ativa o buzzer
			// digitalWrite(BEEP, 1);
			contadorBeep = 0;
		}
	}

	contadorWifi++;
	if (contadorWifi >= TIME_WIFI)
	{
		if (!WiFi.isConnected() && statePair > 0)
		{
			mySendEspNow("tentaConectar");
			wificonnect = true;
		}
		contadorWifi = 0;
	}

	esp_task_wdt_reset(); // Reseta o watchdog timer

	sensorReader(TRIG_PIN1, ECHO_PIN1, 1, drawer1Opened);
	sensorReader(TRIG_PIN2, ECHO_PIN2, 2, drawer2Opened);
}