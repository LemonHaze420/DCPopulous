#ifndef _POPULOUS_H_
#define _POPULOUS_H_

#include	"Object.h"
#include	"SafeProgramming.h"
#include	<windows.h>
#include	"GameWindow.h"
#include	"GraphicsClasses.h"
#include	"SoundSystem.h"
#include	"Sample.h"
#include	"Controls.h"
#include	"Platform.h"
#include	"Preferences.h"
#include	"Localization.h"
#include	"Timer.h"
#include	"Frontend.h"

// Original game headers
#include	"God.h"
#include	"Extern.h"


#define		MAX_RANDOM_NUMBERS	300
#define		MAX_NUM_OF_SAMPLES	128
#define		move_screen				copy_screen


#ifdef NOT_FINAL_BUILD
	#define		DEBUG_CHECKPEEPS	DebugCheckPeeps();
#else
	#define		DEBUG_CHECKPEEPS
#endif



// ----
// Enumerated type for game state.
enum EGameState
{
	GameStarting		= 0,
	// Expand on this later.

	GameError			= -1
};

// ----
// This class is the entire game - mapped from a procedural style into a semi-OO style.
class CPopulous : public CObject
{
public:
	CPopulous();
	~CPopulous();

	// ----
	// Sets up game control data, opens application window.
	// Returns true only if everything succeeded.
	bool	Init(	HINSTANCE	_hInstance,
					LPTSTR		_lpCmdLine,
					int			_nCmdShow);

	// ----
	// Returns true if all the files required by the class
	// can be located. Otherwise false.
	bool		ValidateFiles();

	// ----
	//Runs the tilte screen, this will either run the demo or
	//if the player starts a new game it sets selected and returns
	//Then game is run and the selected game is chosen.
	int 	title();
	void	OptionsScreen();
	//No not a cry for help, but the help screens........ ha...ha.................ha
	void		Help();

	void		RegistrationScreen();

	// ----
	// This method is where the whole populous game runs.
	// The method simply returns true to indicate that no
	// errors occured, and false that something went wrong,
	// and that we are to say sorry.
	bool		Run();

	// ----
	// Returns true if the game stae was set to Game_Error
	bool	DidAnErrorOccur()	{return (GameState == GameError);}

	// ----
	bool	WasFileMissingError()	{return FileMissing;}

	CPreferences	*GetPreferences() {return ThePreferences;}

protected:

	// Put all the internal game function here. We can then
	// call them from anywhere within Run (or any method of
	// this class) as if it were a procedural program.

	// ----
	// ARC
	// ----

	// ----
	void	Setscreen(LONG work, LONG display, SHORT res);
	void	swap_screens();
	void	a_putpixel(USHORT xy, USHORT color);
	int		get_a_dir(UBYTE   *dirname, UBYTE   *buffer);
	void	set_baud_rate();
	int		check_serial(SHORT num);

	// ----
	// 
	// ----

	// ----


	// ----
	// BATTLE
	// ----

	// ----
	void	do_battle(p_peeps *peep, SHORT peep_pos);
	void	battle_over(SHORT won_pos, SHORT lost_pos);
	void	set_battle(SHORT pos1, SHORT pos2);
	void	join_battle(SHORT pos1, SHORT pos2);


	// ----
	// COMPUTER
	// ----

	// ----
	int		make_level(SHORT pos, SHORT who);
	void	one_block_flat(SHORT pos, SHORT who);
	void	devil_effect(SHORT who);
	void	do_computer_effect(SHORT who, s_stats *stat);
	void	set_devil_magnet(SHORT who);




	// ----
	// EFFECT
	// ----

	// ----
	void	do_magnet(SHORT who, SHORT x, SHORT y);
	void	rotate_all_map();
	void	clear_all_map();
	void	place_people(SHORT who, SHORT x, SHORT y, SHORT leader);
	void	do_lower_point(SHORT who, SHORT x, SHORT y);
	void	do_raise_point(SHORT who, SHORT x, SHORT y);
	int		sculpt();
	void	do_flood(SHORT who);
	void	place_first_people();
	void	do_quake(SHORT who, SHORT x, SHORT y);
	void	do_volcano(SHORT who, SHORT x, SHORT y);
	void	set_magnet_to(SHORT who, SHORT where);
	void	do_swamp(SHORT who, SHORT start_x, SHORT start_y);
	void	do_knight(SHORT who);
	void	do_war(SHORT who);
	void	make_woods_rocks();
	void	do_funny();
	void	do_place_funny(SHORT monster, SHORT pos = 0);


