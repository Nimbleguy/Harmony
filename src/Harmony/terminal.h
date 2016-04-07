#ifndef TERMINALH
#define TERMINALH

#include "common.h"
#include "io.h"

#define FBCOMMAND 0xC00003D4
#define FBDATA 0xC00003D5
#define FBHIGH 14
#define FBLOW 15

#define MAXCOLUMN (80 - 1)
#define MAXROW (25 - 1)

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LGREY 7
#define DGREY 8
#define LBLUE 9
#define LGREEN 10
#define LCYAN 11
#define LRED 12
#define LMAGENTA 13
#define LBROWN 14
#define WHITE 15
#define DTCOLOR GREEN

struct fbText{
	char c;
	unsigned char forecolor;
	unsigned char backcolor;
};

void fbWrite(char* s, unsigned char fg, unsigned char bg);
void fbSync();
void fbChar(char c, unsigned char fg, unsigned char bg);
void fbScroll();
void fbClear(unsigned char color);
void fbMove(unsigned int r, unsigned int c);

#endif
