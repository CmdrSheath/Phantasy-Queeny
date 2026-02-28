/////
//==============================================================================
// PHANTASY QUEENY - Turn-Based Combat System
// Sci-fi superhero battles with morality choices
//==============================================================================

#include "game.h"

// Combat UI positions
#define COMBAT_MENU_X   2
#define COMBAT_MENU_Y   20
#define HP_BAR_X        2
#define HP_BAR_Y        2
#define ENEMY_X         160
#define ENEMY_Y         80

// Combat phases
typedef enum {
    PHASE_START,         // Transition in
    PHASE_PLAYER_INPUT,  // Waiting for command
    PHASE_EXECUTION,     // Playing out actions
    PHASE_VICTORY,       // Win screen
    PHASE_DEFEAT,        // Game over
    PHASE_ESCAPE         // Ran away
} CombatPhase;

CombatPhase combatPhase;
u8 turnOrder[8];       // Indices into combined party+enemies
u8 turnCount = 0;
u8 currentTurn = 0;
bool combatActive = FALSE;

// UI state
u8 menuCursor = 0;     // 0=Attack, 1=Tech, 2=Item, 3=Flee
u8 targetCursor = 0;
bool selectingTarget = FALSE;

// Enemy database
const Enemy enemyDatabase[] = {
    [ENEMY_SLASHER] = {
        .type = ENEMY_SLASHER,
        .name = "SLASHER",
        .stats = {
            .hp = 30, .maxHp = 30,
            .mp = 0, .maxMp = 0,
            .strength = 12,
            .mental = 4,
            .agility = 10,
            .defense = 6,
            .level = 2
        },
        .expReward = 15,
        .itemDrop = ITEM_CRYSTAL_SHARD,
        .dropRate = 30,
        .isBoss = FALSE
    }
};

void initCombat() {
    // Clear screen
    VDP_clearPlan(BG_A, TRUE);
    VDP_clearPlan(BG_B, TRUE);
    
    // Load combat palette
    VDP_setPalette(PAL0, palette_ui);
    
    // Setup background (combat arena)
    // VDP_drawImage(BG_B, &combat_bg, 0, 0);
    
    combatPhase = PHASE_START;
    combatActive = TRUE;
    currentTurn = 0;
    menuCursor = 0;
    selectingTarget = FALSE;
    
    // Calculate turn order based on agility
    calculateTurnOrder();
    
    // Draw initial UI
    drawCombatUI();
    
    // Transition effect
    showMessage("BATTLE START!");
    waitFrames(60);
    
    combatPhase = PHASE_PLAYER_INPUT;
    nextTurn();
}

void startCombat(EnemyType type, u8 count) {
    // Setup enemies
    enemyCount = count;
    for (u8 i = 0; i < count; i++) {
        currentEnemies[i] = enemyDatabase[type];
        currentEnemies[i].sprite = SPR_addSprite(&spr_slasher,
            ENEMY_X + (i * 40), ENEMY_Y,
            TILE_ATTR(PAL0, FALSE, FALSE, FALSE));
    }
    
    changeState(STATE_COMBAT);
}

void calculateTurnOrder() {
    // Simple sort by agility (highest first)
    // Combine party and enemies into turn order
    turnCount = 0;
    
    // Add all combatants with their agility scores
    typedef struct { u8 index; bool isPlayer; u8 agility; } Actor;
    Actor actors[8];
    u8 actorCount = 0;
    
    // Add party members
    for (u8 i = 0; i < partyCount; i++) {
        if (party[i].stats.hp > 0) {
            actors[actorCount].index = i;
            actors[actorCount].isPlayer = TRUE;
            actors[actorCount].agility = party[i].stats.agility;
            actorCount++;
        }
    }
    
    // Add enemies
    for (u8 i = 0; i < enemyCount; i++) {
        if (currentEnemies[i].stats.hp > 0) {
            actors[actorCount].index = i;
            actors[actorCount].isPlayer = FALSE;
            actors[actorCount].agility = currentEnemies[i].stats.agility;
            actorCount++;
        }
    }
    
    // Bubble sort by agility (descending)
    for (u8 i = 0; i < actorCount - 1; i++) {
        for (u8 j = 0; j < actorCount - i - 1; j++) {
            if (actors[j].agility < actors[j+1].agility) {
                Actor temp = actors[j];
                actors[j] = actors[j+1];
                actors[j+1] = temp;
            }
        }
    }
    
    // Store turn order (pack into single byte: isPlayer << 7 | index)
    for (u8 i = 0; i < actorCount; i++) {
        turnOrder[i] = (actors[i].isPlayer << 7) | actors[i].index;
    }
    turnCount = actorCount;
}

