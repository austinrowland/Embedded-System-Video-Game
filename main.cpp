// Author: Austin Rowland
// Basic Shell Code provided by Georgia Tech ECE 2035 Fall 2021 TAs

// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"

// NOTE: If game is freezing, most likely an issue with the wiring connections to SD card reader.

// Added Global Variables in this file
bool startScreen = true;
bool quest1Started = false;
bool game1Started = false;
bool game1Ended = false;
bool quest1Completed = false;
bool chestDirections = false;
bool chestOpened = false;
bool initBossMap = false;
char toBossMap, fromBossMap;
int npc1InteractionNum = 0;
int boss1InteractionNum = 0;
bool bluePillSelected = false;
bool redPillSelected = false;
bool playerWon = false;
bool playerLost = false;
int numEnemies = 3;

// Global Dialogue Strings
// Naming Convention: Character_InteractionNumber_LineNumber
char NPC1_0_1[] = "Hey, my name is";
char NPC1_0_2[] = "Brian!";
char NPC1_1_1[] = "I need your help";
char NPC1_1_2[] = "to beat my twin.";
char NPC1_2_1[] = "You must go to";
char NPC1_2_2[] = "the portal & win!";
char NPC1_3_1[] = "YOU DID IT!!";
char NPC1_3_2[] = "Thank you!";
char NPC1_4_1[] = "Go northeast to";
char NPC1_4_2[] = "find the chest.";

char Boss1_0_1[] = "AYO, my name is";
char Boss1_0_2[] = "Evil Brian!";
char Boss1_1_1[] = "Let's play a";
char Boss1_1_2[] = "game of luck.";
char Boss1_2_1[] = "I win, you die!";
char Boss1_2_2[] = "You win, I die!";
char Boss1_3_1[] = "Choose wisely.";
char Boss1_3_2[] = "Red or Blue!";

// Global Direction Strings
char mainMapPortalBlock1[] = "Cannot enter.";
char mainMapPortalBlock2[] = "Talk to the NPC.";
char bluePillSelect[] = "I'm allergic to";
char bluePillSelect2[] = "blue. Oh no!";
char bluePillSelect3[] = "Gotta get rid of";
char bluePillSelect4[] = "my ke-";
char redPillSelect[] = "You fool, this";
char redPillSelect2[] = "ain't the matrix.";
char redPillSelect3[] = "You're alive?";
char redPillSelect4[] = "What the?";
char redPillSelect5[] = "Don't go to";
char redPillSelect6[] = "the blue!";
char BossPortalBlock[] = "Talk to the Boss.";
char gamePortalBlock[] = "Pick red or blue.";
char keyPortalBlock[] = "Grab the key.";
char keyPickup[] = "Picked up key.";
char chestBlock1[] = "Cannot open.";
char chestBlock2[] = "Talk to the NPC.";
char swordPickup[] = "Picked up sword.";
char finalBattle1[] = "Defeat enemies";
char finalBattle2[] = "in world corners.";
char finalBattle3[] = "Beware, spikers";
char finalBattle4[] = "will hurt you.";
char endGame1[] = "You saved us all.";
char endGame2[] = "Thank you Kevin.";
char lostGame1[] = "Nooo Kevin!";
char lostGame2[] = "We are doomed.";
char attackSpeech1[] = "You used sword.";
char attackSpeech2[] = "You took damage.";

// Functions in this file
int get_action (GameInputs inputs);
int update_game (int action);
void draw_game (int init);
void init_main_map ();
void init_boss_map ();
int main ();

// Update Game Functions
void portal_to();
void portal_from();
void npc_speak();
void boss1_speak();
void boss1RedPill();
void boss1BluePill();
void playGameOverSound();
void playGameWonSound();
void playInteractionSound();
void playNotificationSound();
void decreaseLife();

/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    bool has_key;  // has key
    bool has_sword;  // has sword
    int health;  // health value
    bool omniMode;  // omni mode
    int numLives;  // number of lives
    // You can add other properties for the player here
} Player;

