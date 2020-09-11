/*
 * SSD1306.h
 *
 *  Created on: 21.02.2015
 *      Author: DemoniacMilk
 */

#ifndef SSD1306_H_
#define SSD1306_H_

//draw modes: how shall the values for the pixels be determined?
#define AVG		0	// average of all values that are to be drawn in the same column
#define BAND	1	// draw min value, draw max value for each x-step and fill
#define SKIP	2	// display every nth value only and ignore everythng in between

void setStartPage(unsigned short n);
void setStartColumn(unsigned short n);
void display_clear();
void display_init();
void display_print(char sendMe[], int x, int page, int varCount, ...);
void display_drawGraph(int *values, int count, int startX, int startY, int endX, int endY, int drawMode);


#endif /* SSD1306_H_ */
