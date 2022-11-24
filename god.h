#ifndef _GOD_H_
#define	_GOD_H_

#include	<windows.h>
/* // These are defined in Windows.h
typedef signed char BYTE;
typedef short SHORT;
typedef unsigned short USHORT;
typedef long LONG;
typedef unsigned long ULONG;
*/

typedef signed char SBYTE;
typedef unsigned char UBYTE;

#define MAP_WIDTH           64
#define MAP_HEIGHT          64
#define END_WIDTH           65
#define END_HEIGHT          65
#define GET_POS(x,y)        (x+(y<<6))
#define GET_X(x)            ((x) & (MAP_WIDTH-1))
#define GET_Y(x)            ((x) >> 6)
#define PEOPLE              0x5f
#define DEVIL_PEOPLE        0x5d
#define PLACE_TREE          0x5b
#define PLACE_ROCK          0x59
#define CLEAR_WHO           0x57
#define GODS_MANA           0x55
#define DEVILS_MANA         0x53
#define ROTATE_MAP          0x51
//#define DEBUG               0x4f
#define POPULOUS_DEBUG               0x4f
#define REPORT              0x4d
#define CLEAR_MAP           0x73
#define FIRST_LAND          0xfd
#define LAST_LAND           0xed
#define QUIT                0x75
#define CONVERT_MAP         0x91
#define KILL_ALL            0xbf
#define BANK1               (4800)
#define BANK2               (BANK1+1316)
#define BUFFER_SIZE         120
#define CHEAT_1             0x45
#define ESCAPE              0x1b
#define HUMAN_PLAYER        0
#define COMPUTER_PLAYER     1
#define TRACKS              5
#define MAX_MENSA           4
#define MAX_NO_PEEPS        208
#define FUNNY               209
#define END_OF_FUNNYS       211
#define MAX_FOOD            (FLAT_LAND_FOOD*17+START_FOOD)
#define MAX_RECORD_LEN      8000
#define END_OF_PLAGUE       100
#define SPRITE_DATA         22880L
#define SPRITE_DATA_32      8320L
#define BLK_DATA            33600L
#define BACK_SCREEN         32000L
#define SCREEN_SIZE         32000L
#define COPPER_LIST         300L
#define BATTLE_FIRST_FRAME  70
#define BATTLE_LAST_FRAME   73
#define GOD                 0
#define DEVIL               1
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
#define START_FOOD          50
#define FLAT_LAND_FOOD      15
#define ROCK_LAND_FOOD      -15
#define WATER_FOOD          20
#define CITY_FOOD           MAX_FOOD*10
#define IN_TOWN             0x01
#define ON_MOVE             0x02
#define IN_EFFECT           0x04
#define IN_BATTLE           0x08
#define IN_WATER            0x10
#define WAIT_FOR_ME         0x20
#define I_AM_WAITING        0x40
#define IN_RUIN             0x80
#define LEAVING_TOWN        1
#define IN_FIGHT            3
#define S_EFFECT            0
#define S_X                 1
#define S_Y                 2
#define S_RAND              3
#define S_CHECKSUM          4
#define S_RAISE             1
#define S_LOWER             2
#define S_MAGNET            5
#define S_PLACE_GODS        7
#define S_PLACE_DEVILS      8
#define S_PLACE_GODS_LEADER 9
#define S_PLACE_DEVILS_LEADER   10
#define S_PLACE_TREES       11
#define S_PLACE_ROCKS       12
#define S_CLEAR_WHO         13
#define S_ACTION            14
#define S_CHECK             15
#define SA_MODE             1
#define SA_MESSAGE          2
#define SA_MANA_WAR         3
#define SA_MANA_FLOOD       4
#define SA_MANA_KNIGHT      5
#define SA_PAUSE            6
#define SA_COMPUTER_OPTIONS 7
#define SA_GAME_OPTIONS     8
#define SA_GODS_MANA        9
#define SA_DEVILS_MANA      10
#define SA_ROTATE_MAP       11
#define SA_CLEAR_MAP        12
#define SA_LOAD_LAND        13
#define SA_ACTION           14
#define SA_CHEAT            15
#define	SA_QUIT				16
#define POPULATION_GROWTH   2
#define DUMMY_FRAME         255
#define MAGNET_MODE         0
#define SETTLE_MODE         1
#define JOIN_MODE           2
#define FIGHT_MODE          3
#define MOVE_TO_FLAT        0
#define MOVE_TO_BATTLE      1
#define MOVE_TO_ENEMY       2
#define MOVE_TO_FRIEND      3
#define MOVE_TO_EMPTY       4
#define INTEROGATE          0x01
#define SCULPT              0x02
#define PUT_MAGNET          0x04
#define PUT_SWAMP           0x08
#define PUT_PLAGUE          0x10
#define MANA_POINT          1
#define MANA_MAGNET         2
#define MANA_QUAKE          3
#define MANA_SWAMP          4
#define MANA_KNIGHT         5
#define MANA_VOLCANO        6
#define MANA_FLOOD          7
#define MANA_WAR            8
#define TUNE_SWAMP          66
#define TUNE_SWORD1         67
#define TUNE_SWORD2         68
#define TUNE_KNIGHTED       69
#define TUNE_FIRE           70
#define TUNE_HEART1         71
#define TUNE_HEART2         72
#define TUNE_WAR            73
#define TUNE_FLOOD          74
#define TUNE_VOLCANO        75
#define TUNE_QUAKE          76
#define TUNE_MAGNET         77
#define C_LAND              0x0001
#define C_TOWN              0x0002
#define C_LEADER            0x0004
#define C_QUAKE             0x0008
#define C_SWAMP             0x0010
#define C_KNIGHT            0x0020
#define C_VOLCANO           0x0040
#define C_FLOOD             0x0080
#define C_WAR               0x0100
#define G_WATER_FATAL       0x0001
#define G_SWAMP_REMAIN      0x0002
#define G_NO_BUILD          0x0004
#define G_ONLY_RAISE        0x0008
#define G_RAISE_TOWN        0x0010
#define NO_WOODS            22
#define NO_TREES            30
#define DEVIL_MAKES_KNIGHT  3000

