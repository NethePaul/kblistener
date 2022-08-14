
#include "strtokeyh.h"
#include<Windows.h>
#include"VKEX.h"
std::vector<int> type(char character) {
#define doType(...) return {__VA_ARGS__};
	switch (character) {
	case 'a': doType(VK_KEY_A); break;
	case 'b': doType(VK_KEY_B); break;
	case 'c': doType(VK_KEY_C); break;
	case 'd': doType(VK_KEY_D); break;
	case 'e': doType(VK_KEY_E); break;
	case 'f': doType(VK_KEY_F); break;
	case 'g': doType(VK_KEY_G); break;
	case 'h': doType(VK_KEY_H); break;
	case 'i': doType(VK_KEY_I); break;
	case 'j': doType(VK_KEY_J); break;
	case 'k': doType(VK_KEY_K); break;
	case 'l': doType(VK_KEY_L); break;
	case 'm': doType(VK_KEY_M); break;
	case 'n': doType(VK_KEY_N); break;
	case 'o': doType(VK_KEY_O); break;
	case 'p': doType(VK_KEY_P); break;
	case 'q': doType(VK_KEY_Q); break;
	case 'r': doType(VK_KEY_R); break;
	case 's': doType(VK_KEY_S); break;
	case 't': doType(VK_KEY_T); break;
	case 'u': doType(VK_KEY_U); break;
	case 'v': doType(VK_KEY_V); break;
	case 'w': doType(VK_KEY_W); break;
	case 'x': doType(VK_KEY_X); break;
	case 'y': doType(VK_KEY_Y); break;
	case 'z': doType(VK_KEY_Z); break;
	case 'A': doType(VK_SHIFT, VK_KEY_A); break;
	case 'B': doType(VK_SHIFT, VK_KEY_B); break;
	case 'C': doType(VK_SHIFT, VK_KEY_C); break;
	case 'D': doType(VK_SHIFT, VK_KEY_D); break;
	case 'E': doType(VK_SHIFT, VK_KEY_E); break;
	case 'F': doType(VK_SHIFT, VK_KEY_F); break;
	case 'G': doType(VK_SHIFT, VK_KEY_G); break;
	case 'H': doType(VK_SHIFT, VK_KEY_H); break;
	case 'I': doType(VK_SHIFT, VK_KEY_I); break;
	case 'J': doType(VK_SHIFT, VK_KEY_J); break;
	case 'K': doType(VK_SHIFT, VK_KEY_K); break;
	case 'L': doType(VK_SHIFT, VK_KEY_L); break;
	case 'M': doType(VK_SHIFT, VK_KEY_M); break;
	case 'N': doType(VK_SHIFT, VK_KEY_N); break;
	case 'O': doType(VK_SHIFT, VK_KEY_O); break;
	case 'P': doType(VK_SHIFT, VK_KEY_P); break;
	case 'Q': doType(VK_SHIFT, VK_KEY_Q); break;
	case 'R': doType(VK_SHIFT, VK_KEY_R); break;
	case 'S': doType(VK_SHIFT, VK_KEY_S); break;
	case 'T': doType(VK_SHIFT, VK_KEY_T); break;
	case 'U': doType(VK_SHIFT, VK_KEY_U); break;
	case 'V': doType(VK_SHIFT, VK_KEY_V); break;
	case 'W': doType(VK_SHIFT, VK_KEY_W); break;
	case 'X': doType(VK_SHIFT, VK_KEY_X); break;
	case 'Y': doType(VK_SHIFT, VK_KEY_Y); break;
	case 'Z': doType(VK_SHIFT, VK_KEY_Z); break;
	case '0': doType(VK_KEY_0); break;
	case '1': doType(VK_KEY_1); break;
	case '2': doType(VK_KEY_2); break;
	case '3': doType(VK_KEY_3); break;
	case '4': doType(VK_KEY_4); break;
	case '5': doType(VK_KEY_5); break;
	case '6': doType(VK_KEY_6); break;
	case '7': doType(VK_KEY_7); break;
	case '8': doType(VK_KEY_8); break;
	case '9': doType(VK_KEY_9); break;
	case '-': doType(VK_OEM_MINUS); break;
	case '+': doType(VK_OEM_PLUS); break;
	case '.': doType(VK_OEM_PERIOD); break;
	case ',': doType(VK_OEM_COMMA); break;
	case '\t': doType(VK_TAB); break;
	case '\n': doType(VK_RETURN); break;
	case ' ': doType(VK_SPACE); break;
	}
}
