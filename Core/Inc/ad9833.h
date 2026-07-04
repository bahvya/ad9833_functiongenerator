#ifndef AD9833_H
#define AD9833_H


#include <stdint.h>
void ad_write(uint16_t word);
void ad_setup(void);
void ad_mode_select(uint8_t i);
void ad_frequency(float freq);
#define sin 0x2000
#define tri 0x2002
#define sqr 0x2028



#endif /* AD9833_H */




/*

| Waveform     | Control Word |
| ------------ | -----------: |
| Sine         |     `0x2000` |
| Triangle     |     `0x2002` |
| Square (MSB) |     `0x2028` |


*/