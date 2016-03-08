/*
Benchmark routines for testing the capabilites of STM32F4 and STM32F7 MCUs
*/

#include <stdint.h>
#include "dhry.h"
#include "fft_benchmark.h"
#include "whetstone.h"
#include "graphics.h"

char txt[10];
uint32_t dhrystone_time, whetstone_time, graphics_time,fft_time;
uint32_t volatile timer;
enum
{
    DHRYSTONE,
    FFT,
    WHETSTONE,
    GRAPHICS,
    STOP
}state_e;

void Timer2_interrupt() iv IVT_INT_TIM2 {
  TIM2_SR.UIF = 0;
  timer++;
}

void reset_time()
{
  timer = 0;
}

void record_time(state_e)
{
  switch(state_e)
  {
    case DHRYSTONE:
      dhrystone_time += timer;
      break;
    case FFT:
      fft_time += timer;
    case WHETSTONE:
      whetstone_time += timer;
      break;
      case GRAPHICS:
      graphics_time += timer;
      break;
      default:
        break;
  }
}

void timer_setup()
{
  RCC_APB1ENR.TIM2EN = 1;
  TIM2_CR1.CEN = 0;
  TIM2_PSC = 24;
  TIM2_ARR = 55999;
  NVIC_IntEnable(IVT_INT_TIM2);
  TIM2_DIER.UIE = 1;
  TIM2_CR1.CEN = 1;
}

void main() {
  int i,j;

  UART1_Init(57600);              
  Delay_ms(100);                  

  UART1_Write_Text("Start\n");
  UART1_Write(13);
  UART1_Write(10);
  timer_setup();
  UART1_Write_Text("timer started");
  UART1_Write(13);
  UART1_Write(10);

for( i = STOP; i > state_e; state_e++ )  // every state is being executed 1000 times except GRAPHICS, it is done 10 times
  {
    for( j = 0; j < 10000; j++ )
    {
      switch( state_e )
      {
        case DHRYSTONE:
          dhrystone_benchmark();    // after executing, record the time passed
          record_time();
          reset_time();
        break;
        case FFT:
          fft_benchmark();
          record_time();
          reset_time();
          break;
        case WHETSTONE:
          whetstone_benchmark();
          record_time();
          reset_time();
          timer_setup();
        break;
        case GRAPHICS:
        {  uint8_t temp;
          if ( j % 100 == 0)
          {
            for (temp = 0; temp <10; temp++)
            {
              graphics_benchmark();
              record_time();
              reset_time();
             }
          }
         }
        break;
      }
    }
  }

  // print out the time passed

  UART1_Write_Text("Drhystone: ");
  IntToStr(dhrystone_time, txt);
  UART1_Write_Text(txt);
  UART1_Write(13);
  UART1_Write(10);

  UART1_Write_Text("FFT: ");
  IntToStr(fft_time, txt);
  UART1_Write_Text(txt);
  UART1_Write(13);
  UART1_Write(10);

  UART1_Write_Text("whetstone: ");
  IntToStr(whetstone_time, txt);
  UART1_Write_Text(txt);
  UART1_Write(13);
  UART1_Write(10);

  UART1_Write_Text("graphics: ");
  IntToStr(graphics_time, txt);
  UART1_Write_Text(txt);
  UART1_Write(13);
  UART1_Write(10);



}