void nextTurn() {
    if (currentTurn >= turnCount) {
        // New round
        currentTurn = 0;
        calculateTurnOrder(); // Recalculate in case stats changed
    }
    
    u8 actorInfo = turnOrder[currentTurn];
    bool isPlayerTurn = (actorInfo >> 7) & 1;
    u8 actorIndex = actorInfo & 0x7F;
    
    if (isPlayerTurn) {
        // Check if this party member is alive
        if (party[actorIndex].stats.hp == 0) {
            currentTurn++;
            nextTurn();
            return;
        }
        
        combatPhase = PHASE_PLAYER_INPUT;
        menuCursor = 0;
        selectingTarget = FALSE;
        drawMenu();
        
        // Highlight active character
        showMessage(party[actorIndex].name);
    } else {
        // Enemy AI turn
        combatPhase = PHASE_EXECUTION;
        executeEnemyAI(actorIndex);
    }
}

void updateCombat() {
    switch(combatPhase) {
        case PHASE_PLAYER_INPUT:
            handlePlayerInput();
            break;
            
        case PHASE_EXECUTION:
            // Animation/effects playing
            break;
            
        case PHASE_VICTORY:
            if (joyState & BUTTON_A) {
                endCombat(TRUE);
            }
            break;
            
        case PHASE_DEFEAT:
            // Game over handling
            break;
            
        default:
            break;
    }
    
    // Update sprites
    SPR_update();
}

void handlePlayerInput() {
    if (selectingTarget) {
        // Target selection mode
        if (joyState & BUTTON_UP) {
            targetCursor = (targetCursor > 0) ? targetCursor - 1 : enemyCount - 1;
            waitFrames(10); // Debounce
        }
        if (joyState & BUTTON_DOWN) {
            targetCursor = (targetCursor < enemyCount - 1) ? targetCursor + 1 : 0;
            waitFrames(10);
        }
        if (joyState & BUTTON_A) {
            // Confirm target
            executePlayerAction(targetCursor);
            selectingTarget = FALSE;
        }
        if (joyState & BUTTON_B) {
            // Cancel
            selectingTarget = FALSE;
            drawMenu();
        }
    } else {
        // Menu selection
        if (joyState & BUTTON_UP) {
            menuCursor = (menuCursor > 0) ? menuCursor - 1 : 3;
            drawMenu();
            waitFrames(10);
        }
        if (joyState & BUTTON_DOWN) {
            menuCursor = (menuCursor < 3) ? menuCursor + 1 : 0;
            drawMenu();
            waitFrames(10);
        }
        if (joyState & BUTTON_A) {
            selectMenuOption();
        }
    }
}

void drawMenu() {
    // Clear menu area
    VDP_clearTextArea(COMBAT_MENU_X, COMBAT_MENU_Y, 12, 6);
    
    const char* options[] = {"ATTACK", "TECH", "ITEM", "FLEE"};
    
    for (u8 i = 0; i < 4; i++) {
        // Draw cursor
        if (i == menuCursor) {
            VDP_drawText(">", COMBAT_MENU_X, COMBAT_MENU_Y + i);
        }
        VDP_drawText(options[i], COMBAT_MENU_X + 2, COMBAT_MENU_Y + i);
    }
}

void selectMenuOption() {
    switch(menuCursor) {
        case 0: // ATTACK
            selectingTarget = TRUE;
            targetCursor = 0;
            showMessage("SELECT TARGET");
            break;
            
        case 1: // TECH
            // Show tech menu (if any learned)
            if (party[0].techniques[0] == 0) {
                showMessage("NO TECHNIQUES!");
            } else {
                // Open tech submenu
            }
            break;
            
        case 2: // ITEM
            if (inventoryCount == 0) {
                showMessage("NO ITEMS!");
            } else {
                changeState(STATE_MENU); // Item menu
            }
            break;
            
        case 3: // FLEE
            tryFlee();
            break;
    }
}

void executePlayerAction(u8 target) {
    PartyMember* attacker = &party[0]; // Current active character
    Enemy* defender = &currentEnemies[target];
    
    // Calculate damage: (Strength * 2) - Defense + random(0-3)
    u8 baseDamage = (attacker->stats.strength * 2) - defender->stats.defense;
    u8 variance = random() % 4;
    u8 damage = baseDamage + variance;
    
    if (damage < 1) damage = 1;
    
    // Apply damage
    if (defender->stats.hp > damage) {
        defender->stats.hp -= damage;
    } else {
        defender->stats.hp = 0;
        // Enemy defeated!
        SPR_releaseSprite(defender->sprite);
    }
    
    // Show damage number
    char dmgText[4];
    intToStr(damage, dmgText, 3);
    showMessage(dmgText);
    
    // Check victory
    if (checkVictory()) {
        combatPhase = PHASE_VICTORY;
        showMessage("VICTORY!");
        awardRewards();
        return;
    }
    
    // Next turn
    currentTurn++;
    nextTurn();
}

