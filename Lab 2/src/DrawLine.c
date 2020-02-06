/*
===============================================================================
 Name        : DrawLine.c
 Author      : $RJ
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "LPC17xx.h"                        /* LPC17xx definitions */
#include "ssp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* Be careful with the port number and location number, because

some of the location may not exist in that port. */

#define PORT_NUM            1


uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];


#define ST7735_TFTWIDTH 127
#define ST7735_TFTHEIGHT 159

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_SLPOUT 0x11
#define ST7735_DISPON 0x29



#define swap(x, y) {x = x + y; y = x - y; x = x - y ;}

// defining color values

#define LIGHTBLUE 0x00FFE0
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0x000000
#define BLUE 0x0007FF
#define RED 0xFF0000
#define MAGENTA 0x00F81F
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF
#define BROWN 0xA52A2A
#define YELLOW 0xFFFF00
#define RED1 0xE61C1C
#define RED2 0xEE3B3B
#define RED3 0xEF4D4D
#define RED4 0xE88080
#define GGREEN 0X33FF33
#define GREEN1 0x00CC00
#define GREEN2 0x009900
#define GREEN3 0x006600
#define GREEN4 0x193300


int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;

typedef struct Point
{
	float x;
	float y;
}Point;

void spiwrite(uint8_t c)

{

 int pnum = 1;

 src_addr[0] = c;

 SSP_SSELToggle( pnum, 0 );

 SSPSend( pnum, (uint8_t *)src_addr, 1 );

 SSP_SSELToggle( pnum, 1 );

}



void writecommand(uint8_t c)

{

 LPC_GPIO0->FIOCLR |= (0x1<<21);

 spiwrite(c);

}



void writedata(uint8_t c)

{

 LPC_GPIO0->FIOSET |= (0x1<<21);

 spiwrite(c);

}



void writeword(uint16_t c)

{

 uint8_t d;

 d = c >> 8;

 writedata(d);

 d = c & 0xFF;

 writedata(d);

}



void write888(uint32_t color, uint32_t repeat)

{

 uint8_t red, green, blue;

 int i;

 red = (color >> 16);

 green = (color >> 8) & 0xFF;

 blue = color & 0xFF;

 for (i = 0; i< repeat; i++) {

  writedata(red);

  writedata(green);

  writedata(blue);

 }

}



void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)

{

 writecommand(ST7735_CASET);

 writeword(x0);

 writeword(x1);

 writecommand(ST7735_RASET);

 writeword(y0);

 writeword(y1);

}


void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)

{

 //int16_t i;

 int16_t width, height;

 width = x1-x0+1;

 height = y1-y0+1;

 setAddrWindow(x0,y0,x1,y1);

 writecommand(ST7735_RAMWR);

 write888(color,width*height);

}



void lcddelay(int ms)

{

 int count = 24000;

 int i;

 for ( i = count*ms; i > 0; i--);

}



void lcd_init()

{

 int i;
 printf("LCD Demo Begins!!!\n");
 // Set pins P0.16, P0.21, P0.22 as output
 LPC_GPIO0->FIODIR |= (0x1<<6);

 LPC_GPIO0->FIODIR |= (0x1<<21);

 LPC_GPIO0->FIODIR |= (0x1<<22);

 // Hardware Reset Sequence
 LPC_GPIO0->FIOSET |= (0x1<<22);
 lcddelay(500);

 LPC_GPIO0->FIOCLR |= (0x1<<22);
 lcddelay(500);

 LPC_GPIO0->FIOSET |= (0x1<<22);
 lcddelay(500);

 // initialize buffers
 for ( i = 0; i < SSP_BUFSIZE; i++ )
 {

   src_addr[i] = 0;
   dest_addr[i] = 0;
 }

 // Take LCD display out of sleep mode
 writecommand(ST7735_SLPOUT);
 lcddelay(200);

 // Turn LCD display on
 writecommand(ST7735_DISPON);
 lcddelay(200);

}




void drawPixel(int16_t x, int16_t y, uint32_t color)

{

 if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))

 return;

 setAddrWindow(x, y, x + 1, y + 1);

 writecommand(ST7735_RAMWR);

 write888(color, 1);

}

/*****************************************************************************


** Descriptions:        Draw line function

**

** parameters:           Starting point (x0,y0), Ending point(x1,y1) and color

** Returned value:        None

**

*****************************************************************************/


void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)