/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6
#define OMNI_MODE 7
int get_action(GameInputs inputs)
{
    if (!inputs.b1) {
        return ACTION_BUTTON;
    }
    if (!inputs.b2) {
        return MENU_BUTTON;
    }
    if (!inputs.b3) {
        return OMNI_MODE;
    }
    
    if (inputs.ax < -0.2) {
        return GO_LEFT;
    }
    if (inputs.ax > 0.2) {
        return GO_RIGHT;
    }
    if (inputs.ay > 0.2) {
        return GO_UP;
    }
    if (inputs.ay < -0.2) {
        return GO_DOWN;
    }
    return NO_ACTION;
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 * 
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2
int update_game(int action)
{
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;
    
    // Pointers to Check Certain Locations for Movement Conditions
    MapItem* North = get_north(Player.x, Player.y);
    MapItem* North2 = get_north(Player.x, Player.y - 1);
    MapItem* South = get_south(Player.x, Player.y);
    MapItem* South2 = get_south(Player.x, Player.y + 1);
    MapItem* West = get_west(Player.x, Player.y);
    MapItem* West2 = get_west(Player.x - 1, Player.y);
    MapItem* East = get_east(Player.x, Player.y);
    MapItem* East2 = get_east(Player.x + 1, Player.y);
    MapItem* Position = get_here(Player.x, Player.y);
    
    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {
        case GO_UP:
            if ((North->walkable) || (Player.omniMode)) {
                if (read_inputs().ay >= 0.7) {
                    if ((North2->walkable) || (Player.omniMode)) {
                        Player.y--;
                    }
                }
                Player.y--;
            }
            return NO_RESULT;
        case GO_LEFT:
            if ((West->walkable) || (Player.omniMode)) {
                if (read_inputs().ax <= -0.7) {
                    if ((West2->walkable) || (Player.omniMode)) {
                        Player.x--;
                    }
                }
                Player.x--;
            }
            return NO_RESULT;            
        case GO_DOWN:
            if ((South->walkable) || (Player.omniMode)) {
                if (read_inputs().ay <= -0.7) {
                    if ((South2->walkable) || (Player.omniMode)) {
                        Player.y++;
                    }
                }
                Player.y++;
            }
            return NO_RESULT;
        case GO_RIGHT:
            if ((East->walkable) || (Player.omniMode)) {
                if (read_inputs().ax >= 0.7) {
                    if ((East2->walkable) || (Player.omniMode)) {
                        Player.x++;
                    }
                }
                Player.x++;
            }
            return NO_RESULT;
        case ACTION_BUTTON:
            if ((Position->type == PORTAL) && (Position->data == &toBossMap)) {
                portal_to();
            }
            if ((Position->type == PORTAL) && (Position->data == &fromBossMap)) {
                portal_from();
            }
            if (North->type == NPC1 || South->type == NPC1 || West->type == NPC1 || East->type == NPC1) {
                add_speechBubble(npc1X, npc1Y - 1);
                draw_game(true);
                //playInteractionSound();
                npc_speak();
                map_erase(npc1X, npc1Y - 1);
            }
            if (North->type == BOSS1 || South->type == BOSS1 || West->type == BOSS1 || East->type == BOSS1) {
                add_speechBubble(boss1X, boss1Y - 1);
                draw_game(true);
                //playInteractionSound();
                boss1_speak();
                map_erase(boss1X, boss1Y - 1);
            }
            if ((Position->type == REDPILL) && (game1Started)) {
                boss1RedPill();
            }
            if ((Position->type == BLUEPILL) && (game1Started)) {
                boss1BluePill();
            }
            if (Position->type == KEY) {
                Player.has_key = true;
                speech(keyPickup, NULL);
                map_erase(boss1X, boss1Y);
                npc1InteractionNum += 1;
            }
            if (North->type == CHEST || South->type == CHEST || West->type == CHEST) {
                if (chestDirections) {
                    add_openedChest(chestX, chestY);
                    add_sword(chestX - 1, chestY);
                } else {
                    speech(chestBlock1, chestBlock2);
                }
            }
            if (Position->type == SWORD) {
                Player.has_sword = true;
                speech(swordPickup, NULL);
                map_erase(chestX - 1, chestY);
                speech(finalBattle1, finalBattle2);
                speech(finalBattle3, finalBattle4);
                add_enemy1(enemy1X, enemy1Y);
                add_enemy2(enemy2X, enemy2Y);
                add_enemy3(enemy3X, enemy3Y);
                npc1InteractionNum += 1;
            }
            if (North->type == ENEMY1 || South->type == ENEMY1 || West->type == ENEMY1 || East->type == ENEMY1) {
                Player.health = Player.health - 33;
                speech(attackSpeech1, attackSpeech2);
                map_erase(enemy1X, enemy1Y);
                numEnemies--;
            }
            if (North->type == ENEMY2 || South->type == ENEMY2 || West->type == ENEMY2 || East->type == ENEMY2) {
                Player.health = Player.health - 33;
                speech(attackSpeech1, attackSpeech2);
                map_erase(enemy2X, enemy2Y);
                numEnemies--;
            }
            if (North->type == ENEMY3 || South->type == ENEMY3 || West->type == ENEMY3 || East->type == ENEMY3) {
                Player.health = Player.health - 33;
                speech(attackSpeech1, attackSpeech2);
                map_erase(enemy3X, enemy3Y);
                numEnemies--;
            }
            return FULL_DRAW;
        case MENU_BUTTON:
            return FULL_DRAW;
        case OMNI_MODE:
            if (Player.omniMode) {
                Player.omniMode = false;
                Player.health = 100;
            } else {
                Player.omniMode = true;
                Player.health = 10000;
            }
            return FULL_DRAW;
        default:        break;
    }
    return NO_RESULT;
}

/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status 
 * bars. Unless init is nonzero, this function will optimize drawing by only 
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    // Draw game border first
    if(init) draw_border();
    
    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)
            
            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;
            
            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;
                        
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            
            // Figure out what to draw
            DrawFunc draw = NULL;
            if (i == 0 && j == 0)
            {
                draw_player(u, v, Player.has_key);
                continue;
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                        draw = draw_nothing;
                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }

    // Draw status bars    
    draw_upper_status(Player.x, Player.y, Player.health);
    draw_lower_status(Player.has_key, Player.has_sword, Player.numLives);
}


