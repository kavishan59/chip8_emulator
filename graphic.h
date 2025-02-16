#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <stddef.h>
#include <string.h>


#define BLACK SDL_FALSE
#define WHITE SDL_TRUE
#define PIXEL_BY_WIDTH 64
#define PIXEL_BY_HEIGHT 32
#define PIXEL_DIM 8
#define WIDTH PIXEL_BY_WIDTH * PIXEL_DIM
#define HEIGHT PIXEL_BY_HEIGHT * PIXEL_DIM

#define SDL_BUTTON_LEFT     1
#define SDL_BUTTON_MIDDLE   2
#define SDL_BUTTON_RIGHT    3
#define SDL_BUTTON_X1       4
#define SDL_BUTTON_X2       5

extern Mix_Chunk *beep_sound;
typedef SDL_bool s_pixel;


//represent our screen
typedef struct s_screen
{
  SDL_Window *w;
  SDL_Renderer *r;
  s_pixel pixels[PIXEL_BY_WIDTH][PIXEL_BY_HEIGHT]; //represent each pixel
  Uint32 pixel_height; //the height of our pixel in our screen
  Uint32 pixel_width; //the width of our pixel in our screen
} screen;

//manage events needed by our emulator
typedef struct s_input
{
  SDL_bool  key[SDL_NUM_SCANCODES];
  SDL_bool  quit;
  int x, y, xrel, yrel;  
  int xwheel, ywheel;
  SDL_bool  mouse[6];
  SDL_bool resize;
} input;


int initialize_SDL(void);
void destroy_SDL(void);
void clear_screen(screen *screen);
int initialize_screen(screen *screen);
void destroy_screen(screen *screen);
void update_screen(screen *screen);
void resize_screen(screen *screen);
void update_event(input *input);
int initialize_sound(void);
#endif



