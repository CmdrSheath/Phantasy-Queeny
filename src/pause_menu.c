//==============================================================================
// PHANTASY QUEENY - Pause Menu System
// Music continues from overworld, only SFX play
//==============================================================================

#include <genesis.h>
#include <string.h>
#include <stdio.h>
#include "game.h"
#include "audio.h"

static PauseMenuOption currentSelection = MENU_PARTY;
static u16 prevJoyState = 0;
static bool inSubMenu = false;
static u8 subMenuSelection = 0;
static bool firstInit = true;

static const char* menuLabels[MENU_COUNT] = {
    "PARTY",
    "INV",
    "ALIGN",
    "MAP",
    "OPT",
    "SAVE"
};

static void clearSubMenuArea(void) {
    for(u8 y = 8; y <= 20; y++) {
        VDP_clearText(2, y, 22);
    }
}

static void closeSubMenu(void) {
    inSubMenu = false;
    clearSubMenuArea();
    VDP_drawText(" ", 28, 8 + currentSelection);
}

static void openSubMenu(PauseMenuOption option) {
    inSubMenu = true;
    subMenuSelection = 0;
    clearSubMenuArea();

    VDP_drawText("+------------------+", 2, 8);

    switch(option) {
        case MENU_PARTY:
            VDP_drawText("| QUEENY |", 2, 9);
            VDP_drawText("| Lv: 1 |", 2, 10);
            VDP_drawText("| HP: 100/100 |", 2, 11);
            VDP_drawText("| MP: 50/50 |", 2, 12);
            VDP_drawText("| ATK: 15 DEF: 10 |", 2, 13);
            break;
        case MENU_INVENTORY:
            VDP_drawText("| INVENTORY |", 2, 9);
            VDP_drawText("| |", 2, 10);
            VDP_drawText("| (Empty) |", 2, 11);
            VDP_drawText("| |", 2, 12);
            VDP_drawText("| |", 2, 13);
            break;
        case MENU_ALIGNMENT:
            VDP_drawText("| ALIGNMENT |", 2, 9);
            VDP_drawText("| |", 2, 10);
            VDP_drawText("| Neutral |", 2, 11);
            VDP_drawText("| [====|====] |", 2, 12);
            VDP_drawText("| |", 2, 13);
            break;
        case MENU_MAP:
            VDP_drawText("| MAP |", 2, 9);
            VDP_drawText("| |", 2, 10);
            VDP_drawText("| Falcon City |", 2, 11);
            VDP_drawText("| Downtown |", 2, 12);
            VDP_drawText("| |", 2, 13);
            break;
        case MENU_OPTIONS:
            VDP_drawText("| OPTIONS |", 2, 9);
            VDP_drawText("| SFX: ON |", 2, 10);
            VDP_drawText("| Music: ON |", 2, 11);
            VDP_drawText("| |", 2, 12);
            VDP_drawText("| |", 2, 13);
            break;
        case MENU_SAVE:
            VDP_drawText("| SAVE GAME? |", 2, 9);
            VDP_drawText("| |", 2, 10);
            VDP_drawText("| > Yes |", 2, 11);
            VDP_drawText("| No |", 2, 12);
            VDP_drawText("| |", 2, 13);
            break;
        default:
            break;
    }

    VDP_drawText("+------------------+", 2, 14);
    VDP_drawText("C:BACK", 2, 16);
}

void initPauseMenu(void) {
    currentSelection = MENU_PARTY;
    prevJoyState = 0;
    inSubMenu = false;

    if (firstInit) {
        playSFX(SFX_MENU_SELECT);
        firstInit = false;
    }

    VDP_drawText("+--------+", 28, 4);
    VDP_drawText("| MENU |", 28, 5);
    VDP_drawText("+--------+", 28, 6);

    for(u8 i = 0; i < MENU_COUNT; i++) {
        VDP_drawText(menuLabels[i], 30, 8 + i);
    }

    VDP_drawText("+--------+", 28, 8 + MENU_COUNT);
    VDP_drawText(">", 29, 8);
    VDP_drawText("A:SEL B/START:CLOSE", 2, 26);
}

void updatePauseMenu(void) {
    u16 joyState = JOY_readJoypad(JOY_1);
    u16 pressed = joyState & ~prevJoyState;

    if (pressed & (BUTTON_START | BUTTON_B)) {
        playSFX(SFX_MENU_CANCEL);
        firstInit = true;
        changeState(STATE_OVERWORLD);
        return;
    }

    if (inSubMenu) {
        if (pressed & BUTTON_C) {
            playSFX(SFX_MENU_CANCEL);
            closeSubMenu();
        }
        else if (pressed & BUTTON_A && currentSelection == MENU_SAVE) {
            playSFX(SFX_MENU_SELECT);
            VDP_drawText("SAVED!", 10, 18);
        }
        prevJoyState = joyState;
        return;
    }

    if (pressed & BUTTON_UP) {
        if (currentSelection > 0) {
            playSFX(SFX_MENU_MOVE);
            VDP_drawText(" ", 29, 8 + currentSelection);
            VDP_drawText(" ", 28, 8 + currentSelection);
            currentSelection--;
            VDP_drawText(">", 29, 8 + currentSelection);
        }
    }

    if (pressed & BUTTON_DOWN) {
        if (currentSelection < MENU_COUNT - 1) {
            playSFX(SFX_MENU_MOVE);
            VDP_drawText(" ", 29, 8 + currentSelection);
            VDP_drawText(" ", 28, 8 + currentSelection);
            currentSelection++;
            VDP_drawText(">", 29, 8 + currentSelection);
        }
    }

    if (pressed & BUTTON_A) {
        playSFX(SFX_MENU_SELECT);
        VDP_drawText("*", 28, 8 + currentSelection);
        openSubMenu(currentSelection);
    }

    prevJoyState = joyState;
}

void cleanupPauseMenu(void) {
    for(u8 y = 4; y <= 16; y++) {
        VDP_clearText(28, y, 10);
    }
    VDP_clearText(2, 26, 25);
    VDP_clearText(15, 15, 6);
    clearSubMenuArea();
}
