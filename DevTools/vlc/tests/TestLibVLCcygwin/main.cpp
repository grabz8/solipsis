/* CYGWIN - copy libvlc.dll into this directory
export CXX='g++ -mno-cygwin'
${CXX} -g -c ./main.cpp -o ./main.o -I /cygdrive/d/solipsis/Work/solipsis/trunk/DevTools/vlc/libraries/i686-win32/include
${CXX} -g -o ./main.exe ./main.o -llibvlc -L./
./main.exe
*/

#include <stdio.h>
#include <windows.h>
#include <vlc/libvlc.h>

#ifdef DYNAMIC_LOAD
HMODULE h = NULL;
#endif

static void quit_on_exception (libvlc_exception_t *excp) {
#ifdef DYNAMIC_LOAD
   int (*libvlc_exception_raisedFunction)(libvlc_exception_t *) = reinterpret_cast<int (*)(libvlc_exception_t *)>(GetProcAddress(h, "libvlc_exception_raised"));
   if (libvlc_exception_raisedFunction (excp)) {
      char* (*libvlc_exception_get_messageFunction)(libvlc_exception_t *) = reinterpret_cast<char* (*)(libvlc_exception_t *)>(GetProcAddress(h, "libvlc_exception_get_message"));
      fprintf(stderr, "error: %s\n", libvlc_exception_get_messageFunction(excp));
      exit(-1);
   }
#else
   if (libvlc_exception_raised (excp)) {
      fprintf(stderr, "error: %s\n", libvlc_exception_get_message(excp));
      exit(-1);
   }
#endif
}

//#define TEST_VMEM
#ifdef TEST_VMEM
void * _libvlc_lock(void *ctx)
{
    return ctx; // ctx is finally the allocated frame buffer
}
void _libvlc_unlock(void *ctx)
{
}
#endif

