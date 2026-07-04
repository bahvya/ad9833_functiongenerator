#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include <stdint.h>
#include "ad9833.h"
extern SPI_HandleTypeDef hspi1;

void ad_write(uint16_t word){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, (uint8_t *)&word, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6, GPIO_PIN_SET);
}
void ad_mode_select(uint8_t i){
    if(i==1){
        ad_write(sin);
    }
    else if (i==2) {
        ad_write(tri);
    
    }
    else if (i==3) {
        ad_write(sqr);
    }
    else {
    i=1;
    }
}
void ad_frequency(float freq)
{
    const float ftw_per_hz = 268435456.0f / 25000000;
    uint32_t freqword= (uint32_t)(freq * ftw_per_hz);
    uint16_t lsb = freqword & 0x3FFF;          // Bits 13:0
    uint16_t msb = (freqword >> 14) & 0x3FFF;  // Bits 27:14
    uint16_t freq0_lsb = 0x4000 | lsb;
    uint16_t freq0_msb = 0x4000 | msb;
    ad_write(freq0_lsb);
    ad_write(freq0_msb);
    ad_write(0xC000); // PHASE0 = 0

}
void ad_setup(void)
{
    ad_write(0x2100);    // RESET=1, B28=1
    ad_frequency(1);
    ad_write(0x2000);    // RESET=0, sine
    
}