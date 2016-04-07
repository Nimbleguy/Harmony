#include "terminal.h"

/* The address of the framebuffer in memory. */
char *framebuffer = (char *) 0xC00B8010;

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
		if(s[i] == '\t'){
			fbWrite("     ", fg, bg);
		}
		if(s[i] == '\n'){
			if(row == MAXROW){
				fbClear(lccolor);
				row = 0;
			}
			else{
				row++;
			}
			column = 0;
			moveCursor((row * 80) + column);
			continue;
		}
		if(row == MAXROW && column == MAXCOLUMN){
			fbClear(lccolor);
			column = 0;
			row = 0;
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
			writeChar(2 * ((row * 80) + column - 8), ' ', fg, bg);
			moveCursor((row * 80) + column);
			continue;
		}
		writeChar(2 * ((row * 80) + column - 8), s[i], fg, bg);
		moveCursor((row * 80) + column + 1);
		column++;
	}
}

void fbClear(unsigned char color){
	for(unsigned int c = 0; c < MAXCOLUMN + 1; c++){
		// Insert C++ pun here
		for(unsigned int r = 0; r < MAXROW + 1; r++){
			writeChar(2 * ((r * 80) + c - 8), ' ', DTCOLOR, color);
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
	moveCursor((row * 80) + column + 1);
}
