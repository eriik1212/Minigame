// -------------------------------------------------------------------------
// Awesome simple game with SDL
// Lesson 2 - Input Events
//
// SDL API: http://wiki.libsdl.org/APIByCategory
// -------------------------------------------------------------------------

#include <stdio.h>			// Required for: printf()
#include <stdlib.h>			// Required for: EXIT_SUCCESS
#include <math.h>			// Required for: sinf(), cosf()
#include <time.h>			// Required for: srand(time(NULL))

// Include SDL libraries
#include "SDL/include/SDL.h"				// Required for SDL base systems functionality
#include "SDL_image/include/SDL_image.h"	// Required for image loading functionality
#include "SDL_mixer/include/SDL_mixer.h"	// Required for audio loading and playing functionality

// Define libraries required by linker
// WARNING: Not all compilers support this option and it couples 
// source code with build system, it's recommended to keep both 
// separated, in case of multiple build configurations
//#pragma comment(lib, "SDL/lib/x86/SDL2.lib")
//#pragma comment(lib, "SDL/lib/x86/SDL2main.lib")
//#pragma comment(lib, "SDL_image/lib/x86/SDL2_image.lib")
//#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

// -------------------------------------------------------------------------
// Defines, Types and Globals
// -------------------------------------------------------------------------
#define SCREEN_WIDTH		1280
#define SCREEN_HEIGHT		 720

#define MAX_KEYBOARD_KEYS	 256
#define MAX_MOUSE_BUTTONS	   5
#define JOYSTICK_DEAD_ZONE  8000

#define SHIP_SPEED			   8
#define SHIP_SPEED2			   8
#define MAX_SHIP_SHOTS		  32
#define SHOT_SPEED			  12
#define SCROLL_SPEED		   5

#define MAX_LIFE 10

enum WindowEvent
{
	WE_QUIT = 0,
	WE_HIDE,
	WE_SHOW,
	WE_COUNT
};

enum KeyState
{
	KEY_IDLE = 0,		// DEFAULT
	KEY_DOWN,			// PRESSED (DEFAULT->DOWN)
	KEY_REPEAT,			// KEEP DOWN (sustained)
	KEY_UP				// RELEASED (DOWN->DEFAULT)
};

enum GameScreen
{
	MENU = 0,
	CONTROLS,
	GAMEPLAY,
	ENDINGP1,
	ENDINGP2
};

struct Projectile
{
	int x, y;
	bool alive;
};

struct Player
{
	int alive = 1;

}p1;

struct Player2
{
	int alive2 = 1;

}p2;

struct meteorite
{
	bool alive;

}meteo[10];

// Global context to store our game state data
struct GlobalState
{
	// Window and renderer
	SDL_Window* window;
	SDL_Surface* surface;
	SDL_Renderer* renderer;

	// Input events
	KeyState* keyboard;
	KeyState mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x;
	int mouse_y;
	SDL_Joystick* gamepad;
	int gamepad_axis_x_dir;
	int gamepad_axis_y_dir;
	bool window_events[WE_COUNT];

	// Texture variables
	SDL_Texture* background;
	SDL_Texture* menu;
	SDL_Texture* controls;
	SDL_Texture* p1win;
	SDL_Texture* p2win;
	SDL_Texture* ship;
	SDL_Texture* shot;
	SDL_Texture* ship2;
	SDL_Texture* shot2;
	SDL_Texture* laser;
	SDL_Texture* meteorite;
	SDL_Texture* life1[MAX_LIFE];
	SDL_Texture* life2[MAX_LIFE];
	int background_width;

	// Audio variables
	Mix_Music* musicMenu;
	Mix_Music* musicIngame;
	Mix_Music* musicvictory;
	Mix_Chunk* fx_shoot;

	// Game elements
	int ShowHitBox = 0;
	int lser;

	int shot_w;
	int shot_h;

	int shot_w2;
	int shot_h2;

	int ship_x;
	int ship_y;
	int ship_w;
	int ship_h;

	int ship_x2;
	int ship_y2;
	int ship_w2;
	int ship_h2;

