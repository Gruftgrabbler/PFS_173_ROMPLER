#include "sinetest.h"
#include <stdint.h>
#include <stdio.h>
#include <easypdk/pdk.h>
#include "sample.h"
#include "bitmanipulation.h"

#define DAC_BCK_PIN 3           // PA3
#define DAC_DATA_PIN 4          // PA4
#define DAC_WORD_SELECT_PIN 0   // PA0

volatile uint16_t i;

unsigned char _sdcc_external_startup(void){
    EASY_PDK_INIT_SYSCLOCK_8MHZ();                //use 8MHz sysclock
    EASY_PDK_USE_FACTORY_IHRCR_16MHZ();            //use factory IHCR tuning value (tuned for 8MHz SYSCLK @ 5.0V)
    EASY_PDK_USE_FACTORY_BGTR();                  //use factory BandGap tuning value (tuned @ 5.0V)
    return 0;                                     //perform normal initialization
}

void init_timer(){
    // frequency_timer2 = clock_source / (prescale * scale * (bound_register+1))
    // frequency_timer2 = 16Mhz / (1 * 2 * 69) = 115942 Hz = 116kHz

    // TM2B = freq_clock / (pre_scale * scale * (tone_freq * interrupts_per_cycle) -1)
    // TM2B = 8Mhz / (1 * 1 (440Hz * 128) -1) = 141

    // Measurement: max tone_freq ~= 90Hz (For TM2B <= 180)
    // Ich konnte mit dem Code eine Sinus Welle erzeugen, ich verstehe allerdings noch nicht wie ich die Frequenz wirklich kontrollieren kann

    // Preclar has to be 4:
    // TM2B = 8Mhz/ (4 * (90 * 128) -1) = 172   // which worked

    TM2C = TM2C_CLK_IHRC;                         //use IHRC -> 16 Mhz, clock_source = 16Mhz
    //TM2S = TM2S_PRESCALE_NONE | TM2S_SCALE_NONE;  // no prescale, no scale
    TM2S = TM2S_SCALE_DIV4 | TM2S_SCALE_NONE;  // prescale = 4, no scale
    TM2B = 179;
    INTEN = INTEN_TM2;                            //enable TM2 interrupt, send out initial stop bits and autobaud char
}

void setup(){
    set_bit(PAC, DAC_BCK_PIN);          // pinMode(DAC_BCK_PIN, OUTPUT)
    set_bit(PAC, DAC_WORD_SELECT_PIN);  // pinMode(DAC_WORD_SELECT_PIN, OUTPUT)
    set_bit(PAC, DAC_DATA_PIN);         // pinMode(DAC_DATA_PIN, OUTPUT)
    i = 0;

    init_timer();
    __engint();
}

void main(void){
    setup();

    while (1) {
    }
}

void shiftOUTMSBFirst(uint16_t val){
    uint8_t bit;
    for (bit = 0;  bit < 8; bit++) {
        uint8_t curr = !!(val & (1 << (7 - bit)));

        if (curr)
            set_bit(PA, DAC_DATA_PIN);
        else
            clear_bit(PA, DAC_DATA_PIN);

        set_bit(PA, DAC_BCK_PIN);
        clear_bit(PA, DAC_BCK_PIN);
    }
}

void shiftDACOut(){
    uint8_t curr = sample_data[i];
    // Left Channel
    shiftOUTMSBFirst(curr);

    // Right Channel
    set_bit(PA, DAC_WORD_SELECT_PIN);
    clear_bit(PA, DAC_WORD_SELECT_PIN);

    i++;
    i %= SAMPLE_LEN;
}

void interrupt(void) __interrupt(0){
    if(INTRQ & INTRQ_TM2){
        shiftDACOut();
        INTRQ &= ~INTEN_TM2;
    }
}

