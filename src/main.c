//==============================================================================
// PHANTASY QUEENY - Sega Genesis RPG
// Main entry point
//==============================================================================

#include <genesis.h>

// Minimal main for testing compilation
int main(bool hardReset)
{
    // Initialize system
    VDP_setScreenWidth320();
    VDP_setBackgroundColor(0);
    VDP_setPaletteColor(0, 0x0000); // Black background
    VDP_setPaletteColor(1, 0x0EEE); // White text
    
    // Simple text display
    VDP_drawText("PHANTASY QUEENY", 10, 10);
    VDP_drawText("SYSTEM TEST", 12, 12);
    VDP_drawText("Press START", 11, 20);
    
    // Main loop
    while(1)
    {
        // Wait for vertical blank (60 FPS)
        SYS_doVBlankProcess();
    }
    
    return 0;
}