	// ----
	// IO
	// ----

	// ----
	int		save_game(UBYTE *name);
	int		checksum(USHORT *data,LONG length);
	void	load_game(UBYTE *name);
	void	get_message(SHORT wait);
	void	clear_send();
	int		write_full(UBYTE *send);
	int		read_full(UBYTE *recieve,UBYTE *send);
	int		do_action(SHORT c1,SHORT x,SHORT y);

	
	// ----
	// LOAD_SOUND
	// ----
	
	// ----
	void	PlayMeas();
	void	load_sound(UBYTE *fname);
	void	kill_effect(SHORT from, SHORT to);
	int		check_effect(SHORT no);
	
	
	
	// ----
	// MAP
	// ----

	// ----
	void	setup_display(SHORT blank, SHORT landscape);
	SHORT	animate();
	void	clear_map();
	SHORT	raise_point(SHORT x, SHORT y);
	void	make_alt();
	void	make_thing(SHORT g_x, SHORT g_y);
	void	make_map(SHORT x1, SHORT y1, SHORT x2, SHORT y2);
	void	draw_map(SHORT x1, SHORT y1, SHORT x2, SHORT y2);
	void	mod_map(int x1, int y1, int x2, int y2);
	void	zoom_map(SHORT lr_button);
	void	set_tend_icons(SHORT x, SHORT y);
	void	set_mode_icons(SHORT x, SHORT y);
	SHORT	lower_point(SHORT x, SHORT y);
	void	interogate();
	void	show_the_shield();
	void	set_temp_view(SHORT pos);
	void	move_mana();


	// ----
	// MISC
	// ----

	// ----
	void	ms_off(){}
	void	ms_on(){}
	void	make_arc_mouse(int a){}
	void	kill_effect(int a, int b){}
	void	toggle_icon(int _Screen, int _XPos, int _YPos, int _Bank);

	// --
	// The functionality of these methods is as yet unknown.
	void	os_byte(int a, int *x, int *y){}
	void	os_read_var_val(char *x, char *y, int z){}


	// These are as yet not implemented - and possibly not looked in to.
	void	set_serial(){}
	void	clear_serial(){}
	void	r_ser_buff(){}
	void	w_ser_buff(){}
	int		read_serial(	void *a, int b, t_text *c){return 0;}
	int		write_serial(	void *a, int b, t_text *c){return 0;}
	int		compress(UBYTE* a){return 0;}
	void	decompress(UBYTE* a, int b){}

	void	setup_serial_interrupt(){}
	void	waittof();
	
	// --
	// Reimplemented in C.
	int		check_life(int player, int position);
	int		valid_move(int Pos, int PosToCheck);
	void	put_block(	int	_XPos,
						int	_YPos,
						int	_ZPos,
						int	_Block);
	void	draw_it(int xOff, int yOff);
	void	move_sprite(int x, int y, int alt, UBYTE*	r9, UBYTE*	r11, UBYTE*	r12);//(int x, int y, int alt, p_peeps *who, int mapGridRef);
	void	place_in_sprite(long r1, long r4, long r5, long r6, p_peeps	*r10);
	void	play_sound(USHORT number, USHORT channel);
	void	mod_play_sound(USHORT number, USHORT channel);
	void	draw_mouth(int _Screen, int _XPos, int _YPos, int _Height, int _Mouth);

	// --
	// These are implemented as wrappers.
	void	text(	int		_Screen, 
					int		_XPos,
					int		_YPos, 
					UBYTE	*_String);
	int		keyboard();					// WARNING: This is probably incorrectly implemented. 
	void	wait_for_disc();