/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion. Note: using the similar
 * procedure you can init the secondary map(s).
 */
void init_main_map()
{
    // "Random" plants
    Map* map = set_active_map(0);
    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        add_plant(i % map_width(), i / map_width());
    }
    pc.printf("plants\r\n");
    
    add_portal(mainMapPortalX, mainMapPortalY);
    get_here(mainMapPortalX, mainMapPortalY)->data = &toBossMap;
    
    add_npc1(npc1X, npc1Y);
    
    map_erase(3, 1);
        
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    pc.printf("Walls done!\r\n");

    print_map();
}

/**
 * Initialize the boss world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion. Note: using the similar
 * procedure you can init the secondary map(s).
 */
void init_boss_map()
{
    // "Random" rocks
    for(int i = map_width() + 3; i < map_area(); i += 13)
    {
        add_rock(i % map_width(), i / map_width());
    }
    pc.printf("rocks\r\n");
    
    add_portal(bossMapPortalX, bossMapPortalY);
    get_here(bossMapPortalX, bossMapPortalY)->data = &fromBossMap;
    
    add_boss1(boss1X, boss1Y);
    
    add_bluePill(bluePillX, bluePillY);
    
    add_redPill(redPillX, redPillY);
    
    map_erase(boss1X, boss1Y - 1);
        
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    pc.printf("Walls done!\r\n");

    print_map();
}

