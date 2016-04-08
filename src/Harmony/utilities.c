#include "utilities.h"

char* itsStore = 0;
char* htsStore = 0;

//Int to string
char* its(unsigned int i){
	unsigned int ci = i;
	unsigned short count = 0;
	while (ci != 0) {
		count++;
		ci /= 10;
	}
	ci = i;
	if(!itsStore){
		itsStore = (char*)kalloc(sizeof(char) * 11);
	}
	char* str = itsStore;
	for(unsigned int tmp = 0; tmp < 11; tmp++){ //Length of buffer.
		str[tmp] = '0';
	}
	str[count] = '\0';
	while (ci != 0) {
                switch(ci % 10){
			case 0:
				str[--count] = '0';
				break;
			case 1:
                                str[--count] = '1';
                                break;
			case 2:
                                str[--count] = '2';
                                break;
			case 3:
                                str[--count] = '3';
                                break;
			case 4:
                                str[--count] = '4';
                                break;
			case 5:
                                str[--count] = '5';
                                break;
			case 6:
                                str[--count] = '6';
                                break;
			case 7:
                                str[--count] = '7';
                                break;
			case 8:
                                str[--count] = '8';
                                break;
			case 9:
                                str[--count] = '9';
                                break;
			default:
				break;
		}
                ci /= 10;
        }
	return str;
}

//Hex to string.
//Int to string
char* hts(unsigned int i){
        unsigned int ci = i;
        unsigned short count = 2; // 0xNUM
        while (ci != 0) {
                count++;
                ci /= 0x10;
        }
        ci = i;
        if(!htsStore){
                htsStore = (char*)kalloc(sizeof(char) * 11);
        }
        char* str = htsStore;
	for(unsigned int tmp = 0; tmp < 11; tmp++){ //Length of buffer.
                str[tmp] = '0';
        }
        str[count] = '\0';
	str[0] = '0';
	str[1] = 'x';
        while (ci != 0) {
                switch(ci % 0x10){
                        case 0x0:
                                str[--count] = '0';
                                break;
                        case 0x1:
                                str[--count] = '1';
                                break;
                        case 0x2:
                                str[--count] = '2';
                                break;
                        case 0x3:
                                str[--count] = '3';
                                break;
                        case 0x4:
                                str[--count] = '4';
                                break;
                        case 0x5:
                                str[--count] = '5';
                                break;
                        case 0x6:
                                str[--count] = '6';
                                break;
                        case 0x7:
                                str[--count] = '7';
                                break;
                        case 0x8:
                                str[--count] = '8';
                                break;
                        case 0x9:
                                str[--count] = '9';
                                break;
			case 0xA:
                                str[--count] = 'A';
                                break;
			case 0xB:
                                str[--count] = 'B';
                                break;
			case 0xC:
                                str[--count] = 'C';
                                break;
			case 0xD:
                                str[--count] = 'D';
                                break;
			case 0xE:
                                str[--count] = 'E';
                                break;
			case 0xF:
                                str[--count] = 'F';
                                break;
                        default:
                                break;
                }
                ci /= 0x10;
        }
        return str;
}


//Scancode to char
char stc(unsigned char c){
	switch(c){
		case 0x02:
			return keyShift ? '!' : '1';
		case 0x03:
                        return keyShift ? '@' : '2';
		case 0x04:
                        return keyShift ? '#' : '3';
		case 0x05:
                        return keyShift ? '$' : '4';
		case 0x06:
                        return keyShift ? '%' : '5';
                case 0x07:
                        return keyShift ? '^' : '6';
                case 0x08:
                        return keyShift ? '&' : '7';
                case 0x09:
                        return keyShift ? '*' : '8';
                case 0x0A:
                        return keyShift ? '(' : '9';
                case 0x0B:
                        return keyShift ? ')' : '0';
		case 0x0F:
			return '\t';
		case 0x10:
			return keyShift ? 'Q' : 'q';
		case 0x11:
                        return keyShift ? 'W' : 'w';
                case 0x12:
                        return keyShift ? 'E' : 'e';
                case 0x13:
                        return keyShift ? 'R' : 'r';
                case 0x14:
                        return keyShift ? 'T' : 't';
                case 0x15:
                        return keyShift ? 'Y' : 'y';
                case 0x16:
                        return keyShift ? 'U' : 'u';
                case 0x17:
                        return keyShift ? 'I' : 'i';
                case 0x18:
                        return keyShift ? 'O' : 'o';
                case 0x19:
                        return keyShift ? 'P' : 'p';
                case 0x1e:
                        return keyShift ? 'A' : 'a';
                case 0x1f:
                        return keyShift ? 'S' : 's';
                case 0x20:
                        return keyShift ? 'D' : 'd';
                case 0x21:
                        return keyShift ? 'F' : 'f';
                case 0x22:
                        return keyShift ? 'G' : 'g';
                case 0x23:
                        return keyShift ? 'H' : 'h';
                case 0x24:
                        return keyShift ? 'J' : 'j';
                case 0x25:
                        return keyShift ? 'K' : 'k';
                case 0x26:
                        return keyShift ? 'L' : 'l';
                case 0x27:
                        return keyShift ? ':' : ';';
                case 0x28:
                        return keyShift ? '\"' : '\'';
                case 0x1c:
                        return '\n';
                case 0x2c:
                        return keyShift ? 'Z' : 'z';
                case 0x2d:
                        return keyShift ? 'X' : 'x';
                case 0x2e:
                        return keyShift ? 'C' : 'c';
                case 0x2f:
                        return keyShift ? 'V' : 'v';
                case 0x30:
                        return keyShift ? 'B' : 'b';
                case 0x31:
                        return keyShift ? 'N' : 'n';
                case 0x32:
                        return keyShift ? 'M' : 'm';
                case 0x33:
                        return keyShift ? '<' : ',';
                case 0x34:
                        return keyShift ? '>' : '.';
                case 0x39:
                        return ' ';
		case 0x0E:
			return '\b';
		case 0xE0:
                        return '\b';
		case 0x53:
                        return '\b';
		case 0x2A:
			keyShift = TRUE;
			return '\0';
		case 0x36:
			keyShift = TRUE;
			return '\0';
		case 0xAA:
			keyShift = FALSE;
			return '\0';
		case 0xB6:
			keyShift = FALSE;
			return '\0';
		case 0x01:
			//Restart.
			fbClear(BLACK);
			fbWrite("Error 404: Error Not Found.", DTCOLOR, BLACK);
			lidtb(0);
			__asm__ __volatile__ ("int $0x0" : : : "memory");
			return '\0';
		default:
			return '\0';
	}
}

void* genkalloc(unsigned int size, bool align, unsigned int* phys){
	if(frHeap > 0xC0300000){
		//Infringing on user heap, switch to that.
		return malloc(size);
	}
	if(align && (frHeap & 0xFFFFF000)){
		frHeap &= 0xFFFFF000;
		frHeap += 0x1000;
	}
	if(phys){
		phys = (unsigned int*)frHeap;
	}
	void* r = (void *)frHeap;
	frHeap += size;
	return r;
}