{

 int16_t slope = abs(y1 - y0) > abs(x1 - x0);

 if (slope) {

  swap(x0, y0);

  swap(x1, y1);

 }

 if (x0 > x1) {

  swap(x0, x1);

  swap(y0, y1);

 }

 int16_t dx, dy;

 dx = x1 - x0;

 dy = abs(y1 - y0);

 int16_t err = dx / 2;

 int16_t ystep;

 if (y0 < y1) {

  ystep = 1;

 }

 else {

  ystep = -1;

 }

 for (; x0 <= x1; x0++) {

  if (slope) {

   drawPixel(y0, x0, color);

  }

  else {

   drawPixel(x0, y0, color);

  }

  err -= dy;

  if (err < 0) {

   y0 += ystep;

   err += dx;

  }

 }

}

//Rotate point p with respect to o and angle <angle>
Point rotate_point(Point p, Point o, float angle)
{
	Point rt, t, new;

	// Convert to radians
	angle = angle * (3.14285/180);
	float s = sin(angle);
	float c = cos(angle);

	//translate point to origin
	t.x = p.x - o.x;
	t.y = p.y - o.y;

	rt.x = t.x * c - t.y * s;
	rt.y = t.x * s + t.y * c;

	//translate point back
	new.x = rt.x + o.x;
	new.y = rt.y + o.y;

	return new;
}

void tree(Point start, Point end, int level)
{
	Point c, rtl, rtr;
	if(level == 0)
		return;
	int color[] = {GREEN2, GREEN2, GREEN2, GREEN3, GREEN3, GREEN3, GREEN3, GREEN4, GREEN4, GREEN4};

	c.x = end.x + 0.8 * (end.x - start.x);
	c.y = end.y + 0.8 * (end.y - start.y);
	drawLine(c.x, c.y, end.x, end.y, color[level]);
	tree(end, c, level - 1);

	rtl = rotate_point(c, end, 30);
	drawLine(rtl.x, rtl.y, end.x, end.y, color[level]);
	tree(end, rtl, level - 1);

	rtr = rotate_point(c, end, 330);
	drawLine(rtr.x, rtr.y, end.x, end.y, color[level]);
	tree(end, rtr, level - 1);
}

/*

 Main Function main()

*/

void drawTreetwig(Point start, Point end, uint16_t color, uint8_t thickness)
{
	int i;
	for(i = 0; i < thickness; i++)
		drawLine(start.x, start.y + i, end.x, end.y + i, color);
}

int main (void)
{
	Point start, end, new, new_pt;
	uint32_t pnum = PORT_NUM, width = ST7735_TFTWIDTH / 5, len;
	pnum = 1 ;

	srand(time(NULL));
	if ( pnum == 1 )
		SSP1Init();
	else
		 puts("Port number is not correct");

	 lcd_init();


	 fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, WHITE);
	 fillrect(0, 0, width, ST7735_TFTHEIGHT, GGREEN);
	 fillrect(width, 0, width*2, ST7735_TFTHEIGHT, RED1);
	 fillrect(width*2, 0, width*3, ST7735_TFTHEIGHT, RED2);
	 fillrect(width*3, 0, width*4, ST7735_TFTHEIGHT, RED3);
	 fillrect(width*4, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, RED4);

	 //drawLine(0, 0, 50, 50, BLACK);
	 for(int i = 0; i < 60; i++)
	 {
		 start.x = rand() % 70;
		 start.y = rand() % 150;
		 len = rand() % 30;
		 if(len == 0)
			 len = 20;
		 end.x = start.x + len;
		 end.y = start.y;
		 drawTreetwig(start, end, BROWN, 2);
		 tree(start, end, 5);
	 }
#if 0
	 //BARK
	 start.x = 0;
	 start.y = (ST7735_TFTHEIGHT / 2);
	 end.x = 30;
	 end.y = (ST7735_TFTHEIGHT / 2);
	 drawTreetwig(start, end, BROWN, 3);
	 tree(start, end, 7);

	 start.x = 0;
	 start.y = (ST7735_TFTHEIGHT / 4);
	 end.x = 10;
	 end.y = (ST7735_TFTHEIGHT / 4);
	 tree(start, end, 3);

	 start.x = 0;
	 start.y = (3 * ST7735_TFTHEIGHT / 4);
	 end.x = 10;
	 end.y = (3 * ST7735_TFTHEIGHT / 4);
	 tree(start, end, 3);
#endif
	 return 0;

}

