#include "xc.h"
#include "libpic30.h"
#include "adc.h"
#include "buttons.h"

#pragma config POSCMOD = XT
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSDCMD
#pragma config FNOSC = PRI
#pragma config IESO = ON

#pragma config WDTPS = PS32768
#pragma config FWPSA = PR128
#pragma config WINDIS = ON
#pragma config FWDTEN = ON
#pragma config ICS = PGx2
#pragma config GWRP = OFF
#pragma config GCP = OFF
#pragma config JTAGEN = OFF

void init(void) {
    ADC_SetConfiguration(ADC_CONFIGURATION_DEFAULT);
    ADC_ChannelEnable(ADC_CHANNEL_POTENTIOMETER);
    TRISA = 0x0000;
    LATA = 0x0000;
    BUTTON_Enable(BUTTON_S3);
}

int main(void) {
    unsigned int adc_value;
    unsigned int alarm_threshold = 1023 / 2;
    unsigned char alarm_on = 0;
    unsigned char poprzedni_stan_przycisku = BUTTON_IsPressed(BUTTON_S3) ? 0 : 1;
    unsigned char stan_przycisku;

    init();

    while (1) {
        adc_value = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);

        if (adc_value == 0xFFFF) {
            __delay32(400);
            continue;
        }

        stan_przycisku = BUTTON_IsPressed(BUTTON_S3) ? 0 : 1;

        if (poprzedni_stan_przycisku == 1 && stan_przycisku == 0) {
            __delay32(40000);
            
            if (BUTTON_IsPressed(BUTTON_S3)) {
                alarm_on = 0;
                LATA = 0x0000;
            }
        }
        poprzedni_stan_przycisku = stan_przycisku;

        if (!alarm_on && adc_value > alarm_threshold) {
            alarm_on = 1;
            unsigned char sequence_interrupted = 0;
             
            for (int i = 0; i < 8; i++) {
                LATA = 0x0001;
                for (int chunk = 0; chunk < 25; chunk++) {
                    __delay32(50000);
                    unsigned int current_adc_check = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
                    if (current_adc_check != 0xFFFF) {
                        if (current_adc_check < alarm_threshold) {
                            LATA = 0x0000;
                            alarm_on = 0;
                            sequence_interrupted = 1;
                            break;
                        }
                    }
                    if (BUTTON_IsPressed(BUTTON_S3)) {
                         __delay32(40000);
                         if (BUTTON_IsPressed(BUTTON_S3)) {
                             LATA = 0x0000; alarm_on = 0; sequence_interrupted = 1;__delay32(4000000); break;
                         }
                    }
                }
                if (sequence_interrupted) break;

                LATA = 0x0000;
                for (int chunk = 0; chunk < 25; chunk++) {
                    __delay32(50000);
                    unsigned int current_adc_check = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
                    if (current_adc_check != 0xFFFF) {
                        if (current_adc_check < alarm_threshold) {
                            alarm_on = 0;
                            sequence_interrupted = 1;
                            break;
                        }
                    }
                     if (BUTTON_IsPressed(BUTTON_S3)) {
                         __delay32(40000);
                         if (BUTTON_IsPressed(BUTTON_S3)) {
                             LATA = 0x0000; alarm_on = 0; sequence_interrupted = 1; break;
                         }
                    }
                }
                if (sequence_interrupted) break;
            }

            if (sequence_interrupted) {
                LATA = 0x0000;
                alarm_on = 0;
            } else {
                unsigned int final_adc_check = ADC_Read10bit(ADC_CHANNEL_POTENTIOMETER);
                if (final_adc_check != 0xFFFF && final_adc_check >= alarm_threshold) {
                    LATA = 0xFFFF;
                } else {
                    LATA = 0x0000;
                    alarm_on = 0;
                }
            }
        } else if (alarm_on && adc_value < alarm_threshold) {
            alarm_on = 0;
            LATA = 0x0000;
        }
        
        __delay32(1000);
    }

    return 0;
}