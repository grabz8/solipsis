/* CYGWIN - copy libvlc.dll and SDL.dll into this directory
export CXX='g++ -mno-cygwin'
${CXX} -g -c ./main.cpp -o ./main.o -I /cygdrive/d/solipsis/Work/solipsis/trunk/DevTools/vlc-0.8.6e/include -I /cygdrive/d/tmp/SDL-1.2.13/include
${CXX} -g -o ./main.exe ./main.o -llibvlc -L. -lSDL -L.
./main.exe
*/

/* libSDL and libVLC sample code
 * Copyright © 2008 Sam Hocevar <sam@zoy.org>
 * license: [http://en.wikipedia.org/wiki/WTFPL WTFPL] */

/* GREG Tested with SDL 1.2.13 */

#include <stdio.h>
#include <conio.h>
//#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_mutex.h>

#include <vlc/libvlc.h>

#define WIDTH 640
#define HEIGHT 480

#define VIDEOWIDTH 320
#define VIDEOHEIGHT 240

struct ctx
{
    SDL_Surface *surf;
    SDL_mutex *mutex;
};

//static void catch (libvlc_exception_t *ex)
static void quit_on_exception (libvlc_exception_t *ex)
{
    if(libvlc_exception_raised(ex))
    {
        fprintf(stderr, "exception: %s\n", libvlc_exception_get_message(ex));
        exit(1);
    }

    libvlc_exception_clear(ex);
}

static void * lock(struct ctx *ctx)
{
    SDL_LockMutex(ctx->mutex);
    SDL_LockSurface(ctx->surf);
    return ctx->surf->pixels;
}

static void unlock(struct ctx *ctx)
{
    /* VLC just rendered the video, but we can also render stuff */
    uint16_t *pixels = (uint16_t *)ctx->surf->pixels;
    int x, y;

    for(y = 10; y < 40; y++)
        for(x = 10; x < 40; x++)
            if(x < 13 || y < 13 || x > 36 || y > 36)
                pixels[y * VIDEOWIDTH + x] = 0xffff;
            else
                pixels[y * VIDEOWIDTH + x] = 0x0;

    SDL_UnlockSurface(ctx->surf);
    SDL_UnlockMutex(ctx->mutex);
}

int main(int argc, char *argv[])
{
    char clock[64], cunlock[64], cdata[64];
    char width[32], height[32], pitch[32];
    libvlc_exception_t ex;
    libvlc_instance_t *libvlc;
//    libvlc_media_t *m;
    int item;
//    libvlc_media_player_t *mp;
#if 0
    char const *vlc_argv[] =
    {
//        "-q",
        "--ignore-config", /* Don't use VLC's config files */
        "-I", "dummy",
        //"-vvvvv",
//        "--plugin-path", VLC_TREE "/modules",
        "--plugin-path=D:\\tmp\\TestLibVLC\\TestLibVLC\\Debug\\plugins",
//        "--ignore-config", /* Don't use VLC's config files */
        "--noaudio",
        "--vout", "vmem",
        "--vmem-width", width,
        "--vmem-height", height,
        "--vmem-pitch", pitch,
        "--vmem-chroma", "RV16",
        "--vmem-lock", clock,
        "--vmem-unlock", cunlock,
        "--vmem-data", cdata,
    };
#endif
    char const *vlc_argv[] =
    {
        "--no-one-instance",
        "--no-stats",
        "--intf", "dummy",
//        "--loop",
        "--fast-mutex", "--win9x-cv-method=1",
        "--plugin-path=D:\\tmp\\TestLibVLC\\TestLibVLC\\Debug\\plugins",
        "--vout", "vmem",
        "--vmem-width", width,
        "--vmem-height", height,
        "--vmem-pitch", pitch,
        "--vmem-chroma", "RV16",
        "--vmem-lock", clock,
        "--vmem-unlock", cunlock,
        "--vmem-data", cdata,
    };
    int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

    SDL_Surface *screen, *empty;
    SDL_Event event;
    SDL_Rect rect;
    int done = 0, action = 0, pause = 0, n = 0;

    struct ctx ctx;

    /*
     *  Initialise libSDL
     */
//    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD) == -1)
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        printf("cannot initialize SDL error:%s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    empty = SDL_CreateRGBSurface(SDL_SWSURFACE, VIDEOWIDTH, VIDEOHEIGHT,
                                 32, 0, 0, 0, 0);
    ctx.surf = SDL_CreateRGBSurface(SDL_SWSURFACE, VIDEOWIDTH, VIDEOHEIGHT,
                                    16, 0x001f, 0x07e0, 0xf800, 0);

    ctx.mutex = SDL_CreateMutex();

    int options = SDL_ANYFORMAT | SDL_HWSURFACE | SDL_DOUBLEBUF;

    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, options);
    if(!screen)
    {
        printf("cannot set video mode\n");
        return EXIT_FAILURE;
    }

    /*
     *  Initialise libVLC
     */
    sprintf(clock, "%lld", (long long int)(intptr_t)lock);
    sprintf(cunlock, "%lld", (long long int)(intptr_t)unlock);
    sprintf(cdata, "%lld", (long long int)(intptr_t)&ctx);
    sprintf(width, "%i", VIDEOWIDTH);
    sprintf(height, "%i", VIDEOHEIGHT);
    sprintf(pitch, "%i", VIDEOWIDTH * 2);

    if(argc < 2)
    {
        printf("too few arguments (MRL needed)\n");
        return EXIT_FAILURE;
    }
    libvlc_exception_init(&ex);
