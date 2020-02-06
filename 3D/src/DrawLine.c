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



int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;

typedef struct Point3D
{
	float x;
	float y;
	float z;
} Point3D;

typedef struct Point
{
	float x;
	float y;
} Point;

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

 int16_t i;

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

 for ( i = count*ms; i--; i > 0);

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


/*

 Main Function main()

*/

Point3D e = {200, 200, 200};
Point3D s = {50, 50, 300};
Point3D p1 = {100, 100, 110};
float sin_theta = 0, cos_theta = 0, sin_phi = 0, cos_phi = 0, D = 30, rho = 0;
float delta_x = 0, delta_y = 0;
float hypotenuse = 0;

void init_coordinate_system()
{
	rho = sqrt(pow(e.x, 2) + pow(e.y, 2) + pow(e.z, 2));
	hypotenuse = sqrt(pow(e.x, 2) + pow(e.y, 2));
	sin_theta = e.y / hypotenuse;
	cos_theta = e.x / hypotenuse;
	sin_phi = hypotenuse / rho;
	cos_phi = e.z / rho;
	delta_y = ST7735_TFTHEIGHT / 2;
	delta_x = ST7735_TFTWIDTH / 2;
}

Point get_2D_coordinates(Point3D world)
{
	Point3D viewer;
	Point perspective;

	viewer.x = -sin_theta * world.x + cos_theta * world.y;
	viewer.y = -cos_theta * cos_phi * world.x - sin_theta * cos_phi * world.y + sin_phi * world.z;
	viewer.z = -cos_theta * sin_phi * world.x - cos_theta * sin_phi * world.y - cos_phi * world.z + rho;

	perspective.x = delta_x + ((D / viewer.z) * viewer.x);
	perspective.y = delta_y - ((D / viewer.z) * viewer.y);

	return perspective;
}


void find_intersection_pt()
{
	Point3D s = {50, 50, 200};
	Point3D pt = {0, 100, 100};
	Point3D n = {0, 0, 1};
	Point3D a = {0, 0, 0};
	Point3D i_pt;
	float lamda;
	Point source, point1, intersection_pt;

	source = get_2D_coordinates(s);
	point1 = get_2D_coordinates(pt);

	drawLine(source.x, source.y, point1.x, point1.y, BLACK);

	//lamda = (n.x * (a.x - s.x) + n.y * (a.y - s.y) + n.z * (a.z - s.z)) / (n.x * (s.x - p1.x) + n.y * (s.y - p1.y) + n.z * (s.z - p1.z));

	lamda = -s.z / (n.z * (s.z - pt.z));
	i_pt.x = s.x + lamda * (s.x - pt.x);
	i_pt.y = s.y + lamda * (s.y - pt.y);
	i_pt.z = s.z + lamda * (s.z - pt.z);

	printf("i_pt %f and lamda = %f", i_pt.z, lamda);
	intersection_pt = get_2D_coordinates(i_pt);
	drawLine(intersection_pt.x, intersection_pt.y, point1.x, point1.y, BLACK);

}

void draw_world_coordinates()
{
	Point3D c = {0,0,0};
	Point3D x = {200, 0, 0};
	Point3D y = {0, 200, 0};
	Point3D z = {0, 0, 200};
	Point centre, xaxis, yaxis, zaxis;

	centre = get_2D_coordinates(c);
	xaxis = get_2D_coordinates(x);
	yaxis = get_2D_coordinates(y);
	zaxis = get_2D_coordinates(z);

	drawLine(centre.x, centre.y, xaxis.x, xaxis.y, RED);
	drawLine(centre.x, centre.y, yaxis.x, yaxis.y, BLUE);
	drawLine(centre.x, centre.y, zaxis.x, zaxis.y, GREEN);
}

void draw_cube()
{
	Point3D p0 = {0, 100, 0}, p1 = {0, 100, 100}, p2 = {100, 100, 100}, p3 = {100, 100, 0};
	Point3D  p4 = {0, 0, 100}, p5 = {100, 0, 100}, p6 = {100, 0, 0}, p7 = {0, 0, 0};

	Point pt0, pt1, pt2, pt3, pt4, pt5, pt6, pt7;

	pt0 = get_2D_coordinates(p0);
	pt1 = get_2D_coordinates(p1);
	pt2 = get_2D_coordinates(p2);
	pt3 = get_2D_coordinates(p3);
	pt4 = get_2D_coordinates(p4);
	pt5 = get_2D_coordinates(p5);
	pt6 = get_2D_coordinates(p6);
	pt7 = get_2D_coordinates(p7);

	drawLine(pt0.x, pt0.y, pt1.x, pt1.y, BLACK);
	drawLine(pt0.x, pt0.y, pt7.x, pt7.y, BLACK);
	drawLine(pt0.x, pt0.y, pt3.x, pt3.y, BLACK);
	drawLine(pt1.x, pt1.y, pt2.x, pt2.y, BLACK);
	drawLine(pt1.x, pt1.y, pt4.x, pt4.y, BLACK);
	drawLine(pt2.x, pt2.y, pt5.x, pt5.y, BLACK);
	drawLine(pt2.x, pt2.y, pt3.x, pt3.y, BLACK);
	drawLine(pt6.x, pt6.y, pt3.x, pt3.y, BLACK);
	drawLine(pt6.x, pt6.y, pt5.x, pt5.y, BLACK);
	drawLine(pt4.x, pt4.y, pt7.x, pt7.y, BLACK);
	drawLine(pt7.x, pt7.y, pt6.x, pt6.y, BLACK);
	drawLine(pt4.x, pt4.y, pt5.x, pt5.y, BLACK);
}

int main (void)
{
	 uint32_t pnum = PORT_NUM;

	 pnum = 1 ;

	 if ( pnum == 1 )
		 SSP1Init();

	 else
		 puts("Port number is not correct");

	 lcd_init();

	 fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, WHITE);
	 init_coordinate_system();
	 draw_world_coordinates();
	 draw_cube();
	 //find_intersection_pt();
	 return 0;

}

