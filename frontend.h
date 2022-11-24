//typedef char UBYTE;
//typedef short SHORT;
//typedef unsigned short USHORT;
//typedef long LONG;
//typedef unsigned long ULONG;

#define SPRITE_DATA_32      8320L
#define BLK_DATA            33600L
#define BACK_SCREEN         32000L
#define SCREEN_SIZE         32000L
#define COPPER_LIST         300L

#define FLAT_BLOCK          15
#define WATER_BLOCK         0
#define FARM_BLOCK          31
#define SWAMP_BLOCK         53
#define FIRST_TOWN          32
#define LAST_TOWN           42
#define CITY_TOWER          41
#define CITY_CENTRE         42
#define CITY_WALL1          43
#define CITY_WALL2          44
#define CROSS_BLOCK         45
#define SKULL_BLOCK         46
#define ROCK_BLOCK          47
#define LAST_ROCK_BLOCK     49
#define TREE_BLOCK          50
#define LAST_TREE_BLOCK     52
#define FIRST_RUIN_TOWN     54
#define BAD_LAND            66

#define FLAG_SPRITE         64
#define SHIELD_SPRITE       68
#define GODS_HAND_SPRITE    78
#define ANHK_SPRITE         80
#define SKULL_SPRITE        81
#define DEVILS_HAND_SPRITE  79
#define SWAMP_HAND_SPRITE   83
#define PLAGUE_HAND_SPRITE  82
#define CROSSHAIR_SPRITE    84
#define MANA_SPRITE         69
#define VICTORY_SPRITE      85
#define LAST_VICTORY_SPRITE 88
#define FIRST_WATER_SPRITE  93
#define LAST_WATER_SPRITE   96
#define FIRST_WAIT_SPRITE   101
#define LAST_WAIT_SPRITE    102
#define FIRST_KNIGHT_WATER  109
#define LAST_KNIGHT_WATER   112
#define KNIGHT_WAITS        125
#define TITLE_SPRITE        129
#define BLUE_VS_RED         138
#define BLUE_VS_PEEP        130
#define RED_VS_PEEP         134

struct w_walker
    {
    SHORT   image;          /* sprite image             */
    SHORT   pos;            /* position in from array   */
    SHORT   frame;          /* their paling frame       */
    SHORT   direction;      /* their direction          */
    SHORT   where;          /* where they are now       */
    SHORT   count;          /* no of count frames       */
    SHORT   special;
    };

struct  b_building
    {
    SHORT   which;
    SHORT   where;
    };
