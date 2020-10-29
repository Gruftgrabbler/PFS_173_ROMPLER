#include <stdint.h>
#include <stdio.h>
#include <easypdk/pdk.h>
#include "sample.h"
#include "bitmanipulation.h"

// Output pin numeration - PORT A
#define DAC_BCK_PIN 3           // PA3
#define DAC_DATA_PIN 4          // PA4
#define DAC_WORD_SELECT_PIN 0   // PA0
#define LED_PIN 6               // PA6

// Output pin numeration - PORT B
#define BUTTON_PIN 0            // PB0
#define ENCODER_PIN 1           // PB1

// DAC
volatile uint16_t i;
volatile uint8_t startDAC;
volatile uint8_t stopDAC;

// Pitch Shift
int8_t enc;
uint8_t tm2b;

// Play at button Press
uint8_t button_last_state;
uint8_t button_curr_state;

unsigned char _sdcc_external_startup(void){
    // TODO Refactor this to match the Makefile
    EASY_PDK_INIT_SYSCLOCK_8MHZ();                  //use 8MHz sysclock
    EASY_PDK_USE_FACTORY_IHRCR_16MHZ();             //use factory IHCR tuning value (tuned for 8MHz SYSCLK @ 5.0V)
    EASY_PDK_USE_FACTORY_BGTR();                    //use factory BandGap tuning value (tuned @ 5.0V)
    return 0;                                       //perform normal initialization
}

void init_adc(){
    // Setup ADC for analog input conversion
    clear_bit(PBC, ENCODER_PIN);                // disable PB.1 GPIO output
    clear_bit(PBPH, ENCODER_PIN);               // disable pull up on PB.1 NOTE: also disable PxPL if available (e.g. on port B)
    clear_bit(PBDIER, ENCODER_PIN);             // disable PB.1 GPIO input

    // Setup ADC Conversion
    ADCRGC = ADCRG_ADC_REF_VDD;                   //ADC reference voltage is VDD
    ADCM = ADCM_CLK_SYSCLK_DIV16;                 //SYSCLK 8MHZ/32 -> 250 kHz ADC clock
    ADCC = ADCC_ADC_ENABLE | ADCC_CH_AD1_PB1;   // enable ADC and use channel 1 (PB.1)
    //NOTE: a delay of 400usec is required after initialization, before first ADC conversion can start
}

void init_timer(){
    // frequency_timer2 = clock_source / (prescale * scale * (TM2B + 1))
    // TM2B = freq_clock / (pre_scale * scale * (frequency_timer2) -1)
    // TM2B = freq_clock / (pre_scale * scale * (tone_freq * interrupts_per_cycle) -1)
    // TM2B = 8Mhz / (1 * 1 (440Hz * 128) -1) = 141
    TM2C = TM2C_CLK_IHRC;                         //use IHRC -> 16 Mhz, clock_source = 16Mhz
    TM2S = TM2S_PRESCALE_NONE | TM2S_SCALE_NONE;  // no prescale, no scale
    tm2b = 141;
    TM2B = tm2b;
    INTEN = INTEN_TM2;                            //enable TM2 interrupt, send out initial stop bits and autobaud char
}

void setup(){
    set_bit(PAC, DAC_BCK_PIN);          // pinMode(DAC_BCK_PIN, OUTPUT)
    set_bit(PAC, DAC_WORD_SELECT_PIN);  // pinMode(DAC_WORD_SELECT_PIN, OUTPUT)
    set_bit(PAC, DAC_DATA_PIN);         // pinMode(DAC_DATA_PIN, OUTPUT)

    set_bit(PAC, LED_PIN);              // pinMode(LED_PIN, OUTPUT)

    clear_bit(PBC, BUTTON_PIN);         // pinMode(BUTTON_PIN; INPUT)
    set_bit(PBDIER, BUTTON_PIN);        // Enable digital input for BUTTON_PIN
    set_bit(PBPH, BUTTON_PIN);          // Enable Pull-Up Resistor


    i = 0;
    startDAC = 0;
    stopDAC = 1;

    button_last_state = 0;
    button_curr_state = 0;

    init_adc();
    init_timer();
    __engint();                         // Enable global interrupt
}

uint8_t readADC(){
    ADCC |= ADCC_ADC_CONV_START;                  // start ADC conversion
    while( !(ADCC & ADCC_ADC_CONV_COMPLETE) );    // busy wait for ADC conversion to finish (we also could use the ADC interrupt...)
    return ADCR;                                  // read the ADC value
}

void readButton(){
    if(!!stopDAC && !!!startDAC) {
       // button_curr_state = !(PB & (1 << BUTTON_PIN));
        button_curr_state = (PB & (1 << BUTTON_PIN));
        if((button_curr_state) != button_last_state){
            if(button_curr_state){


                enc = readADC() >> 1;           // Decrease the original 8Bit ADC resolution down to 7Bit (128Steps)
                enc -= 64;                      // Center the encoder position between [-64, 64]
                TM2B = tm2b + enc;              // Modify the playback pitch by adjusting the interrupt frequency

                startDAC = 1;
                stopDAC = 0;
                set_bit(PA, LED_PIN);           // Turns on the LED to indicate that a sample is played
            }
        }
        button_last_state = button_curr_state;
    }
}


void main(void){
    setup();

    while (1) {
        readButton();
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

uint8_t getData(){
    if (!!startDAC && !!!stopDAC){
        return sample_data[i];
    }
    return 0;
}

void shiftDACOut(){
    uint8_t curr = getData();
    // Left Channel
    shiftOUTMSBFirst(curr);

    // Right Channel
    set_bit(PA, DAC_WORD_SELECT_PIN);
    clear_bit(PA, DAC_WORD_SELECT_PIN);

    if(!!startDAC && !!!stopDAC) i++;   // Only increase counter when dac is actually emitting samples data

    if(i >= SAMPLE_LEN) {
        startDAC = 0;
        stopDAC = 1;
        i = 0;
        clear_bit(PA, LED_PIN);         // Turns off the LED to indicate that transmission is done
    }
}

void interrupt(void) __interrupt(0){
    if(INTRQ & INTRQ_TM2){
        shiftDACOut();
        INTRQ &= ~INTEN_TM2;
    }
}