
#include "SDL/SDL.h"

#include "qEngine.h"
#include "String.h"
#include "Common.h"
#include "Math.h"
#include "InputEvent.h"
#include "Log.h"
#include "Timer.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480

qEngine * engine;
Common commonInstance;
Common * common = &commonInstance;
Timer timer_obj;
Timer * timer = &timer_obj;


static void ReadInput(void)
{
	SDL_Event sdlEvent;

	while( SDL_PollEvent(&sdlEvent) ) {
		switch (sdlEvent.type) {
			case SDL_QUIT:
				engine->Stop();
				break;

			case SDL_KEYUP:
				break;

			case SDL_MOUSEMOTION:
				break;

			case SDL_MOUSEBUTTONUP:
				// fall through
			default:
				break;
		}

	}
}


int main(void)
{
	SDL_Surface * screen;

	if( SDL_Init(SDL_INIT_EVERYTHING) != 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        return 1;
    }

	SDL_WM_SetCaption("qEngine", NULL);

	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(SDL_ENABLE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	const SDL_VideoInfo * info = SDL_GetVideoInfo();
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, info->vfmt->BitsPerPixel, SDL_OPENGL);
    
	qEngine engineInstance(SCREEN_WIDTH, SCREEN_HEIGHT);
	engine = &engineInstance;
	engine->LoadMap("act1.map");
    engine->SetCurrentCameraPath(0);

    int prev_time, now_time, time_for_frame, sleep_time;
    prev_time = SDL_GetTicks();

	while( engine->IsOn() ) {
		ReadInput();
		engine->UpdateWorld();
		engine->RenderFrame();

		SDL_GL_SwapBuffers();

        now_time = SDL_GetTicks();
        time_for_frame = now_time - prev_time;
        prev_time = now_time;
        sleep_time = 16.7 - time_for_frame;
        if( sleep_time > 0 )
            SDL_Delay(sleep_time);
	}

	SDL_Quit();

	return 0;
}
