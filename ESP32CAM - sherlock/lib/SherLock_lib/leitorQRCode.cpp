#include "leitorQRCode.h"

// Criação de um handle para a tarefa
TaskHandle_t QRCodeReader_Task;

// Estrutura de dados do QR Code
struct quirc *q = NULL;
uint8_t *image = NULL;
camera_fb_t *fb = NULL;
struct quirc_code code;
struct quirc_data data;
quirc_decode_error_t err;
struct QRCodeData qrCodeData;
String QRCodeResult = "";

void setupCamera()
{
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.frame_size = FRAMESIZE_QVGA;
  config.xclk_freq_hz = 20000000;
  config.jpeg_quality = 1;
  config.fb_count = 1;

  // Inicializando câmera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    // Serial.printf("Falha na inicialização da câmera com erro 0x%x", err);
    ESP.restart();
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_framesize(s, FRAMESIZE_QVGA);
}

void createQRCodeReaderTask()
{
  xTaskCreatePinnedToCore(
      QRCodeReader,        // Função da tarefa
      "QRCodeReader_Task", // Nome da tarefa
      10000,               // Tamanho da pilha da tarefa
      NULL,                // Parâmetro da tarefa
      1,                   // Prioridade da tarefa
      &QRCodeReader_Task,  // Handle da tarefa
      0                    // Fixar a tarefa no núcleo 0
  );
}

void QRCodeReader(void *pvParameters)
{
  while (1)
  {
    processQRCode();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void processQRCode()
{
  q = quirc_new();
  if (q == NULL)
  {
    // Serial.print("Não foi possível criar o objeto quirc\r\n");
    return;
  }

  fb = esp_camera_fb_get();
  if (!fb)
  {
    // Serial.println("Falha na captura da câmera");
    return;
  }

  quirc_resize(q, fb->width, fb->height);
  image = quirc_begin(q, NULL, NULL);
  memcpy(image, fb->buf, fb->len);
  quirc_end(q);

  int count = quirc_count(q);
  if (count > 0)
  {
    quirc_extract(q, 0, &code);
    err = quirc_decode(&code, &data);
    handleDecodeResult(err == QUIRC_SUCCESS);
  }

  esp_camera_fb_return(fb);
  fb = NULL;
  image = NULL;
  quirc_destroy(q);
}

void handleDecodeResult(bool success)
{
  if (success)
  {
    CheckPassCode(&data);
  }
}