	void	copy_screen(int a, int b);
	void	pixel(int _Screen, int _XPos, int _YPos, int _Colour);
	void	draw_sprite(LONG _Screen, int _XPos, int _YPos, int _SpriteIndex);
	void	draw_s_32(LONG _Screen, int _XPos, int _YPos, int _SpriteIndex);
	void	draw_fixed_sprite(int x, int y, int z, LONG	GraphicsPointer); // Must look into which screen this actually is...
	void	draw_icon(int _Screen, int _XPos, int _YPos, int _Icon);

	void	draw_bar(int a, int b, int c, int d, int e, int f);
	void	clr_wsc();	// Copy screen
	void	bbc_cls();	// Null bodied
	void	change_palette(int _a);
	void	open_screen();

	//Implimented by asd. will scroll the map when the dpad is pressed
	void	CPopulous::DPadScroll();

	// --
	// These do not need to be implemented - they have been replaced.
	/*
	void	make_arc_sprites_8(char *x,LONG z){}
	void	make_arc_sprites_16_nomask(char *x,LONG z){}
	void	make_arc_sprites_16(char *x,LONG spriteData){}
	void	make_arc_sprites_32(char *x,LONG spriteData){}

	void	claim_sound_system(){}
	*/

	// --
	// Directly copied
	void	PlaySound(USHORT number, USHORT channel);


	// ----
	// NOT GAME
	// ----

	// ----
	void	requester(LONG screen,SHORT x,SHORT y,SHORT width,SHORT height,t_text *ender1,t_text *ender2);
	void	toggle_button(SHORT x,SHORT y,t_text *toggle);
	void	get_name(LONG screen,UBYTE *str,SHORT x,SHORT y,SHORT width,t_text *ender1,t_text *ender2);
	int		save_load(SHORT is_save);
	void	sort(SHORT n,UBYTE *buffer);
	SHORT	do_message(UBYTE *tmp,SHORT all);
	void	get_a_message(UBYTE *tmp);
	void	serial_message(UBYTE *tmp);
	int		two_players(SHORT in_serial);
	int		try_serial(SHORT you_are,SHORT use_random);
	int		check_cancel(t_text *ender1,t_text *ender2);
	void	put_in_string(UBYTE *into,UBYTE *from,SHORT width);
	void	game_options();
	void	set_options();
	void	options(SHORT who);
	void	start_game();
	void	end_game(SHORT lost);
	void	won_conquest(LONG score);
	void	code(UBYTE *str,SHORT rnd);
	int		decode(UBYTE *str);
	void	paint_the_map(UBYTE *send);
	void	waitfor(SHORT num);

	int		show_world();
	void	return_to_game();
	

	// ----
	// PEEPS
	// ----

	// ----
	void	move_peeps();
	void	move_explorer(p_peeps *peep, SHORT peep_pos);
	int		where_do_i_go(p_peeps     *peep, SHORT peep_pos);
	int		move_magnet_peeps(p_peeps     *peep, SHORT peep_pos);
	void	get_heading(p_peeps *peep);
	void	join_forces(SHORT pos1, SHORT pos2);
	void	zero_population(p_peeps *peep, SHORT peep_pos);
	int		set_frame(p_peeps *peep);
	void	set_town(p_peeps *peep, SHORT clear);


	// ----
	// POPULOUS
	// ----
	
	// ----
	void	closeall();
	void	create_mouse();
	SHORT	Random();
	

	// ----
	// READ_BLOCK
	// ----
	
	// ----
	void	read_back_scr();
	void 	read_sprites(SHORT number);
	int		load_ground(SHORT num, SHORT ok_cancel);
	int 	read_lord();
	void 	read_mouth();

	// ----
	// SOUND
	// ----

	// ----
	int		test_channel(SHORT Num);



	// ----
	// Misc
	void		TakePicture();
	int			newrand();
	void		SetRandomNumbers();



	// ----
	// FRONTEND
	int		title_page();
	void	start_people();
	void	do_people();
	int		do_rock();
	void	init_extras(SHORT last);
	int		process_extras(SHORT last);
	void	display();
	void	scale_alts();
	void	start_scroll();
	void	do_text(long* x);
	void	draw_map_block(long a, long b, long c, long d);
	void	draw_it();
	void	move_people(long r0, long r1, long r2, long r3, long frame, long bounce, long special);
	void	draw_building(long r0, long r1, long r2);

