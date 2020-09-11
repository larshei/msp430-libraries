/*
 * WS2811.h
 *
 *  Created on: 06.01.2016
 *      Author: DemoniacMilk
 */

#ifndef WS2811_H_
#define WS2811_H_

#define PDIR	P1DIR
#define POUT	P1OUT
#define PPIN	BIT0
#define LED_count	25

#include <stdlib.h>
#include <msp430.h>

// Struct to hold RGB values
typedef struct{
	unsigned short red;
	unsigned short green;
	unsigned short blue;
} RGB_color;

// Struct to hold HSV values
typedef struct{
	int hue;
	float sat;
	float val;
} HSV_color;

// Some global Variables (Stack Overflow on trying to define these localy ... )
float c, m, x, h;


// FUNCTIONS
HSV_color* new_HSV_color(int hue, float sat, float val);							// create a new color and assign hue (0..359), saturation and value (each 0.0 .. 1.0)
RGB_color* new_RGB_color(unsigned short r, unsigned short g, unsigned short b);		// create a new color and assign the value r,g,b (0..255)
void HSV_to_RGB(HSV_color* HSVL, RGB_color* RGB);									// convert a HSV color to RGB. Use Values of color HSV, store values in color RGB
void n_to_color(RGB_color* RGB, int n);												// sets n numbers of LEDs to the desired color
void write_colors_to_LED(unsigned short* array, int n_LED);
void calculate_rainbow(unsigned short* array, int n_LED, float hue_offset, float hue_range, HSV_color* HSV, RGB_color* RGB);

#endif /* WS2811_H_ */