int main(int argc, char **argv) {
    libvlc_exception_t excp;
    libvlc_instance_t *inst, *inst2;
    int item, item2;

#ifdef TEST_VMEM
    char *framebuf1, *framebuf2;
    char plock[64], punlock[64], pdata1[64], pdata2[64];
#endif

    char const *myargs[] =
    {
        "--no-one-instance",
        "--no-stats",
        "--intf", "dummy",
//        "--loop",
#ifdef WIN32
        "--fast-mutex", "--win9x-cv-method=1",
#endif
//        "--plugin-path=D:\\solipsis\\Work\\solipsis\\trunk\\DevTools\\vlc\\libraries\\i686-win32\\runtime\\plugins",
        "--plugin-path=D:\\solipsis\\Work\\solipsis\\trunk\\DevTools\\vlc\\build_vlc\\vlc-0.8.6e2\\vlc-0.8.6e\\plugins",
#ifdef TEST_VMEM
        "--vout", "vmem",
        "--vmem-width", "320",
        "--vmem-height", "200",
        "--vmem-pitch", "0",
        "--vmem-chroma", "RV16",
        "--vmem-lock", plock,
        "--vmem-unlock", punlock,
        "--vmem-data", pdata1,
#endif
    };
    int myargsc = sizeof(myargs) / sizeof(*myargs);
    char *filename = "D:\\burned\\solipsis\\8103D9F9d01.mpeg";

#ifdef TEST_VMEM
    framebuf1 = (char*)malloc(320*200*sizeof(unsigned short));
    sprintf(plock, "%lld", _libvlc_lock);
    sprintf(punlock, "%lld", _libvlc_unlock);
    sprintf(pdata1, "%lld", framebuf1);
#endif

    char const *myargs2[] =
    {
        "--no-one-instance",
        "--no-stats",
        "--intf", "dummy",
//        "--loop",
#ifdef WIN32
        "--fast-mutex", "--win9x-cv-method=1",
#endif
//        "--plugin-path=D:\\solipsis\\Work\\solipsis\\trunk\\DevTools\\vlc\\libraries\\i686-win32\\runtime\\plugins",
        "--plugin-path=D:\\solipsis\\Work\\solipsis\\trunk\\DevTools\\vlc\\build_vlc\\vlc-0.8.6e2\\vlc-0.8.6e\\plugins",
#ifdef TEST_VMEM
        "--vout", "vmem",
        "--vmem-width", "320",
        "--vmem-height", "200",
        "--vmem-pitch", "0",
        "--vmem-chroma", "RV16",
        "--vmem-lock", plock,
        "--vmem-unlock", punlock,
        "--vmem-data", pdata2,
#endif
    };
    int myargsc2 = sizeof(myargs2) / sizeof(*myargs2);
    char *filename2 = "D:\\burned\\solipsis\\E5535DC8d01.mpeg";

#ifdef TEST_VMEM
    framebuf2 = (char*)malloc(320*200*sizeof(unsigned short));
    sprintf(plock, "%lld", _libvlc_lock);
    sprintf(punlock, "%lld", _libvlc_unlock);
    sprintf(pdata2, "%lld", framebuf2);
#endif

#ifdef DYNAMIC_LOAD
//    h = LoadLibrary("C:\\program files\\VideoLAN\\VLC\\libvlc.dll");
    h = LoadLibrary("libvlc.dll");
    void (*libvlc_exception_initFunction)( libvlc_exception_t *) = reinterpret_cast<void (*)(libvlc_exception_t *)>(GetProcAddress(h, "libvlc_exception_init"));
    libvlc_instance_t* (*libvlc_newFunction)(int , char **, libvlc_exception_t *) = reinterpret_cast<libvlc_instance_t* (*)(int , char **, libvlc_exception_t *)>(GetProcAddress(h, "libvlc_new"));
    int (*libvlc_playlist_addFunction)(libvlc_instance_t *, const char *, const char *, libvlc_exception_t *) = reinterpret_cast<int (*)(libvlc_instance_t *, const char *, const char *, libvlc_exception_t *)>(GetProcAddress(h, "libvlc_playlist_add"));
    void (*libvlc_playlist_playFunction)(libvlc_instance_t*, int, int, char **, libvlc_exception_t *) = reinterpret_cast<void (*)(libvlc_instance_t*, int, int, char **, libvlc_exception_t *)>(GetProcAddress(h, "libvlc_playlist_play"));
    void (*libvlc_playlist_stopFunction)(libvlc_instance_t*, libvlc_exception_t *) = reinterpret_cast<void (*)(libvlc_instance_t*, libvlc_exception_t *)>(GetProcAddress(h, "libvlc_playlist_stop"));
    int (*libvlc_playlist_isplayingFunction)(libvlc_instance_t *, libvlc_exception_t *) = reinterpret_cast<int (*)(libvlc_instance_t *, libvlc_exception_t *)>(GetProcAddress(h, "libvlc_playlist_isplaying"));
    void (*libvlc_destroyFunction)(libvlc_instance_t *) = reinterpret_cast<void (*)(libvlc_instance_t *)>(GetProcAddress(h, "libvlc_destroy"));
    (*libvlc_exception_initFunction)(&excp);
    printf("Starting 1st video\n");
    inst = libvlc_newFunction (myargsc, (char**)myargs, &excp);
    quit_on_exception (&excp);
    item = libvlc_playlist_addFunction (inst, filename, NULL, &excp); 
    quit_on_exception (&excp);
    libvlc_playlist_playFunction (inst, item, 0, NULL, &excp); 
    quit_on_exception (&excp);
//    while (libvlc_playlist_isplayingFunction(inst, &excp))
        Sleep (3000);

    printf("Starting 2nd video\n");
    inst2 = libvlc_newFunction (myargsc2, (char**)myargs2, &excp);
    quit_on_exception (&excp);
    item2 = libvlc_playlist_addFunction (inst2, filename2, NULL, &excp); 
    quit_on_exception (&excp);
    libvlc_playlist_playFunction (inst2, item2, 0, NULL, &excp); 
    quit_on_exception (&excp);
    Sleep (10000);

    printf("End of 2nd video\n");
    libvlc_playlist_stopFunction(inst2, &excp);
    libvlc_destroyFunction (inst2);
    Sleep (3000);

    printf("End of 1st video\n");
    libvlc_playlist_stopFunction(inst, &excp);
    libvlc_destroyFunction (inst);
#else
    libvlc_exception_init (&excp);
    printf("Starting 1st video\n");
    inst = libvlc_new (myargsc, (char**)myargs, &excp);
    quit_on_exception (&excp);
    item = libvlc_playlist_add (inst, filename, NULL, &excp); 
    quit_on_exception (&excp);
    libvlc_playlist_play (inst, item, 0, NULL, &excp); 
    quit_on_exception (&excp);
//    while (libvlc_playlist_isplaying(inst, &excp))
        Sleep (3000);

    printf("Starting 2nd video\n");
    inst2 = libvlc_new (myargsc2, (char**)myargs2, &excp);
    quit_on_exception (&excp);
    item2 = libvlc_playlist_add (inst2, filename2, NULL, &excp); 
    quit_on_exception (&excp);
    libvlc_playlist_play (inst2, item2, 0, NULL, &excp); 
    quit_on_exception (&excp);
    Sleep (10000);

    printf("End of 2nd video\n");
    libvlc_playlist_stop(inst2, &excp);
    libvlc_destroy (inst2);
    Sleep (3000);

    printf("End of 1st video\n");
    libvlc_playlist_stop(inst, &excp);
    libvlc_destroy (inst);
#endif

#ifdef TEST_VMEM
    free(framebuf2);
    free(framebuf1);
#endif

    printf("End.\n");

    return 0;
}