	SHORT	read_back_scr(UBYTE	*fname);
	void	load_graphics();
	void	load_screen(UBYTE	*fname);
	void	draw_s32(long _Screen, int _X, int _Y, int _Frame);
	void	res_head(long r0, long r1, long r2);
	void	show_text(long _TextLine, long _Screen, long _Qaz);
	
	// ----
	// Confirm quit dialog. 
	// Throws an exception to get out of the code. - Hmmm, blatant exception abuse.
	void	game_quit();

	// ----
	// This method calls code to update our variables, such
	// as the mouse position, and feed it through into the 
	// way Populous works.
	// A return of false indicates that the game should 
	// stop processing and return control to Windows.
	bool		UpdatePopulousData();

	// New method to take over the work of the mouse driver
	// callback (which played the music and sound effects).
	void		ProcessAudio();


	// ----
//	void	DrawController(int _X, int _Y);
	void	PointToPrimary();

	// ----
	// Flashy little battery low logo.
	void	CheckBattery();

	// ----
	// Debug routines to check the integrity of data and 
	// allow us to track down bugs.
	void		DebugCheckPeeps();





	// ----
	// Internal data.
	// Lots to go here.

	CString			TempString;
	char			convert[256];
	RECT			Region;

	CTimer			*Timer50ths;		// Counts 50ths of a second.
	CTimer			*Timer5ths;			// Counts 5ths of a second.
	CTimer			*GameFrameLimiter;	// Locks frame rate for game
	CTimer			*IntroFrameLimiter;	// Locks frame rate for intro.
	CTimer			*MusicTimer;		// Counts 50ths of a second - using a dedicated timer as Timer50ths has been programmatically abused! (Nice image?)
	CTimer			*BatteryTimer;		// Battery flash timer
//	CTimer			*MouseTimer;		// Times mouse auto-repeat speed.

	CGraphicsData	*BackGroundImage;	// Background images.
	CGraphicsData	*BlockGraphics;		// Map tiles.
	CGraphicsData	*SpriteGraphics;	// Sprites (16 pixel wide).
	CGraphicsData	*Sprite32Graphics;	// Sprites (32 pixel wide).
	CGraphicsData	*IconGraphics;		// Icons.
	CGraphicsData	*MouthGraphics;		// Mouths.
	CGraphicsData	*ToggleIconGraphics;// Mask for toggling icons.
	CGraphicsData	*TheKeyboardGraphic;
	CGraphicsData	*HeadGraphics;
	CGraphicsData	*BigFont;
	CGraphicsData	*TheMenuCornerGraphics;
	CGraphicsData	*HelpIconGraphics;
//	CGraphicsData	*PDAGraphics;
//	CGraphicsData	*PDAKeysGraphics;
	
	CFont			*StandardFont;		// Font.

	PopulousInstrumentStruct	SampleArray[MAX_NUM_OF_SAMPLES];	// Array of pointers to sample data

	int	RandomArrayLoc;
	int RandomArray[MAX_RANDOM_NUMBERS];

	
	SHORT			*measln;	// 128 bytes. Load: dat.measln
	SHORT			seqlen;		// 2 bytes. Load: dat.seqlen
	struct SeqItem	sequn[8];	// 32 bytes. Load: dat.sequn
	UBYTE			*measures;	// 16384 bytes (16.0 Kb) Load: dat.measures


	// ----
	// Low level objects - these make the whole thing work.
	EGameState		GameState;
	CGameWindow		*TheGameWindow;			// Game window.
	HINSTANCE		HInstance;

	CPlatform		*ThePlatform;			// Platform information.
	CSoundSystem	*TheSoundSystem;		// Sound system.
	CPreferences	*ThePreferences;		// Preferences.
	CLocalization	*TheLocalization;		// Localization strings.
	bool			FileMissing;			// Set when a file goes missing.

	// Aliases
	CGraphics		*GraphicsRef;
	CControls		*ControllerRef;
	bool			Redraw;
	int				RedrawCount;
};
#endif