#ifndef LINK_CODES_H
#define LINK_CODES_H

// link codes (each 1-byte)
#define LINK_BUTTON_PRESSED 0x00 // 0000xxxx, x - player number(0000 - 1111, 16 players). V1: [0, 3], V2: [0, 15]
#define LINK_CLEAR 0x10 // 0001xxxx, no data. V1, V2
#define LINK_CORRECT_PRESS_SIGNAL 0x20 // 0010xxxx, x - player number(0000 - 1111, 16 players). V1: [4, 7], V2: [0, 15]
#define LINK_FALSTART_PRESS_SIGNAL 0x30 // 0011xxxx, x - player number(0000 - 1111, 16 players) V1: [4, 7], V2: [0, 15]
#define LINK_GAME_START_SIGNAL 0x40 // 0100xxxx, no data. V1, V2
#define LINK_DISPLAY_CORRECT_PRESS_SIGNAL 0x50 // 0101xxxx, x - player number(0000 - 1111, 16 players) only V2: [0, 15]
#define LINK_DISPLAY_FALSTART_PRESS_SIGNAL 0x60 // 0110xxxx, x - player number(0000 - 1111, 16 players) only V2: [0, 15]
#define LINK_PENDING_PRESS_SIGNAL 0x70 //0111xxxx, x - player number(0000 - 1111, 16 players) only V2: [0, 15]
#define LINK_UPDATE_TIME 0x80 // 1xxxxxxx - x time in seconds (0000000 - 1111111, 0-127 seconds) V1, V2: [0, 127]

#endif
