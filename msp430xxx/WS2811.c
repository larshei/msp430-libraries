/*
 * WS2811.c
 *
 *  Created on: 06.01.2016
 *      Author: DemoniacMilk
 */
#include <WS2811.h>

HSV_color* new_HSV_color(int hue, float sat, float val){
	HSV_color *HSV;
	HSV = malloc(sizeof(HSV_color));// reserve memory
	if (HSV == 0) {
		return 0;
	}	// not enough ram available, exit


	// make sure were in range
	if (hue < 0){
		while (hue < 0) {hue += 360;}
	} else if (hue > 0) {
		while (hue > 360) {hue -= 360;}
	}

	if (sat > 1.0) {sat = 1.0;}
	if (sat < 0.0) {sat = 0.0;}
	if (val > 1.0) {val = 1.0;}
	if (val < 0.0) {val = 0.0;}

	HSV->hue = hue;
	HSV->sat = sat;
	HSV->val = val;

	return HSV;
}

RGB_color* new_RGB_color(unsigned short r, unsigned short g, unsigned short b){
	RGB_color *RGB;
	RGB = malloc(sizeof(RGB_color));// reserve memory
	if (RGB == 0) {return 0;}	// not enough ram available, exit

	RGB->red = r;
	RGB->green = g;
	RGB->blue = b;

	return RGB;
}

void HSV_to_RGB(HSV_color* HSVL, RGB_color* RGB){
	c = (HSVL->val) * (HSVL->sat);
	m = HSVL->val - c;

	// get position on current linear gradient
	h = (float)HSVL->hue / 60;
	// calculate rest
	while (h > 2) {
		h -= 2;
	}
	h -= 1;
	if (h < 0) {
		h = -h;
	}
	x = c * (1 - h);


	if (HSVL->hue >= 0 && HSVL->hue < 60) {
		RGB->red = (c + m) * 255;
		RGB->green = (x + m) * 255;
		RGB->blue = m * 255;
	} else if (HSVL->hue >= 60 && HSVL->hue < 120){
		RGB->red = (x + m) * 255;
		RGB->green = (c + m) * 255;
		RGB->blue = m * 255;
	} else if (HSVL->hue >= 120 && HSVL->hue < 180) {
		RGB->red = m * 255;
		RGB->green = (c + m) * 255;
		RGB->blue = (x+m) * 255;
	} else if (HSVL->hue >= 180 && HSVL->hue < 240){
		RGB->red = m * 255;
		RGB->green = (x + m) * 255;
		RGB->blue = (c+m) * 255;
	} else if (HSVL->hue >= 240 && HSVL->hue < 300){
		RGB->red = (x+m) * 255;
		RGB->green = m * 255;
		RGB->blue = (c+m) * 255;
	} else if (HSVL->hue >= 300 && HSVL->hue < 360){
		RGB->red = (c+m) * 255;
		RGB->green = m * 255;
		RGB->blue = (x+m) * 255;
	} else {
		RGB->red = m * 255;
		RGB->green = m * 255;
		RGB->blue = m * 255;
	}
	__no_operation();
}

void n_to_color(RGB_color* RGB, int n){
	int i,j,c;
	short color[3], workByte;

	color[0] = RGB->green;
	color[1] = RGB->red;
	color[2] = RGB->blue;

	// for the number of LEDs
	for (i=0; i<n;i++) {
		for(c=0;c<3;c++){
			workByte = color[c];
			for (j=0; j<8;j++) {
				if (workByte & 0x80) {
					POUT |= PPIN;
					asm(" nop");
					asm(" nop");
					asm(" nop");
					asm(" nop");
					asm(" nop");
					asm(" nop");
					POUT &= ~PPIN;
				} else {
					POUT |= PPIN;
					POUT &= ~PPIN;
					asm(" nop");
					asm(" nop");
					asm(" nop");
					asm(" nop");
				}
				workByte <<= 1;
			}
		}
	}
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * function write_colors_to_LED
 * param:
 * 		RGB_color: pointer to starting element in an array with RGB_colors (unsigned shorts: R, G, B)
 * 			an array containing R0, G0, B0, R1, G1, B1, R2, G2, ... , Rn-1, Gn-1, Bn-1
 *  	n_LED: integer, number of LEDs
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void write_colors_to_LED(unsigned short* array, int n_LED){
	int i,j,c;

	unsigned short workByte;
	for (i=0; i<n_LED;i++) {
		for(c=0;c<3;c++){
			workByte = *array++;
			for (j=0; j<8;j++) {
				if (workByte & 0x80) {
					POUT |= PPIN;
					asm(" nop");
					asm(" nop");
					asm(" nop");
					asm(" nop");
					asm(" nop");
					asm(" nop");
					POUT &= ~PPIN;
				} else {
					POUT |= PPIN;
					POUT &= ~PPIN;
					asm(" nop");
					asm(" nop");
					asm(" nop");
					asm(" nop");
				}
				workByte <<= 1;
			}
		}
	}
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * function calculate_rainbow
 *
 * Sweeps through the Hue value in HSV-room. Stores the calculated colors  Saturation and Value are not affected and kept from the param HSV.
 *
 *
 * param:
 * 		RGB_color: (unsigned short*) pointer to starting element in an array. The array must have the size of 3*LED_count Bytes!
 *  	n_LED: (float) number of LEDs
 *  	hue_offset: (float) offsets the hue of the rainbow. You can use this function with differnt hue offsets over time to create a "moving" rainbow
 *  	hue_range: (float) number of degrees to vary the hue (360° = full rainbow, 720° = 2 rainbows, fractions possible)
 *  	RGB and HSV: the function needs a handler for HSV and RGB colors. Create one with new_RGB/HSV_color. Make sure to not do this in a loop as it allocates memory.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
void calculate_rainbow(unsigned short* array, int n_LED, float hue_offset, float hue_range, HSV_color* HSV, RGB_color* RGB){
	float hue_frac;
	int i;
	// Make sure our hue is somewhere between 0 and 359 (makes calculation in loop easier)
	while (hue_offset > 359){
		hue_offset -= 360; }
	while (hue_offset < 0){
		hue_offset += 360; }

	hue_frac = hue_range / n_LED;

	for (i=0; i<n_LED; i++){
		HSV->hue = hue_frac * i + hue_offset;
		while (HSV->hue >= 360) { HSV->hue -= 360;	}
    	HSV_to_RGB(HSV, RGB);
    	*array++ = RGB->red;
    	*array++ = RGB->green;
    	*array++ = RGB->blue;
	}
}

