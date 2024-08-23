#ifndef __leitorQRCode__
#define __leitorQRCode_H__

#include "esp_camera.h"
#include "quirc.h"
#include <globais.h>

#define LED_PIN 4 // Led do ESP32CAM

//------------------------------------------------------CÂMERA-------------------------------------------

// Definição dos pinos da câmera
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

//------------------------------------------------------QRCODE-------------------------------------------
struct QRCodeData
{
  bool valid;
  int dataType;
  uint8_t payload[1024];
  int payloadLen;
};

extern TaskHandle_t QRCodeReader_Task;
extern struct QRCodeData qrCodeData;
extern String QRCodeResult;

//----------------------------------------------------PROTÓTIPOS-----------------------------------------

// Inicializa a câmera
void setupCamera();

// Task para capturar QR Code
void createQRCodeReaderTask();

// Função executada pela tarefa "QRCodeReader_Task"
void QRCodeReader(void *pvParameters);

// Verifica o conteúdo (passCode) do QR Code e compara com os salvos na RAM
void CheckPassCode(const struct quirc_data *data);

// Processa o que tem no QR Code (decodifica)
void processQRCode();

// Manipula o resultado da decodificação
void handleDecodeResult(bool success);

#endif