struct  p_peeps
    {
    UBYTE       flags;
    UBYTE       player;
    UBYTE       iq;
    UBYTE       weapons;
    SHORT       population;
    USHORT      battle_population;
    SHORT       at_pos;
    SHORT       direction;
    SHORT       frame;
    struct p_peeps  *head_for;
    SHORT       in_out;
    BYTE        status;
    BYTE        magnet_last_move;
    };
struct  m_magnet
    {
    SHORT   carried;
    SHORT   go_to;
    SHORT   flags;
    SHORT   no_towns;
    LONG    population;
    LONG    mana;
    };
struct  s_sprite
    {
    SHORT   x;
    SHORT   y;
    SHORT   frame;
    SHORT   who;
    };
struct  r_record
    {
    UBYTE   who;
    UBYTE   effect;
    UBYTE   x;
    UBYTE   y;
    };
struct t_text
    {
    SHORT   x,y,width;
    UBYTE   text[40];
    };
struct  s_stats 
    {
    UBYTE   send[6];
    SHORT   type;
    SHORT   computer_done;
    SHORT   land_effect;
    SHORT   computer_skill;
    USHORT  computer_mode;
    SHORT   computer_speed;
    SHORT   quake_count;
    SHORT   no_castles;
    SHORT   no_towns;
    SHORT   no_swamps;
    SHORT   no_quakes;
    SHORT   arrived;
    SHORT   last_battle;
    SHORT   house_color;
    struct  p_peeps *best1;
    struct  p_peeps *best2;
    struct  p_peeps *mybest;
    };
struct  c_conquest
    {
    UBYTE   skill;
    UBYTE   speed;
    UBYTE   his_mode;
    UBYTE   my_mode;
    UBYTE   game_mode;
    UBYTE   land;
    UBYTE   my_pop;
    UBYTE   his_pop;
    SHORT   seed;
    };
struct  s_serial
    {
    SHORT   read_pos;
    SHORT   write_pos;
    SHORT   write_ready;
    USHORT  baud_rate;
    UBYTE   buffer[128];
    UBYTE   error[128];
    };
struct  f_funny 
    {
    SHORT   dir;
    SHORT   start;
    SHORT   end;
    SHORT   kill[3];
    };

struct SeqItem
{
	USHORT	Measure;
	USHORT	Times;
};

#endif