	int meteorite_x[10];
	int meteorite_y[10];
	int meteorite_w[10];
	int meteorite_h[10];

	int life1_x[MAX_LIFE];
	int life1_y[MAX_LIFE];
	int life1_w[MAX_LIFE];
	int life1_h[MAX_LIFE];

	int life2_x[MAX_LIFE];
	int life2_y[MAX_LIFE];
	int life2_w[MAX_LIFE];
	int life2_h[MAX_LIFE];

	Projectile shots[MAX_SHIP_SHOTS];
	Projectile shots2[MAX_SHIP_SHOTS];

	int last_shot;
	int scroll;

	GameScreen currentScreen;		// 0-LOGO, 1-TITLE, 2-GAMEPLAY, 3-ENDINGP1, 4-ENDINGP2
};

// Global game state variable
GlobalState state;

// Functions Declarations
// Some helpful functions to draw basic shapes
// -------------------------------------------------------------------------
static void DrawRectangle(int x, int y, int width, int height, SDL_Color color);
static void DrawLine(int x1, int y1, int x2, int y2, SDL_Color color);
static void DrawCircle(int x, int y, int radius, SDL_Color color);

// Functions Declarations and Definition
// -------------------------------------------------------------------------

void updateLifeIndicatorPlayer1(unsigned short* lifeIndicatorP1, unsigned short damage)
{
	for (int i = (MAX_LIFE - 1); i >= 0; --i) {

		if (*(lifeIndicatorP1 + i) == 0) {
			++damage;
		}
		else if (*(lifeIndicatorP1 + i) == 1 && i >= (MAX_LIFE - damage)) {
			*(lifeIndicatorP1 + i) = 0;
		}
	}
}

void updateLifeIndicatorPlayer2(unsigned short* lifeIndicatorP2, unsigned short damage)
{
	for (int i = 0; i < MAX_LIFE; ++i) {
		if (*(lifeIndicatorP2 + i) == 0) {
			++damage;
		}
		else if (*(lifeIndicatorP2 + i) == 1 && i < damage) {
			*(lifeIndicatorP2 + i) = 0;
		}
	}
}



void hitbox() {
	//LIFE ELEMENTS
	unsigned short life1[MAX_LIFE];
	unsigned short life2[MAX_LIFE];

	// Load Lifes P1 & P2
	for (int i = 0; i < 10; ++i) {
		life1[i] = 1;
	}
	for (int i = 0; i < 10; ++i) {
		life2[i] = 1;
	}

	//SHOTS
	for (int i = 0; i < MAX_SHIP_SHOTS; ++i)
	{
		if (state.ship_x2< state.shots[i].x + state.shot_w && state.ship_x2 + state.ship_w2>state.shots[i].x && state.ship_y2<state.shots[i].y + state.shot_h && state.ship_h2 + state.ship_y2>state.shots[i].y)
		{

			state.shots[i].alive=false;
			updateLifeIndicatorPlayer2(life2, 1);
			for (int i = 0; i < MAX_LIFE; ++i) {

				if (life2[i] == 0) {
					SDL_DestroyTexture(state.life2[i]);
				}

			}
			if (life2[MAX_LIFE - 1] == 0) {
				p2.alive2 = 0;
			}
		}

		if (state.ship_x< state.shots2[i].x + state.shot_w2 && state.ship_x + state.ship_w>state.shots2[i].x && state.ship_y<state.shots2[i].y + state.shot_h2 && state.ship_h + state.ship_y>state.shots2[i].y)
		{
			state.shots2[i].alive = false;
			updateLifeIndicatorPlayer1(life1, 1);

				for (int i = 0; i < MAX_LIFE; ++i) {

					if (life1[i] == 0) {
						SDL_DestroyTexture(state.life1[i]);
					}

					if (life1[0] == 0) {
						p1.alive = 0;
					}
				}
		}
	}
}


