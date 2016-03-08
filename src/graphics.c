// Microcontroller graphic demo by Pascal from Serveurperso.com
// admin@serveurperso.com http://www.serveurperso.com

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#include "lpc177x_8x_timer.h"
//#include "lpc177x_8x_i2c.h"
//#include "joystick.h"
//#include "pca9532.h"
//#include "board.h"
//#include "sdram.h"

//#include "ea_lcd_board.h"
//#include "lpc_swim.h"

#define WINDOWX 800
#define WINDOWY 480

// INCREMENT = SCALE / sqrt(N) * 2
// Optimizer-friendly values
#define N 1024 // Number of dots
#define SCALE 8192
#define INCREMENT 512

#define SPEED 10

#define PI2 6.283185307179586476925286766559

static char buff[512];

//SWIM_WINDOW_T win1;

int16_t sine[SCALE];
int16_t cosi[SCALE];

void initialize() {
 uint16_t i;
 for(i = 0; i < SCALE; i++) {
  sine[i] = (int)(sin(PI2 * i / SCALE) * SCALE);
  cosi[i] = (int)(cos(PI2 * i / SCALE) * SCALE); // FIXME cos(x) = sin(pi / 2 + x)
 }
}

uint16_t fastsqrt(uint32_t n) {
 uint16_t c = 0x8000;
 uint16_t g = 0x8000;
 for(;;) {
  if(g * g > n)
   g ^= c;
  c >>= 1;
  if(c == 0)
   return g;
  g |= c;
 }
}

/*void matrix(int16_t xyz[3][N], uint8_t rgb[3][N]) {
 static uint32_t t = 0;
 uint16_t i;
 int16_t x = -SCALE;
 int16_t y = -SCALE;
 uint16_t d;
 uint16_t s;

 for(i = 0; i < N; i++) {

  xyz[0][i] = x;
  xyz[1][i] = y;

  d = fastsqrt(x * x + y * y);
  s = sine[(t * 30) % SCALE] + SCALE;

  xyz[2][i] = sine[(d + s) % SCALE] *
              sine[(t * 10) % SCALE] / SCALE / 2;

  rgb[0][i] = (cosi[xyz[2][i] + SCALE / 2] + SCALE) *
              (RED_COLORS - 1) / SCALE / 2;

  rgb[1][i] = (cosi[(xyz[2][i] + SCALE / 2 + 2 * SCALE / 3) % SCALE] + SCALE) *
              (GREEN_COLORS - 1) / SCALE / 2;

  rgb[2][i] = (cosi[(xyz[2][i] + SCALE / 2 + SCALE / 3) % SCALE] + SCALE) *
              (BLUE_COLORS - 1) / SCALE / 2;

  x += INCREMENT;
  if(x >= SCALE) {
   x = -SCALE;
   y += INCREMENT;
  }

 }
 t++;
}
*/

void rotate(int16_t xyz[3][N], uint8_t rgb[3][N],
            uint16_t angleX, uint16_t angleY, uint16_t angleZ) {
 uint16_t i;
 int16_t tmpX;
 int16_t tmpY;
 int16_t sinx = sine[angleX];
 int16_t cosx = cosi[angleX];
 int16_t siny = sine[angleY];
 int16_t cosy = cosi[angleY];
 int16_t sinz = sine[angleZ];
 int16_t cosz = cosi[angleZ];

 for(i = 0; i < N; i++) {
  tmpX      = (xyz[0][i] * cosx - xyz[2][i] * sinx) / SCALE;
  xyz[2][i] = (xyz[0][i] * sinx + xyz[2][i] * cosx) / SCALE;
  xyz[0][i] = tmpX;

  tmpY      = (xyz[1][i] * cosy - xyz[2][i] * siny) / SCALE;
  xyz[2][i] = (xyz[1][i] * siny + xyz[2][i] * cosy) / SCALE;
  xyz[1][i] = tmpY;

  tmpX      = (xyz[0][i] * cosz - xyz[1][i] * sinz) / SCALE;
  xyz[1][i] = (xyz[0][i] * sinz + xyz[1][i] * cosz) / SCALE;
  xyz[0][i] = tmpX;
 }
}

void draw(int16_t xyz[3][N], uint8_t rgb[3][N]) {
 static uint16_t oldProjX[N] = {0};
 static uint16_t oldProjY[N] = {0};
 static uint8_t oldDotSize[N] = {0};
 uint16_t i;
 uint16_t projX;
 uint16_t projY;
 uint16_t projZ;
 uint16_t dotSize;

 for(i = 0; i < N; i++) {
  projZ   = SCALE - (xyz[2][i] + SCALE) / 4;
  projX   = WINDOWX / 2 + (xyz[0][i] * projZ / SCALE) / 25;
  projY   = WINDOWY / 2 + (xyz[1][i] * projZ / SCALE) / 25;
  dotSize = 3 - (xyz[2][i] + SCALE) * 2 / SCALE;

  if(projX > dotSize &&
     projY > dotSize &&
     projX < WINDOWX - dotSize &&
     projY < WINDOWY - dotSize) {

   oldProjX[i] = projX;
   oldProjY[i] = projY;
   oldDotSize[i] = dotSize;
  }
 }
}


int graphics_benchmark()
{
  int32_t dev_lcd = 0;
 int32_t windowX;
 int32_t windowY;

// int32_t framebuf = (int32_t)(SDRAM_BASE + 0x10000);

 uint8_t joyState;
 uint16_t ledState;

 int16_t angleX;// = 0;

 int16_t angleY = 0;
 int16_t angleZ = 0;

 int16_t speedX = 0;
 int16_t speedY = 0;
 int16_t speedZ = 0;

 int16_t xyz[3][N];
 uint8_t rgb[3][N];

 angleX = 0;
 do {

//  memset((void*)(framebuf), 0x00, windowX * windowY * 2);



  // Create a SWIM window
//  swim_window_open(&win1, windowX, windowY, (COLOR_T*)framebuf, 0, 0,
//                          windowX - 1, windowY - 1, 1, BLACK, BLACK, BLACK);

  initialize();

   rotate(xyz, rgb, angleX, angleY, angleZ);

   draw(xyz, rgb);

   angleX += speedX;
   angleY += speedY;
   angleZ += speedZ;

   if(angleX >= SCALE)
    angleX -= SCALE;
   else if(angleX < 0)
    angleX += SCALE;

   if(angleY >= SCALE)
    angleY -= SCALE;
   else if(angleY < 0)
    angleY += SCALE;

   if(angleZ >= SCALE)
    angleZ -= SCALE;
   else if(angleZ < 0)
    angleZ += SCALE;


 } while(0);

}