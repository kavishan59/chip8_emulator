#include "graphic.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <string.h>

int initialize_SDL(void)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "Error SDL_init: %s.\n", SDL_GetError());
    return -1;
  }
  return 0;
}

void destroy_SDL(void)
{
  SDL_Quit();
}

void clear_screen(screen *screen)
{
  memset(screen->pixels, BLACK, sizeof(screen->pixels));
}


int initialize_screen(screen *screen)
{
  screen->w = SDL_CreateWindow("Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  if (screen->w == NULL)
  {
    fprintf(stderr, "Error SDL_creareWindow: %s.\n", SDL_GetError());
    return -1;
  }

  screen->r = SDL_CreateRenderer(screen->w, -1, SDL_RENDERER_ACCELERATED);

  if (NULL == screen->r)
  {
    fprintf(stderr, "Error SDL_CreateRenderer: %s.\n", SDL_GetError());
    SDL_DestroyWindow(screen->w);
    return -2;
  }
  
  clear_screen(screen);
  screen->pixel_height = PIXEL_DIM;
  screen->pixel_width = PIXEL_DIM;
  return 0;
}

void destroy_screen(screen *screen)
{
  SDL_DestroyRenderer(screen->r);
  SDL_DestroyWindow(screen->w);
}


// fill the screen black , then set on every white pixel
void update_screen(screen *screen)
{
  SDL_SetRenderDrawColor(screen->r, 0, 0, 0, 255);
  SDL_RenderClear(screen->r);
  SDL_SetRenderDrawColor(screen->r, 255, 255, 255, 255);

  for (size_t i = 0; i < PIXEL_BY_WIDTH; i++)
  {
    for(size_t j = 0; j < PIXEL_BY_HEIGHT; j++)
    {
      if(screen->pixels[i][j] == WHITE)
      {
        SDL_Rect pixel_rect = {screen->pixel_width * i, screen->pixel_height * j, screen->pixel_width,screen->pixel_height};
        SDL_RenderFillRect(screen->r, &pixel_rect);
      }
    }
  }
  SDL_RenderPresent(screen->r);
}

void resize_screen(screen *screen)
{
  int w,h;
  SDL_GetWindowSize(screen->w, &w, &h);
  screen->pixel_height = h / PIXEL_BY_HEIGHT;
  screen->pixel_width = w / PIXEL_BY_WIDTH;
}


//manage every event using pollevent sdl function and store them in our struct
void update_event(input *input)
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    if (event.type == SDL_QUIT)
      input->quit = SDL_TRUE;
    else if(event.type == SDL_KEYDOWN)
        input->key[event.key.keysym.sym] = SDL_TRUE;
    else if(event.type == SDL_KEYUP)
        input->key[event.key.keysym.sym] = SDL_FALSE;
    else if(event.type == SDL_MOUSEMOTION)
    {
      input->x = event.motion.x;
      input->y = event.motion.y;
      input->xrel = event.motion.xrel;
      input->yrel = event.motion.yrel;
    }
    else if(event.type == SDL_MOUSEWHEEL)
    {
      input->xwheel = event.wheel.x;
      input->ywheel = event.wheel.y;
    }
    else if(event.type == SDL_MOUSEBUTTONDOWN)
      input->mouse[event.button.button] = SDL_TRUE;
    else if(event.type == SDL_MOUSEBUTTONUP)
      input->mouse[event.button.button] = SDL_FALSE;
    else if(event.type == SDL_WINDOWEVENT)
        if(event.window.event == SDL_WINDOWEVENT_RESIZED)
          input->resize = SDL_TRUE;
  }
}