//    libvlc = libvlc_new(vlc_argc, vlc_argv, &ex);
    libvlc = libvlc_new(vlc_argc, (char**)vlc_argv, &ex);
//    catch(&ex);
    quit_on_exception(&ex);
//    m = libvlc_media_new(libvlc, argv[1], &ex);
    item = libvlc_playlist_add (libvlc, argv[1], NULL, &ex); 
//    catch(&ex);
    quit_on_exception(&ex);
//    mp = libvlc_media_player_new_from_media(m, &ex);
//    catch(&ex);
//    libvlc_media_release(m);

//    libvlc_media_player_play(mp, &ex);
    libvlc_playlist_play (libvlc, item, 0, NULL, &ex); 
//    catch(&ex);
    quit_on_exception(&ex);

    /*
     *  Main loop
     */
    rect.w = 0;
    rect.h = 0;

    while(!done)
    { 
        action = 0;

#if 0
        /* Keys: enter (fullscreen), space (pause), escape (quit) */
        while( SDL_PollEvent( &event ) ) 
        { 
            switch(event.type)
            {
            case SDL_QUIT:
                done = 1;
                break;
            case SDL_KEYDOWN:
                action = event.key.keysym.sym;
                break;
            }
        }

        switch(action)
        {
        case SDLK_ESCAPE:
            done = 1;
            break;
        case SDLK_RETURN:
            options ^= SDL_FULLSCREEN;
            screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, options);
            break;
        case ' ':
            pause = !pause;
            break;
        }
#endif

        rect.x = (int)((1. + .5 * sin(0.03 * n)) * (WIDTH - VIDEOWIDTH) / 2);
        rect.y = (int)((1. + .5 * cos(0.03 * n)) * (HEIGHT - VIDEOHEIGHT) / 2);

        if(!pause)
            n++;

        /* Blitting the surface does not prevent it from being locked and
         * written to by another thread, so we use this additional mutex. */
        SDL_LockMutex(ctx.mutex);
        SDL_BlitSurface(ctx.surf, NULL, screen, &rect);
        SDL_UnlockMutex(ctx.mutex);

        SDL_Flip(screen);
        SDL_Delay(10);

        SDL_BlitSurface(empty, NULL, screen, &rect);

        if (!libvlc_playlist_isplaying(libvlc, &ex)) break;
/*        int key = 0;
        // check keyboard
        if (_kbhit())
            key = _getch();
        if (key == 27) break;*/
    }

    /*
     * Stop stream and clean up libVLC
     */
//    libvlc_media_player_stop(mp, &ex);
//    libvlc_playlist_stop(libvlc, &ex);
//    catch(&ex);
//    quit_on_exception(&ex);
//    libvlc_playlist_clear(libvlc, &ex);
//    quit_on_exception(&ex);

//    libvlc_media_player_release(mp);
//    libvlc_release(libvlc);
    libvlc_destroy(libvlc);

    /*
     * Close window and clean up libSDL
     */
    SDL_DestroyMutex(ctx.mutex);
    SDL_FreeSurface(ctx.surf);
    SDL_FreeSurface(empty);

    SDL_Quit();

    return 0;
}

#undef main
int main(int argc, char *argv[])
{
    return SDL_main(argc, argv);
}
