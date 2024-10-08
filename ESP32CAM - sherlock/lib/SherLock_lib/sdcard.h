#ifndef __sdcard__
#define __sdcard__

#include "FS.h"     // SD Card ESP32
#include "SD_MMC.h" // SD Card ESP32
#include "time.h"
#include "soc/soc.h"
#include "driver/rtc_io.h"
#include <globais.h>
#include <WiFi.h>

//----------------------------------------------------PROTÓTIPOS-----------------------------------------

//***************************************************************************************************
// Function  : Faz o setup do cartão SD
// Arguments : none
// Return    : none
//***************************************************************************************************
void setupSDCard(void);

//***************************************************************************************************
// Function  : Tira uma foto e salva no cartão SD
// Arguments : O trem pra salvar
// Return    : none
//***************************************************************************************************
void SaveSDCard(String info);

#endif