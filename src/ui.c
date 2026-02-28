/////
// UI and menu systems - implement later
#include "game.h"
void initMenu() {}
void updateMenu() {}
void cleanupMenu() {}
void drawText(u16 x, u16 y, const char* text) {
    VDP_drawText(text, x, y);
}
void drawNumber(u16 x, u16 y, u16 number, u8 digits) {}
void showMessage(const char* text) {
    VDP_drawText(text, 2, 24);
}
void clearMessage() {
    VDP_clearTextArea(2, 24, 36, 2);
}