void executeEnemyAI(u8 enemyIndex) {
    Enemy* attacker = &currentEnemies[enemyIndex];
    
    // Simple AI: Attack random living party member
    u8 target;
    do {
        target = random() % partyCount;
    } while (party[target].stats.hp == 0);
    
    PartyMember* defender = &party[target];
    
    // Calculate damage
    u8 baseDamage = (attacker->stats.strength * 2) - defender->stats.defense;
    u8 variance = random() % 4;
    u8 damage = baseDamage + variance;
    
    if (damage < 1) damage = 1;
    
    // Apply damage
    if (defender->stats.hp > damage) {
        defender->stats.hp -= damage;
    } else {
        defender->stats.hp = 0;
        // Party member defeated!
    }
    
    // Show message
    char msg[32];
    strcpy(msg, attacker->name);
    strcat(msg, " ATTACKS!");
    showMessage(msg);
    
    // Check defeat
    if (checkDefeat()) {
        combatPhase = PHASE_DEFEAT;
        showMessage("DEFEAT...");
        return;
    }
    
    // Next turn
    currentTurn++;
    nextTurn();
}

bool checkVictory() {
    for (u8 i = 0; i < enemyCount; i++) {
        if (currentEnemies[i].stats.hp > 0) return FALSE;
    }
    return TRUE;
}

bool checkDefeat() {
    for (u8 i = 0; i < partyCount; i++) {
        if (party[i].stats.hp > 0) return FALSE;
    }
    return TRUE;
}

void tryFlee() {
    // Flee chance based on agility difference
    u8 partyAgi = party[0].stats.agility;
    u8 enemyAgi = currentEnemies[0].stats.agility;
    
    if (partyAgi + (random() % 10) > enemyAgi) {
        combatPhase = PHASE_ESCAPE;
        showMessage("ESCAPED!");
        waitFrames(60);
        endCombat(FALSE);
    } else {
        showMessage("CAN'T ESCAPE!");
        currentTurn++;
        nextTurn();
    }
}

void awardRewards() {
    u16 totalExp = 0;
    
    for (u8 i = 0; i < enemyCount; i++) {
        totalExp += currentEnemies[i].expReward;
        
        // Item drop check
        if ((random() % 100) < currentEnemies[i].dropRate) {
            // Add to inventory
            if (inventoryCount < MAX_INVENTORY) {
                inventory[inventoryCount++] = currentEnemies[i].itemDrop;
            }
        }
    }
    
    // Distribute EXP
    for (u8 i = 0; i < partyCount; i++) {
        if (party[i].stats.hp > 0) {
            party[i].stats.exp += totalExp;
            checkLevelUp(&party[i]);
        }
    }
    
    char msg[32];
    strcpy(msg, "GAINED ");
    char expStr[6];
    intToStr(totalExp, expStr, 5);
    strcat(msg, expStr);
    strcat(msg, " EXP!");
    showMessage(msg);
}

void checkLevelUp(PartyMember* member) {
    // Simple level curve: level * 100 exp needed
    u16 needed = member->stats.level * 100;
    
    while (member->stats.exp >= needed) {
        member->stats.exp -= needed;
        member->stats.level++;
        
        // Stat gains
        member->stats.maxHp += 5;
        member->stats.maxMp += 2;
        member->stats.strength += 1;
        member->stats.mental += 1;
        member->stats.agility += 1;
        member->stats.defense += 1;
        
        // Heal on level up
        member->stats.hp = member->stats.maxHp;
        member->stats.mp = member->stats.maxMp;
        
        // Learn new techniques at certain levels
        learnTechniques(member);
        
        needed = member->stats.level * 100;
    }
}

void learnTechniques(PartyMember* member) {
    // Queeny's technique learning based on alignment
    if (strcmp(member->name, "QUEENY") == 0) {
        switch(member->stats.level) {
            case 2:
                if (queeny.alignment == ALIGN_HEROINE) {
                    member->techniques[0] = TECH_HEAL;      // Good path
                } else if (queeny.alignment == ALIGN_ANTAGONIST) {
                    member->techniques[0] = TECH_DRAIN;     // Evil path
                } else {
                    member->techniques[0] = TECH_NEUTRAL;   // Balanced
                }
                break;
            case 5:
                // More powerful techniques
                break;
        }
    }
}

void endCombat(bool victory) {
    combatActive = FALSE;
    
    // Cleanup sprites
    for (u8 i = 0; i < enemyCount; i++) {
        if (currentEnemies[i].stats.hp > 0) {
            SPR_releaseSprite(currentEnemies[i].sprite);
        }
    }
    
    if (victory) {
        // Return to overworld
        changeState(STATE_OVERWORLD);
    } else {
        // Game over or retry
        // For now, just heal and retry
        party[0].stats.hp = party[0].stats.maxHp;
        changeState(STATE_OVERWORLD);
    }
}

void cleanupCombat() {
    VDP_clearPlan(BG_A, TRUE);
    VDP_clearPlan(BG_B, TRUE);
    clearMessage();
}

// Utility
void waitFrames(u16 frames) {
    for (u16 i = 0; i < frames; i++) {
        SYS_doVBlankProcess();
    }
}

void intToStr(u16 num, char* str, u8 digits) {
    // Simple integer to string
    for (u8 i = digits; i > 0; i--) {
        str[i-1] = '0' + (num % 10);
        num /= 10;
    }
    str[digits] = '\0';
}
