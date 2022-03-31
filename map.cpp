#include "map.h"

#include "globals.h"
#include "graphics.h"

/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 * In this file you need to define how the map will be structured. IE how will
 * you put values into the map, pull them from the map. Remember a "Map" variable
 * is a hashtable plus two ints (see below) 
 * You will have more than one map variable, one will be the main map with it's own hashtable.
 * Then you'll have a second map with another hashtable
 * You should store objects into the hashtable with different properties (spells
 * etc)
 */
struct Map {
    HashTable* items;
    int w, h;
};

/**
 * Storage area for the maps.
 * This is a global variable, but can only be access from this file because it
 * is static.
 */
static Map map[4];
static int active_map;
static unsigned int num_buckets = 4;

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) 
{
    // Creates a unique value for (x, y) coordinates by using formula:
    // Row Number (X) * Width of Row(map.h) + Column Number (Y)
    unsigned key = X*(map_width()) + Y;
    return key;
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key)
{
    // Performs the hash function of the key passed in.
    return (key % num_buckets);
}

void maps_init()
{
    // If Maps Exist (Game Started), wipe maps and reinitialize
    if (map[0].items != NULL) {
        destroyHashTable(map[0].items);
    }
    if (map[1].items != NULL) {
        destroyHashTable(map[1].items);
    }
    if (map[2].items != NULL) {
        destroyHashTable(map[2].items);
    }
    // TODO: Implement!    
    // Initialize hash table
    // Set width & height
    // Main Map
    map[0].items = createHashTable(map_hash, num_buckets);
    map[0].w = mainMapWidth;
    map[0].h = mainMapHeight;
    
    // Boss 1 Map
    map[1].items = createHashTable(map_hash, num_buckets);
    map[1].w = bossMapWidth;
    map[1].h = bossMapHeight;
    
    
}

Map* get_active_map()
{
    // There's only one map
    return &map[active_map];
}

Map* set_active_map(int m)
{
    active_map = m;
    return &map[active_map];
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'P'};
    for(int y = 0; y < map_height(); y++)
    {
        for (int x = 0; x < map_width(); x++)
        {
            MapItem* item = get_here(x,y);
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}

int map_width()
{
    Map* active_map = get_active_map();
    return active_map->w;
}

int map_height()
{
    Map* active_map = get_active_map();
    return active_map->h;
}

int map_area()
{
    Map* active_map = get_active_map();
    return ((active_map->w) * (active_map->h));
}

MapItem* get_north(int x, int y)
{
    Map *map = get_active_map();
    int posKey = XY_KEY(x, y - 1);
    MapItem* itemAtPosKey = (MapItem*)getItem(map->items, posKey);
    return itemAtPosKey;
}

MapItem* get_south(int x, int y)
{
    Map *map = get_active_map();
    int posKey = XY_KEY(x, y + 1);
    MapItem* itemAtPosKey = (MapItem*)getItem(map->items, posKey);
    return itemAtPosKey;
}

MapItem* get_east(int x, int y)
{
    Map *map = get_active_map();
    int posKey = XY_KEY(x + 1, y);
    MapItem* itemAtPosKey = (MapItem*)getItem(map->items, posKey);
    return itemAtPosKey;
}

MapItem* get_west(int x, int y)
{
    Map *map = get_active_map();
    int posKey = XY_KEY(x - 1, y);
    MapItem* itemAtPosKey = (MapItem*)getItem(map->items, posKey);
    return itemAtPosKey;
}

MapItem* get_here(int x, int y)
{
    Map *map = get_active_map();
    int posKey = XY_KEY(x, y);
    MapItem* itemAtPosKey = (MapItem*)getItem(map->items, posKey);
    return itemAtPosKey;
}


void map_erase(int x, int y)
{
    Map *map = get_active_map();
    int posKey = XY_KEY(x, y);
    MapItem* itemAtPosKey = (MapItem*)getItem(map->items, posKey);
    if (itemAtPosKey != NULL) {
        deleteItem(map->items, posKey);
    }
}

void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1->data = NULL;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}

void add_plant(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PLANT;
    w1->draw = draw_tree;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_portal(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PORTAL;
    w1->draw = draw_portal;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_rock(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = ROCK;
    w1->draw = draw_rock;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_npc1(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NPC1;
    w1->draw = draw_npc1;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_boss1(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = BOSS1;
    w1->draw = draw_boss1;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_keySprite(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = KEY;
    w1->draw = draw_keySprite;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_bluePill(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = BLUEPILL;
    w1->draw = draw_bluePill;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_redPill(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = REDPILL;
    w1->draw = draw_redPill;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_unopenedChest(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = CHEST;
    w1->draw = draw_unopenedChest;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_openedChest(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = CHEST;
    w1->draw = draw_openedChest;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_speechBubble(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = SPEECHBUBBLE;
    w1->draw = draw_speechBubble;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_sword(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = SWORD;
    w1->draw = draw_sword;
    w1->walkable = true;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_enemy1(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = ENEMY1;
    w1->draw = draw_enemy;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_enemy2(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = ENEMY2;
    w1->draw = draw_enemy;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}

void add_enemy3(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = ENEMY3;
    w1->draw = draw_enemy;
    w1->walkable = false;
    w1->data = NULL;
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}