void Start()
{
	// Initialize SDL internal global state
	SDL_Init(SDL_INIT_EVERYTHING);

	// Init input events system
	//if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0) printf("SDL_EVENTS could not be initialized! SDL_Error: %s\n", SDL_GetError());

	// Init window
	state.window = SDL_CreateWindow("W Fight!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	state.surface = SDL_GetWindowSurface(state.window);

	// Init renderer
	state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(state.renderer, 100, 149, 237, 255);		// Default clear color: Cornflower blue

	// L2: DONE 1: Init input variables (keyboard, mouse_buttons)
	state.keyboard = (KeyState*)calloc(sizeof(KeyState) * MAX_KEYBOARD_KEYS, 1);
	for (int i = 0; i < MAX_MOUSE_BUTTONS; i++) state.mouse_buttons[i] = KEY_IDLE;

	// L2: DONE 2: Init input gamepad 
	// Check SDL_NumJoysticks() and SDL_JoystickOpen()
	if (SDL_NumJoysticks() < 1) printf("WARNING: No joysticks connected!\n");
	else
	{
		state.gamepad = SDL_JoystickOpen(0);
		if (state.gamepad == NULL) printf("WARNING: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
	}

	// Init image system and load textures
	IMG_Init(IMG_INIT_PNG);
	state.background = SDL_CreateTextureFromSurface(state.renderer, IMG_Load("Assets/Windowsxp.png"));
	state.menu = SDL_CreateTextureFromSurface(state.renderer, IMG_Load("Assets/MENU.png"));
	state.p1win = SDL_CreateTextureFromSurface(state.renderer, IMG_Load("Assets/endgamep1.png"));
	state.p2win = SDL_CreateTextureFromSurface(state.renderer, IMG_Load("Assets/endgamep2.png"));
	state.ship = SDL_CreateTextureFromSurface(state.renderer, IMG_Load("Assets/ship.png"));
	state.shot = SDL_CreateTextureFromSurface(state.renderer, IMG_Load("Assets/shotW1.png"));
	state.ship2 = SDL_CreateTextureFromSurface(state.renderer, IMG_Load("Assets/ship.png"));
	state.shot2 = SDL_CreateTextureFromSurface(state.renderer, IMG_Load("Assets/shotW.png"));
	state.meteorite = SDL_CreateTextureFromSurface(state.renderer, IMG_Load("Assets/meteorite_sprite.png"));
	for (int i = 0; i < MAX_LIFE; ++i) {
		state.life1[i] = SDL_CreateTextureFromSurface(state.renderer, IMG_Load("Assets/Heart.png"));
	}
	for (int i = 0; i < MAX_LIFE; ++i) {
		state.life2[i] = SDL_CreateTextureFromSurface(state.renderer, IMG_Load("Assets/Heart.png"));
	}

	SDL_QueryTexture(state.background, NULL, NULL, &state.background_width, NULL);

	// L4: TODO 1: Init audio system and load music/fx
	// EXTRA: Handle the case the sound can not be loaded!
	Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

	// Music
	state.musicMenu = Mix_LoadMUS("Assets/menu.ogg");
	state.musicIngame = Mix_LoadMUS("Assets/ingame.ogg");
	state.musicvictory = Mix_LoadMUS("Assets/victory.ogg");
	state.fx_shoot = Mix_LoadWAV("Assets/laser.wav");

	// L4: TODO 2: Start playing loaded music
	Mix_PlayMusic(state.musicMenu, -1);


	//Init srand
	srand(time(NULL));

	// Init game variables
	state.shot_w = 30;
	state.shot_h = 30;

	state.shot_w2 = 30;
	state.shot_h2 = 30;

	state.ship_x = 100;
	state.ship_y = SCREEN_HEIGHT / 2;
	state.ship_w = 64;
	state.ship_h = 64;

	state.ship_x2= 1180;
	state.ship_y2 = SCREEN_HEIGHT / 2;
	state.ship_w2 = 64;
	state.ship_h2 = 64;

	state.last_shot = 0;
	state.scroll = 0;

	state.meteorite_y[10] = SCREEN_HEIGHT;
	state.meteorite_x[10] = (rand() % SCREEN_WIDTH);
	state.meteorite_w[10] = 100;
	state.meteorite_h[10] = 20;

	int sepx1=0;
	for (int i = 0; i < MAX_LIFE; ++i) {
		state.life1_y[i] = 10;
		state.life1_x[i] = sepx1+=30;
		state.life1_w[i] = 20;
		state.life1_h[i] = 20;
	}

	int sepx2 = 0;
	for (int i = MAX_LIFE-1; i >= 0; --i) {
		state.life2_y[i] = 10;
		state.life2_x[i] = SCREEN_WIDTH - (sepx2+=30);
		state.life2_w[i] = 20;
		state.life2_h[i] = 20;
	}


	state.currentScreen = MENU;
}

// ----------------------------------------------------------------
void Finish()
{
	// L4: TODO 3: Unload music/fx and deinitialize audio system
	Mix_FreeMusic(state.musicMenu);
	Mix_FreeMusic(state.musicIngame);
	Mix_FreeMusic(state.musicvictory);
	Mix_FreeChunk(state.fx_shoot);
	Mix_CloseAudio();
	Mix_Quit();

	// Unload textures and deinitialize image system
	SDL_DestroyTexture(state.background);
	SDL_DestroyTexture(state.ship);
	SDL_DestroyTexture(state.ship2);
	SDL_DestroyTexture(state.meteorite);
	for (int i = 0; i < MAX_LIFE; ++i) {
		SDL_DestroyTexture(state.life1[i]);
	}
	for (int i = 0; i < MAX_LIFE; ++i) {
		SDL_DestroyTexture(state.life2[i]);
	}
	IMG_Quit();

	// L2: DONE 3: Close game controller
	SDL_JoystickClose(state.gamepad);
	state.gamepad = NULL;

	// Deinitialize input events system
	//SDL_QuitSubSystem(SDL_INIT_EVENTS);

	// Deinitialize renderer and window
	// WARNING: Renderer should be deinitialized before window
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);

	// Deinitialize SDL internal global state
	SDL_Quit();

	// Free any game allocated memory
	free(state.keyboard);
}

// ----------------------------------------------------------------
bool CheckInput()
{
	// Update current mouse buttons state 
    // considering previous mouse buttons state
	for (int i = 0; i < MAX_MOUSE_BUTTONS; ++i)
	{
		if (state.mouse_buttons[i] == KEY_DOWN) state.mouse_buttons[i] = KEY_REPEAT;
		if (state.mouse_buttons[i] == KEY_UP) state.mouse_buttons[i] = KEY_IDLE;
	}
    
	// Gather the state of all input devices
	// WARNING: It modifies global keyboard and mouse state but 
	// its precision may be not enough
	//SDL_PumpEvents();

	// Poll any currently pending events on the queue,
	// including 'special' events like window events, joysticks and 
	// even hotplug events for audio devices and joysticks,
	// you can't get those without inspecting event queue
	// SDL_PollEvent() is the favored way of receiving system events
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		switch (event.type)
		{
			case SDL_QUIT: state.window_events[WE_QUIT] = true; break;
			case SDL_WINDOWEVENT:
			{
				switch (event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST: state.window_events[WE_HIDE] = true; break;
					//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED: state.window_events[WE_SHOW] = true; break;
					case SDL_WINDOWEVENT_CLOSE: state.window_events[WE_QUIT] = true; break;
					default: break;
				}
			} break;
			// L2: DONE 4: Check mouse events for button state
			case SDL_MOUSEBUTTONDOWN: state.mouse_buttons[event.button.button - 1] = KEY_DOWN; break;
			case SDL_MOUSEBUTTONUP:	state.mouse_buttons[event.button.button - 1] = KEY_UP; break;
			case SDL_MOUSEMOTION:
			{
				state.mouse_x = event.motion.x;
				state.mouse_y = event.motion.y;
			} break;
			case SDL_JOYAXISMOTION:
			{
				// Motion on controller 0
				if (event.jaxis.which == 0)
				{
					// X axis motion
					if (event.jaxis.axis == 0)
					{
						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE) state.gamepad_axis_x_dir = -1;
						else if (event.jaxis.value > JOYSTICK_DEAD_ZONE) state.gamepad_axis_x_dir = 1;
						else state.gamepad_axis_x_dir = 0;
					}
					// Y axis motion
					else if (event.jaxis.axis == 1)
					{
						if (event.jaxis.value < -JOYSTICK_DEAD_ZONE) state.gamepad_axis_y_dir = -1;
						else if (event.jaxis.value > JOYSTICK_DEAD_ZONE) state.gamepad_axis_y_dir = 1;
						else state.gamepad_axis_y_dir = 0;
					}
				}
			} break;
			default: break;
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	// L2: DONE 5: Update keyboard keys state
    // Consider previous keys states for KEY_DOWN and KEY_UP
	for (int i = 0; i < MAX_KEYBOARD_KEYS; ++i)
	{
		// A value of 1 means that the key is pressed and a value of 0 means that it is not
		if (keys[i] == 1)
		{
			if (state.keyboard[i] == KEY_IDLE) state.keyboard[i] = KEY_DOWN;
			else state.keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if (state.keyboard[i] == KEY_REPEAT || state.keyboard[i] == KEY_DOWN) state.keyboard[i] = KEY_UP;
			else state.keyboard[i] = KEY_IDLE;
		}
	}

	// L2: DONE 6: Check ESCAPE key pressed to finish the game
	if (state.keyboard[SDL_SCANCODE_ESCAPE] == KEY_DOWN) return false;

	// Check QUIT window event to finish the game
	if (state.window_events[WE_QUIT] == true) return false;

	return true;
}

// ----------------------------------------------------------------
void MoveStuff()
{
	switch (state.currentScreen)
	{
	case MENU:
	{
		if (state.keyboard[SDL_SCANCODE_RETURN] == KEY_DOWN) state.currentScreen = CONTROLS;
	} break;
	case CONTROLS:
	{
		// Play Music Ingame
		Mix_FadeOutMusic(2000);

		if (state.keyboard[SDL_SCANCODE_RETURN] == KEY_DOWN) {
			state.currentScreen = GAMEPLAY; 	
			Mix_PlayMusic(state.musicIngame, -1);
		}
	} break;
	case GAMEPLAY:
	{

		int cool = 10;

		//LIMITS of the MAP - SHIP-P1
		if ((state.ship_x + 64) > SCREEN_WIDTH/2) state.ship_x = (SCREEN_WIDTH/2) - 64;
		if ((state.ship_y + 64) > SCREEN_HEIGHT) state.ship_y = SCREEN_HEIGHT - 64;
		if (state.ship_x < 0) state.ship_x = 0;
		if (state.ship_y < 0) state.ship_y = 0;

		//LIMITS of the MAP - SHIP-P2
		if ((state.ship_x2) < SCREEN_WIDTH/2) state.ship_x2 = (SCREEN_WIDTH/2);
		if ((state.ship_y2 + 64) > SCREEN_HEIGHT) state.ship_y2 = SCREEN_HEIGHT - 64;
		if ((state.ship_x2 + 64 )> SCREEN_WIDTH) state.ship_x2 = SCREEN_WIDTH - 64;
		if (state.ship_y2 < 0) state.ship_y2 = 0;

		
		//-----------------------------------------------ESPECIAL KEYS-------------------------------------------------------
		//SHOW HIT BOX
		if (state.keyboard[SDL_SCANCODE_9] == KEY_DOWN) 
		{
			if (state.ShowHitBox == 255)
			{
				state.ShowHitBox = 0;
			}

			else if (state.ShowHitBox == 0)
			{
				state.ShowHitBox = 255;
			}

		}



		// L2: DONE 7: Move the ship with arrow keys
		//_---------------------------------------------------PLAYER 1-----------------------------------------------------------
		//------Y
		if (state.keyboard[SDL_SCANCODE_W] == KEY_REPEAT && p1.alive==1) state.ship_y -= SHIP_SPEED;
		else if (state.keyboard[SDL_SCANCODE_S] == KEY_REPEAT && p1.alive == 1) state.ship_y += SHIP_SPEED;
		//------X
		if (state.keyboard[SDL_SCANCODE_A] == KEY_REPEAT && p1.alive == 1) state.ship_x -= SHIP_SPEED;
		else if (state.keyboard[SDL_SCANCODE_D] == KEY_REPEAT && p1.alive == 1) state.ship_x += SHIP_SPEED;

		//------SHOT
		if (state.keyboard[SDL_SCANCODE_G] == KEY_DOWN && p1.alive == 1)
		{
			if (MAX_SHIP_SHOTS > 0) {
				cool--;

				if (state.last_shot == MAX_SHIP_SHOTS) state.last_shot = 0;


				state.shots[state.last_shot].alive = true;
				state.shots[state.last_shot].x = state.ship_x + 55;
				state.shots[state.last_shot].y = state.ship_y - 3;
				state.last_shot++;
			}

			else cool++;

			//------Play sound fx_shoot
			Mix_PlayChannel(-1, state.fx_shoot, 0);
		}
		//-------LIFE OF  SHOTS PLAYER 
		for (int i = 0; i < MAX_SHIP_SHOTS; ++i)
		{
			if (state.shots[i].alive)
			{
				if (state.shots[i].x < SCREEN_WIDTH) state.shots[i].x += SHOT_SPEED;
				else state.shots[i].alive = false;
			}
		}

		//------TP
		if (state.keyboard[SDL_SCANCODE_W] == KEY_REPEAT && p1.alive == 1)
		{
			if (state.keyboard[SDL_SCANCODE_F] == KEY_DOWN && p1.alive == 1)
			{
				state.ship_y -= 100;
			}
		}
		if (state.keyboard[SDL_SCANCODE_D] == KEY_REPEAT && p1.alive == 1)
		{
			if (state.keyboard[SDL_SCANCODE_F] == KEY_DOWN && p1.alive == 1)
			{
				state.ship_x += 100;
			}
		}
		if (state.keyboard[SDL_SCANCODE_S] == KEY_REPEAT && p1.alive == 1)
		{
			if (state.keyboard[SDL_SCANCODE_F] == KEY_DOWN && p1.alive == 1)
			{
				state.ship_y += 100;
			}
		}
		if (state.keyboard[SDL_SCANCODE_A] == KEY_REPEAT && p1.alive == 1)
		{
			if (state.keyboard[SDL_SCANCODE_F] == KEY_DOWN && p1.alive == 1)
			{
				state.ship_x -= 100;
			}
		}

		//---------------------------------PLAYER 2-----------------------------------------------------
		//------Y
		if (state.keyboard[SDL_SCANCODE_UP] == KEY_REPEAT && p2.alive2 == 1) state.ship_y2 -= SHIP_SPEED;
		else if (state.keyboard[SDL_SCANCODE_DOWN] == KEY_REPEAT && p2.alive2 == 1) state.ship_y2 += SHIP_SPEED;
		//------X
		if (state.keyboard[SDL_SCANCODE_LEFT] == KEY_REPEAT && p2.alive2 == 1) state.ship_x2 -= SHIP_SPEED;
		else if (state.keyboard[SDL_SCANCODE_RIGHT] == KEY_REPEAT && p2.alive2 == 1) state.ship_x2 += SHIP_SPEED;

		//------SHOT
		if (state.keyboard[SDL_SCANCODE_L] == KEY_DOWN && p2.alive2==1)
		{
			if (MAX_SHIP_SHOTS > 0) {
				cool--;

				if (state.last_shot == MAX_SHIP_SHOTS) state.last_shot = 0;


				state.shots2[state.last_shot].alive = true;
				state.shots2[state.last_shot].x = state.ship_x2 - 20;
				state.shots2[state.last_shot].y = state.ship_y2 - 3;
				state.last_shot++;
			}

			else cool++;


			// -----Play sound fx_shoot
			Mix_PlayChannel(-1, state.fx_shoot, 0);
		}

		//-----LIFE OF SHOTS PLAYER 2
		for (int i = 0; i < MAX_SHIP_SHOTS; ++i)
		{
			if (state.shots2[i].alive)
			{
				if (state.shots2[i].x < SCREEN_WIDTH) state.shots2[i].x -= SHOT_SPEED;
				else state.shots2[i].alive = false;
			}
		}

		//------TP
		if (state.keyboard[SDL_SCANCODE_UP] == KEY_REPEAT && p1.alive == 1)
		{
			if (state.keyboard[SDL_SCANCODE_K] == KEY_DOWN && p1.alive == 1)
			{
				state.ship_y2 -= 100;
			}
		}
		if (state.keyboard[SDL_SCANCODE_RIGHT] == KEY_REPEAT && p1.alive == 1)
		{
			if (state.keyboard[SDL_SCANCODE_K] == KEY_DOWN && p1.alive == 1)
			{
				state.ship_x2 += 100;
			}
		}
		if (state.keyboard[SDL_SCANCODE_DOWN] == KEY_REPEAT && p1.alive == 1)
		{
			if (state.keyboard[SDL_SCANCODE_K] == KEY_DOWN && p1.alive == 1)
			{
				state.ship_y2 += 100;
			}
		}
		if (state.keyboard[SDL_SCANCODE_LEFT] == KEY_REPEAT && p1.alive == 1)
		{
			if (state.keyboard[SDL_SCANCODE_K] == KEY_DOWN && p1.alive == 1)
			{
				state.ship_x2 -= 100;
			}
		}

		//meteorite move
		for (int i = 0; i < 10; ++i)
		{
			state.meteorite_y[i] += 1;
			if (state.meteorite_y[i] < SCREEN_WIDTH) {
				meteo[i].alive = false;

			}
		}

		if (p2.alive2 == 0) state.currentScreen = ENDINGP1;	
		if (p1.alive == 0) state.currentScreen = ENDINGP2;
	} break;
	case ENDINGP1:
	{
		if (p2.alive2 == 0) {
			Mix_PlayMusic(state.musicvictory, -1);
		}
	} break;
	case ENDINGP2:
	{
		if (p1.alive == 0) {
			Mix_PlayMusic(state.musicvictory, -1);
		}
	} break;
	default: break;
	}
}

// ----------------------------------------------------------------
void Draw()
{
	// Clear screen to Cornflower blue
	SDL_SetRenderDrawColor(state.renderer, 100, 149, 237, 255);
	SDL_RenderClear(state.renderer);

	switch (state.currentScreen)
	{
	case MENU:
	{
		SDL_Rect rec = { -state.scroll, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_RenderCopy(state.renderer, state.menu, NULL, &rec);
	} break;
	case CONTROLS:
	{

	} break;
	case GAMEPLAY:
	{
		// Draw background texture (two times for scrolling effect)
		// NOTE: rec rectangle is being reused for next draws
		SDL_Rect rec = { -state.scroll, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_RenderCopy(state.renderer, state.background, NULL, &rec);
	
		// Draw meteorite texture
		for (int p = 0; p < 10; p++) {

			DrawRectangle(state.meteorite_x[p]+rand()%800, state.meteorite_y[p], state.ship_w, state.ship_h, { 255, 0, 0,  (Uint8)state.ShowHitBox });
			rec.x = state.meteorite_x[p]; rec.y = state.meteorite_y[p]; rec.w = 96; rec.h = 96;
			SDL_RenderCopy(state.renderer, state.meteorite, NULL, &rec);

		}

		// Draw lifes textures
		// P1
		for (int i = 0; i < MAX_LIFE; ++i) {
			rec.x = state.life1_x[i]; rec.y = state.life1_y[i]; rec.w = 20; rec.h = 20;
			SDL_RenderCopy(state.renderer, state.life1[i], NULL, &rec);
		}


		//P2
		for (int i = 0; i < MAX_LIFE; ++i) {
			rec.x = state.life2_x[i]; rec.y = state.life2_y[i]; rec.w = 20; rec.h = 20;
			SDL_RenderCopy(state.renderer, state.life2[i], NULL, &rec);
		}



		// -----------------------PLAYER 1
		// Draw ship rectangle hitbox
	
		if (p1.alive == 1)
		{
			DrawRectangle(state.ship_x, state.ship_y, state.ship_w, state.ship_h, { 255, 0, 0,  (Uint8)state.ShowHitBox });
		}
		else if (p1.alive == 0) 
		{
			SDL_DestroyTexture(state.ship);
		}
		rec.x = state.ship_x; rec.y = state.ship_y; rec.w = 64; rec.h = 64;
		SDL_RenderCopy(state.renderer, state.ship, NULL, &rec);

		rec.w = 64; rec.h = 64;
		for (int i = 0; i < MAX_SHIP_SHOTS; ++i)
		{
			if (state.shots[i].alive)
			{
				DrawRectangle(state.shots[i].x, state.shots[i].y + 15, state.shot_w, state.shot_h, { 0, 250, 0,  (Uint8)state.ShowHitBox });
				rec.x = state.shots[i].x; rec.y = state.shots[i].y;
				SDL_RenderCopy(state.renderer, state.shot, NULL, &rec);
			}
		}

		// -----------------------PLAYER 2
		// Draw ship rectangle hitbox


		if (p2.alive2 == 1)
		{
			DrawRectangle(state.ship_x2, state.ship_y2, state.ship_w2, state.ship_h2, { 255, 0, 0,  (Uint8)state.ShowHitBox });
		}
		else if (p2.alive2 == 0)
		{
			SDL_DestroyTexture(state.ship2);
		}

		rec.x = state.ship_x2; rec.y = state.ship_y2; rec.w = 64; rec.h = 64;
		SDL_RenderCopy(state.renderer, state.ship2, NULL, &rec);

		rec.w = 64; rec.h = 64;

		for (int i = 0; i < MAX_SHIP_SHOTS; ++i)
		{
			if (state.shots2[i].alive)
			{
				DrawRectangle(state.shots2[i].x, state.shots2[i].y + 15, state.shot_w2, state.shot_h2, { 0, 250, 0, (Uint8)state.ShowHitBox });
				rec.x = state.shots2[i].x; rec.y = state.shots2[i].y;
				SDL_RenderCopy(state.renderer, state.shot2, NULL, &rec);
			}
		}

	} break;

	case ENDINGP1:
	{
		SDL_Rect rec = { -state.scroll, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_RenderCopy(state.renderer, state.p1win, NULL, &rec);
	} break;
	case ENDINGP2:
	{
		SDL_Rect rec = { -state.scroll, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_RenderCopy(state.renderer, state.p2win, NULL, &rec);
	} break;
	default: break;
	}

	// Finally present framebuffer
	SDL_RenderPresent(state.renderer);
}


// Main Entry point
// -------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	Start();

	while (CheckInput())
	{
		hitbox();

		MoveStuff();

		Draw();
	}

	Finish();

	return(EXIT_SUCCESS);
}

// Functions Definition
// -------------------------------------------------------------------------
void DrawRectangle(int x, int y, int width, int height, SDL_Color color)
{
	SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(state.renderer, color.r, color.g, color.b, color.a);

	SDL_Rect rec = { x, y, width, height };

	int result = SDL_RenderFillRect(state.renderer, &rec);

	if (result != 0) printf("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
}

// ----------------------------------------------------------------
void DrawLine(int x1, int y1, int x2, int y2, SDL_Color color)
{
	SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(state.renderer, color.r, color.g, color.b, color.a);

	int result = SDL_RenderDrawLine(state.renderer, x1, y1, x2, y2);

	if (result != 0) printf("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
}

// ----------------------------------------------------------------
void DrawCircle(int x, int y, int radius, SDL_Color color)
{
	SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(state.renderer, color.r, color.g, color.b, color.a);

	SDL_Point points[360];
	float factor = (float)M_PI / 180.0f;

	for (int i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cosf(factor * i));
		points[i].y = (int)(y + radius * sinf(factor * i));
	}

	int result = SDL_RenderDrawPoints(state.renderer, points, 360);

	if (result != 0) printf("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
}