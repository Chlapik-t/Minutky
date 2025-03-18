#include "main.h"
#include "max7219.h"
#include "milis.h"
#include <stdbool.h>
#include <stm8s.h>

/*Enkoder Makra*/
#define NCCLK_PORT GPIOB
#define NCCLK_PIN GPIO_PIN_5
#define NCDATA_PORT GPIOB
#define NCDATA_PIN GPIO_PIN_4
#define NCSW_PORT GPIOB
#define NCSW_PIN GPIO_PIN_3

/*max Makra*/
#define CLK_GPIO GPIOB     
#define CLK_PIN GPIO_PIN_0  
#define DATA_GPIO GPIOB     
#define DATA_PIN GPIO_PIN_2 
#define CS_GPIO GPIOB       
#define CS_PIN GPIO_PIN_1

/*Makra na funkce*/
#define CLK_HIGH GPIO_WriteHigh(CLK_GPIO, CLK_PIN)
#define CLK_LOW GPIO_WriteLow(CLK_GPIO, CLK_PIN)
#define DATA_HIGH GPIO_WriteHigh(DATA_GPIO, DATA_PIN)
#define DATA_LOW GPIO_WriteLow(DATA_GPIO, DATA_PIN)
#define CS_HIGH GPIO_WriteHigh(CS_GPIO, CS_PIN)
#define CS_LOW GPIO_WriteLow(CS_GPIO, CS_PIN)
;
void max7219_send(uint8_t address, uint8_t data)
{
    uint8_t mask; 
    CS_LOW;       

    
    mask = 0b10000000; 
    CLK_LOW;          
    while (mask)
    { 
        if (mask & address)
        {              
            DATA_HIGH;
        }
        else
        {             
            DATA_LOW; 
        }
        CLK_HIGH;         
        mask = mask >> 1; 
        CLK_LOW;          
    }

   
    mask = 0b10000000;
    while (mask)
    {
        if (mask & data)
        {              
            DATA_HIGH; 
        }
        else
        {             
            DATA_LOW;
        }
        CLK_HIGH;         
        mask = mask >> 1;
        CLK_LOW;          
    }

    CS_HIGH; 
}

void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // taktovani MCU na 16MHz

    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(BTN_PORT, BTN_PIN, GPIO_MODE_IN_FL_NO_IT);

    GPIO_Init(DIN_PORT, DIN_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(CS_PORT,  CS_PIN,  GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(CLK_PORT, CLK_PIN, GPIO_MODE_OUT_PP_LOW_FAST);

    GPIO_Init(NCCLK_PORT, NCCLK_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(NCDATA_PORT, NCDATA_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(NCSW_PORT, NCSW_PIN, GPIO_MODE_IN_PU_NO_IT);

    init_milis();
}
void max7219_init(void)
{   /* Nastavení pinu do začáteční pozice */
    GPIO_Init(CS_GPIO, CS_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(CLK_GPIO, CLK_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(DATA_GPIO, DATA_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    // nastavíme základní parametry budiče
    max7219_send(DECODE_MODE, DECODE_ALL);        // Nějakej dekodér
    max7219_send(SCAN_LIMIT, 3);                  // Kolik cifer zapneme
    max7219_send(INTENSITY, 5);                   // Jas
    max7219_send(DISPLAY_TEST, DISPLAY_TEST_OFF); // test displeje
    max7219_send(SHUTDOWN, SHUTDOWN_ON);           // On/OFF

}

uint8_t minutes1 = 0;
uint8_t seconds1 = 0;
uint8_t minutes2 = 0;
uint8_t seconds2 = 0;

int check_ncoder(void)
{
    static bool last = 0;
    int8_t r = 0;

    bool now = (bool)PUSH(NCCLK);
    if (last && !now) {
        if ((bool)PUSH(NCDATA))
            r = 1;
        else
            r = -1;
    }
    if (!last && now) {
        if ((bool)PUSH(NCDATA))
            r = -1;
        else
            r = 1;
    }
    last = now;
    return r;
}

void show_dispaly(void)
{
    max7219_send(DIGIT7, minutes2 / 10);
    max7219_send(DIGIT6, (minutes2 % 10) | 0xF0);
    max7219_send(DIGIT5, seconds2 / 10);
    max7219_send(DIGIT4, seconds2 % 10);

    max7219_send(DIGIT3, minutes1 / 10);
    max7219_send(DIGIT2, (minutes1 % 10) | 0xF0);
    max7219_send(DIGIT1, seconds1 / 10);
    max7219_send(DIGIT0, seconds1 % 10);
}

int main(void)
{
    uint32_t time = 0;
    uint32_t time_check = 0;

    init();
    max7219_init();

    while (1) {
        if (milis() - time > 111) {
            time = milis();

            REVERSE(LED);

            show_dispaly();
            minutes1++;

        }
        if (milis() - time_check > 3) {
            time_check = milis();

            minutes2 += check_ncoder();
        }
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