/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
int main()
{
    // Useful for Debugging
    //while (true) {
        //GameInputs gameInputs = read_inputs();
        //uLCD.locate(0, 0);
        //uLCD.printf("X:%f\nY:%f", gameInputs.ax, gameInputs.ay);
    //}
    
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");
    
    // Initialize Start Screen
    
    uLCD.locate(4, 5);
    uLCD.printf("By: Austin");
    uLCD.locate(4, 6);
    uLCD.printf("    Rowland");
    uLCD.locate(4, 10);
    uLCD.printf("Press Start");
    uLCD.locate(3, 11);
    uLCD.printf("(Top Button!)");
    uLCD.locate(4, 2);
    uLCD.text_width(2);
    uLCD.text_height(2);
    uLCD.printf("Kevin");
    
    GameInputs inputs;
    int action;
    do {
        inputs = read_inputs();
        action = get_action(inputs);
    } while (action != ACTION_BUTTON);
    
    startScreen = false;
    uLCD.cls();
    
    
    // Initialize the maps
    maps_init();
    init_main_map();
    
    // Initialize game state
    set_active_map(0);
    Player.x = Player.y = 5;
    Player.has_key = false;
    Player.has_sword = false;
    Player.health = 100;
    Player.omniMode = false;
    Player.numLives = 3;

    // Initial game drawing
    draw_game(true);
    // Main game loop
    while(1)
    {
        // Timer to measure game update speed
        Timer t; t.start();
        
        // Actually do the game update:
        // 1. Read inputs  
        GameInputs inputs = read_inputs();     
        // 2. Determine action (get_action)
        int action = get_action(inputs);        
        // 3. Update game (update_game)
        int result = update_game(action);
        // 3b. Check for game over
        if (result == GAME_OVER) {
            break;
        }
        // 3c. Check if Health is less than 1
        if (Player.health <= 0) {
            speech(lostGame1, lostGame2);
            decreaseLife();
        }
        // 3d. Check if Number of Lives is 0
        if (Player.numLives <= 0) {
            playerLost = true;
            break;
        }
        // 3e. Check if numEnemies is 0
        if (numEnemies <= 0) {
            speech(endGame1, endGame2);
            playerWon = true;
            break;
        }
        // 4. Draw frame (draw_game)
        draw_game(result);
        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }
    uLCD.cls();
    uLCD.locate(5, 8);
   // End Game Conditions
    if (playerWon == true) {
        uLCD.background_color(GREEN);
        uLCD.printf("YOU WIN!!");
        playGameWonSound();
    }
    if (playerLost == true) {
        uLCD.background_color(RED);
        uLCD.printf("GAME OVER");
        playGameOverSound();
    }
}

void portal_to() {
    if (quest1Started) {
        set_active_map(1);
        if (!initBossMap) {
            init_boss_map();
            Player.x = bossMapPortalX;
            Player.y = bossMapPortalY;
        }
    } else {
        playNotificationSound();
        speech(mainMapPortalBlock1, mainMapPortalBlock2);
    }
}

void portal_from() {
    if ((Player.has_key == false) && (game1Ended == false) && (game1Started == true)) {
        //playNotificationSound();
        speech(gamePortalBlock, NULL);
    } else if ((Player.has_key == false) && (game1Ended == true) && (game1Started == true)) {
        //playNotificationSound();
        speech(keyPortalBlock, NULL);
    } else {
        map_erase(bossMapPortalX, bossMapPortalY);
        set_active_map(0);
        Player.x = mainMapPortalX;
        Player.y = mainMapPortalY;
        if (game1Ended == true) {
            map_erase(mainMapPortalX, mainMapPortalY);
        }
    }
}

