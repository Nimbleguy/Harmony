#include "terminal.h"

/* The address of the framebuffer in memory. */
char *framebuffer = (char *) 0xC00B8010;

//Other framebuffer.
struct fbText frame[(MAXROW + 1) * (MAXCOLUMN + 1)] = {0};

/* Maximum column in 79. Maximum row is 24. */
unsigned int column = 0;
unsigned int row = 0;
//Last clear color.
unsigned char lccolor = BLACK;

/* Writes char c to framebuffer position i with foreground & background color fg & bg */
void writeChar(unsigned int i, char c, unsigned char fg, unsigned char bg){
	framebuffer[i] = c;
	framebuffer[i + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);
}

/* Moves the cursor. */
void moveCursor(unsigned short pos){
	outb(FBCOMMAND, FBHIGH);
	outb(FBDATA, ((pos >> 8) & 0x00FF));
	outb(FBCOMMAND, FBLOW);
	outb(FBDATA, pos & 0x00FF);
}

void fbWrite(char* s, unsigned char fg, unsigned char bg){
	for(unsigned int i = 0; s[i] != '\0'; i++){
		if(s[i] == '\n'){
			if(row == MAXROW){
				fbScroll();
			}
			else{
				row++;
			}
			column = 0;
			moveCursor((row * (MAXCOLUMN + 1)) + column);
			continue;
		}
		if(row == MAXROW && column == MAXCOLUMN){
			fbScroll();
		}
		if(column == MAXCOLUMN){
                        column = 0;
                        row++;
                }
		if(s[i] == '\b'){
			if(column == 0){
				if(row == 0){
					continue;
				}
				row--;
                                column = MAXCOLUMN - 1;
			}
			else{
				column--;
			}
			fbChar(' ', fg, bg);
			fbMove(row, column);
			continue;
		}
		fbChar(s[i], fg, bg);
		fbMove(row, column);
		column++;
	}
}

void fbChar(char c, unsigned char fg, unsigned char bg){
	frame[(row * 80) + column] = (struct fbText){.c = c, .forecolor = fg, .backcolor = bg};
}

void fbSync(){
	for(unsigned int c = 0; c < MAXCOLUMN + 1; c++){
		for(unsigned int r = 0; r < MAXROW + 1; r++){
			struct fbText t = frame[(r * 80) + column];
			writeChar(2 * ((r * (MAXCOLUMN + 1)) + c - 8), t.c, t.forecolor, t.backcolor);
		}
	}
	moveCursor((row * (MAXCOLUMN + 1)) + column + 1);
}

void fbScroll(){
	fbMove(0, 0);
	return;

	for(unsigned int r = 0; r < (MAXROW + 1) - 1; r++){ //-1 because last is done manually.
		for(unsigned int c = 0; c < (MAXCOLUMN + 1); c++){
			frame[(r * (MAXCOLUMN + 1)) + c] = frame[((r + 1) * (MAXCOLUMN + 1)) + c];
		}
	}
	for(unsigned int c = 0; c < (MAXCOLUMN + 1); c++){
		frame[(MAXROW * (MAXCOLUMN + 1)) + c] = (struct fbText){.c = ' ', .forecolor = DTCOLOR, .backcolor = BLACK};
	}
	fbMove(MAXROW, 0);
	fbSync();
}

void fbClear(unsigned char color){
	for(unsigned int c = 0; c < MAXCOLUMN + 1; c++){
		// Insert C++ pun here
		for(unsigned int r = 0; r < MAXROW + 1; r++){
			frame[(r * (MAXCOLUMN + 1)) + c] = (struct fbText){.c = ' ', .forecolor = DTCOLOR, .backcolor = color};
		}
	}
	moveCursor(0);
	column = 0;
	row = 0;
	lccolor = color;
}

void fbMove(unsigned int r, unsigned int c){
	row = r;
	column = c;
	moveCursor((row * (MAXCOLUMN + 1)) + column + 1);
}