void npc_speak() {
    // First Interaction (introduction and quest)
    if (npc1InteractionNum == 0) {
        speech(NPC1_0_1, NPC1_0_2);
        npc1InteractionNum += 1;
    }
    if (npc1InteractionNum == 1) {
        speech(NPC1_1_1, NPC1_1_2);
        npc1InteractionNum += 1;
    }
    if (npc1InteractionNum == 2) {
        speech(NPC1_2_1, NPC1_2_2);
        quest1Started = true;
    }
    // Second Interaction (after beating boss 1)
    if (npc1InteractionNum == 3) {
        speech(NPC1_3_1, NPC1_3_2);
        npc1InteractionNum += 1;
    }
    if (npc1InteractionNum == 4) {
        speech(NPC1_4_1, NPC1_4_2);
        chestDirections = true;
        add_unopenedChest(chestX, chestY);
    }
    if (npc1InteractionNum == 5) {
        speech(finalBattle3, finalBattle4);
    }
}

void boss1_speak() {
    if (boss1InteractionNum == 0) {
        speech(Boss1_0_1, Boss1_0_2);
        boss1InteractionNum += 1;
    }
    if (boss1InteractionNum == 1) {
        speech(Boss1_1_1, Boss1_1_2);
        boss1InteractionNum += 1;
    }
    if (boss1InteractionNum == 2) {
        speech(Boss1_2_1, Boss1_2_2);
        boss1InteractionNum += 1;
    }
    if (boss1InteractionNum == 3) {
        speech(Boss1_3_1, Boss1_3_2);
        game1Started = true;
    }
}

void boss1RedPill() {
    speech(redPillSelect, redPillSelect2);
    speech(redPillSelect3, redPillSelect4);
    speech(redPillSelect5, redPillSelect6);
    redPillSelected = true;
    Player.health = Player.health - 75;
    map_erase(redPillX, redPillY);
}

void boss1BluePill() {
    speech(bluePillSelect, bluePillSelect2);
    speech(bluePillSelect3, bluePillSelect4);
    bluePillSelected = true;
    game1Ended = true;
    add_keySprite(boss1X, boss1Y);
    map_erase(redPillX, redPillY);
    map_erase(bluePillX, bluePillY);
}

void playGameOverSound() {
    FILE *wave_file;
    
    wave_file=fopen("/sd/GameOverSound.wav","r");
    waver.play(wave_file);
    fclose(wave_file);
}

void playGameWonSound() {
    FILE *wave_file;
    
    wave_file=fopen("/sd/GameWonSound.wav","r");
    waver.play(wave_file);
    fclose(wave_file);
}

void playInteractionSound() {
    FILE *wave_file;
    
    wave_file=fopen("/sd/InteractionSound.wav","r");
    // Uncomment for debugging
    //if (wave_file == NULL) {
        //uLCD.locate(4, 15);
        //uLCD.printf("hi");
    //}
    waver.play(wave_file);
    fclose(wave_file);
}

void playNotificationSound() {
    FILE *wave_file;
    
    wave_file=fopen("/sd/NotificationSound.wav","r");
    // Uncomment for debugging
    //if (wave_file == NULL) {
    //    printf("hi");
    //}
    waver.play(wave_file);
    fclose(wave_file);
}

void decreaseLife() {
    // Initialize Maps Again
    maps_init();
    init_main_map();
    
    // Reinitialize Game State (except decrement numLives)
    set_active_map(0);
    Player.x = Player.y = 5;
    Player.has_key = false;
    Player.has_sword = false;
    Player.health = 100;
    Player.omniMode = false;
    Player.numLives--;
    
    // Reset Game Progress Variables
    startScreen = true;
    quest1Started = false;
    game1Started = false;
    game1Ended = false;
    quest1Completed = false;
    chestDirections = false;
    chestOpened = false;
    initBossMap = false;
    npc1InteractionNum = 0;
    boss1InteractionNum = 0;
    bluePillSelected = false;
    redPillSelected = false;
    playerWon = false;
    playerLost = false;
    numEnemies = 3;
}
