/*****************************************************************************
 * vmem.c: memory video driver for vlc
 *****************************************************************************
 * Copyright (C) 2008 the VideoLAN team
 * $Id$
 *
 * Authors: Sam Hocevar <sam@zoy.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

/*****************************************************************************
 * Preamble
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc/vlc.h>
// GREG BEGIN
//#include <vlc_vout.h>
#include <vlc_video.h>
#include <video_output.h>
// GREG END

#define argsPVLC

/*****************************************************************************
 * Local prototypes
 *****************************************************************************/
static int  Create    ( vlc_object_t * );
static void Destroy   ( vlc_object_t * );

static int  Init          ( vout_thread_t * );
// GREG BEGIN
static void End           ( vout_thread_t * );
// GREG END
static int  LockPicture   ( vout_thread_t *, picture_t * );
static int  UnlockPicture ( vout_thread_t *, picture_t * );

/*****************************************************************************
 * Module descriptor
 *****************************************************************************/
#define T_WIDTH N_( "Width" )
#define LT_WIDTH N_( "Video memory buffer width." )

#define T_HEIGHT N_( "Height" )
#define LT_HEIGHT N_( "Video memory buffer height." )

#define T_PITCH N_( "Pitch" )
#define LT_PITCH N_( "Video memory buffer pitch in bytes." )

#define T_CHROMA N_( "Chroma" )
#define LT_CHROMA N_( "Output chroma for the memory image as a 4-character " \
                      "string, eg. \"RV32\"." )

#define T_LOCK N_( "Lock function" )
#define LT_LOCK N_( "Address of the locking callback function. This " \
                    "function must return a valid memory address for use " \
                    "by the video renderer." )

#define T_UNLOCK N_( "Unlock function" )
#define LT_UNLOCK N_( "Address of the unlocking callback function" )

#define T_DATA N_( "Callback data" )
#define LT_DATA N_( "Data for the locking and unlocking functions" )

#ifndef argsPVLC
#define T_IPH N_( "Instance params handler function" )
#define LT_IPH N_( "Address of the instance parameters handler function" )
#endif

vlc_module_begin( );
    set_description( _( "Video memory module" ) );
    set_shortname( _("Video memory") );

    set_category( CAT_VIDEO );
    set_subcategory( SUBCAT_VIDEO_VOUT );
    set_capability( "video output", 0 );

    add_integer( "vmem-width", 320, NULL, T_WIDTH, LT_WIDTH, VLC_FALSE );
    add_integer( "vmem-height", 200, NULL, T_HEIGHT, LT_HEIGHT, VLC_FALSE );
    add_integer( "vmem-pitch", 640, NULL, T_PITCH, LT_PITCH, VLC_FALSE );
    add_string( "vmem-chroma", "RV16", NULL, T_CHROMA, LT_CHROMA, VLC_TRUE );
    add_string( "vmem-lock", "0", NULL, T_LOCK, LT_LOCK, VLC_TRUE );
    add_string( "vmem-unlock", "0", NULL, T_UNLOCK, LT_UNLOCK, VLC_TRUE );
    add_string( "vmem-data", "0", NULL, T_DATA, LT_DATA, VLC_TRUE );
#ifndef argsPVLC
    add_string( "vmem-iph", "0", NULL, T_IPH, LT_IPH, VLC_TRUE );
#endif

    set_callbacks( Create, Destroy );
vlc_module_end();

/*****************************************************************************
 * vout_sys_t: video output descriptor
 *****************************************************************************/
struct vout_sys_t
{
    int i_width, i_height, i_pitch;

    void * (*pf_lock) (void *);
    void (*pf_unlock) (void *);
    void *p_data;
#ifndef argsPVLC
    void (*pf_iph) (void *, char *, char *);
#endif
};

/*****************************************************************************
 * Create: allocates video thread
 *****************************************************************************
 * This function allocates and initializes a vout method.
 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    vout_thread_t *p_vout = ( vout_thread_t * )p_this;

    /* Allocate instance and initialize some members */
    p_vout->p_sys = malloc( sizeof( vout_sys_t ) );
    if( ! p_vout->p_sys )
        return VLC_ENOMEM;

    p_vout->pf_init = Init;
// GREG BEGIN
//    p_vout->pf_end = NULL;
    p_vout->pf_end = End;
// GREG END
    p_vout->pf_manage = NULL;
    p_vout->pf_render = NULL;
    p_vout->pf_display = NULL;

    return VLC_SUCCESS;
}

#ifdef argsPVLC
static char* getPsz(int argc, char **argv, char *name)
{
    int a;
    for (a = 0; a < argc; a++)
        if (strstr(argv[a], name) != 0)
            return strdup(argv[a + 1]);
    return 0;
}
#endif

/*****************************************************************************
 * Init: initialize video thread
 *****************************************************************************/
static int Init( vout_thread_t *p_vout )
{
    int i_index;
    picture_t *p_pic;
    char *psz_chroma, *psz_tmp;
    int i_width, i_height, i_pitch, i_chroma;
#ifndef argsPVLC
    char value[32], chroma[32];
#endif

#ifndef argsPVLC
    i_width = config_GetInt( p_vout, "vmem-width" );
    i_height = config_GetInt( p_vout, "vmem-height" );
    i_pitch = config_GetInt( p_vout, "vmem-pitch" );
#else
    i_width = atoi( getPsz( p_vout->p_vlc->i_argc, p_vout->p_vlc->ppsz_argv, "vmem-width" ) );
    i_height = atoi( getPsz( p_vout->p_vlc->i_argc, p_vout->p_vlc->ppsz_argv, "vmem-height" ) );
    i_pitch = atoi( getPsz( p_vout->p_vlc->i_argc, p_vout->p_vlc->ppsz_argv, "vmem-pitch" ) );
#endif

#ifndef argsPVLC
    psz_chroma = config_GetPsz( p_vout, "vmem-chroma" );
    strcpy( chroma, psz_chroma);
#else
    psz_chroma = getPsz( p_vout->p_vlc->i_argc, p_vout->p_vlc->ppsz_argv, "vmem-chroma" );
#endif
    if( psz_chroma )
    {
        if( strlen( psz_chroma ) < 4 )
        {
            msg_Err( p_vout, "vmem-chroma should be 4 characters long" );
            free( psz_chroma );
            return VLC_EGENERIC;
        }
        i_chroma = VLC_FOURCC( psz_chroma[0], psz_chroma[1],
                               psz_chroma[2], psz_chroma[3] );
        free( psz_chroma );
    }
    else
    {
        msg_Err( p_vout, "Cannot find chroma information." );
        return VLC_EGENERIC;
    }

#ifndef argsPVLC
    psz_tmp = config_GetPsz( p_vout, "vmem-lock" );
#else
    psz_tmp = getPsz( p_vout->p_vlc->i_argc, p_vout->p_vlc->ppsz_argv, "vmem-lock" );
#endif
    p_vout->p_sys->pf_lock = (void * (*) (void *))(intptr_t)atoll( psz_tmp );
    free( psz_tmp );

#ifndef argsPVLC
    psz_tmp = config_GetPsz( p_vout, "vmem-unlock" );
#else
    psz_tmp = getPsz( p_vout->p_vlc->i_argc, p_vout->p_vlc->ppsz_argv, "vmem-unlock" );
#endif
    p_vout->p_sys->pf_unlock = (void (*) (void *))(intptr_t)atoll( psz_tmp );
    free( psz_tmp );

#ifndef argsPVLC
    psz_tmp = config_GetPsz( p_vout, "vmem-data" );
#else
    psz_tmp = getPsz( p_vout->p_vlc->i_argc, p_vout->p_vlc->ppsz_argv, "vmem-data" );
#endif
    p_vout->p_sys->p_data = (void *)(intptr_t)atoll( psz_tmp );
    free( psz_tmp );

#ifndef argsPVLC
    psz_tmp = config_GetPsz( p_vout, "vmem-iph" );
    p_vout->p_sys->pf_iph = (void (*) (void *, char *, char *))(intptr_t)atoll( psz_tmp );
    free( psz_tmp );

    sprintf(value, "%lld", (long long int)(intptr_t)(p_vout->p_sys->p_data));
//    p_vout->p_sys->pf_iph( (void *)p_vout, "vmem-data", value );
p_vout->p_sys->pf_iph( (void *)p_vout->p_vlc, "vmem-data", value );
    p_vout->p_sys->p_data = (void *)(intptr_t)atoll( value );
    sprintf(value, "%i", i_width);
//    p_vout->p_sys->pf_iph( (void *)p_vout, "vmem-width", value );
p_vout->p_sys->pf_iph( (void *)p_vout->p_vlc, "vmem-width", value );
    i_width = atoi( value );
    sprintf(value, "%i", i_height);
//    p_vout->p_sys->pf_iph( (void *)p_vout, "vmem-height", value );
p_vout->p_sys->pf_iph( (void *)p_vout->p_vlc, "vmem-height", value );
    i_height = atoi( value );
    sprintf(value, "%i", i_pitch);
//    p_vout->p_sys->pf_iph( (void *)p_vout, "vmem-pitch", value );
p_vout->p_sys->pf_iph( (void *)p_vout->p_vlc, "vmem-pitch", value );
    i_pitch = atoi( value );
    strcpy(value, chroma);
//    p_vout->p_sys->pf_iph( (void *)p_vout, "vmem-chroma", value );
p_vout->p_sys->pf_iph( (void *)p_vout->p_vlc, "vmem-chroma", value );
    if( strlen( value ) < 4 )
    {
        msg_Err( p_vout, "vmem-chroma should be 4 characters long" );
        return VLC_EGENERIC;
    }
    i_chroma = VLC_FOURCC( value[0], value[1],
                           value[2], value[3] );
    sprintf(value, "%lld", (long long int)(intptr_t)(p_vout->p_sys->pf_lock));
//    p_vout->p_sys->pf_iph( (void *)p_vout, "vmem-lock", value );
p_vout->p_sys->pf_iph( (void *)p_vout->p_vlc, "vmem-lock", value );
    p_vout->p_sys->pf_lock = (void * (*) (void *))(intptr_t)atoll( value );
    sprintf(value, "%lld", (long long int)(intptr_t)(p_vout->p_sys->pf_unlock));
//    p_vout->p_sys->pf_iph( (void *)p_vout, "vmem-unlock", value );
p_vout->p_sys->pf_iph( (void *)p_vout->p_vlc, "vmem-unlock", value );
    p_vout->p_sys->pf_unlock = (void (*) (void *))(intptr_t)atoll( value );
#endif

    if( !p_vout->p_sys->pf_lock || !p_vout->p_sys->pf_unlock )
    {
        msg_Err( p_vout, "Invalid lock or unlock callbacks" );
        return VLC_EGENERIC;
    }

    I_OUTPUTPICTURES = 0;

    /* Initialize the output structure */
    p_vout->output.i_chroma = i_chroma;
    p_vout->output.pf_setpalette = NULL;
    p_vout->output.i_width = i_width;
    p_vout->output.i_height = i_height;
    p_vout->output.i_aspect = p_vout->output.i_width
                               * VOUT_ASPECT_FACTOR / p_vout->output.i_height;

    /* Define the bitmasks */
    switch( i_chroma )
    {
      case VLC_FOURCC( 'R','V','1','5' ):
        p_vout->output.i_rmask = 0x001f;
        p_vout->output.i_gmask = 0x03e0;
        p_vout->output.i_bmask = 0x7c00;
        break;

      case VLC_FOURCC( 'R','V','1','6' ):
        p_vout->output.i_rmask = 0x001f;
        p_vout->output.i_gmask = 0x07e0;
        p_vout->output.i_bmask = 0xf800;
        break;

      case VLC_FOURCC( 'R','V','2','4' ):
        p_vout->output.i_rmask = 0xff0000;
        p_vout->output.i_gmask = 0x00ff00;
        p_vout->output.i_bmask = 0x0000ff;
        break;

      case VLC_FOURCC( 'R','V','3','2' ):
        p_vout->output.i_rmask = 0xff0000;
        p_vout->output.i_gmask = 0x00ff00;
        p_vout->output.i_bmask = 0x0000ff;
        break;
    }

    /* Try to initialize 1 direct buffer */
    p_pic = NULL;

    /* Find an empty picture slot */
    for( i_index = 0 ; i_index < VOUT_MAX_PICTURES ; i_index++ )
    {
        if( p_vout->p_picture[ i_index ].i_status == FREE_PICTURE )
        {
            p_pic = p_vout->p_picture + i_index;
            break;
        }
    }

    /* Allocate the picture */
    if( p_pic == NULL )
    {
        return VLC_SUCCESS;
    }

    vout_InitPicture( VLC_OBJECT(p_vout), p_pic, p_vout->output.i_chroma,
                      p_vout->output.i_width, p_vout->output.i_height,
                      p_vout->output.i_aspect );

    p_pic->p->i_pitch = i_pitch;

    p_pic->pf_lock = LockPicture;
    p_pic->pf_unlock = UnlockPicture;

    p_pic->i_status = DESTROYED_PICTURE;
    p_pic->i_type   = DIRECT_PICTURE;

    PP_OUTPUTPICTURE[ I_OUTPUTPICTURES ] = p_pic;

    I_OUTPUTPICTURES++;

    return VLC_SUCCESS;
}

// GREG BEGIN
/*****************************************************************************
 * End: terminate Sys video thread output method
 *****************************************************************************
 * Terminate an output method created by Create.
 * It is called at the end of the thread.
 *****************************************************************************/
static void End( vout_thread_t *p_vout )
{
    return;
}
// GREG END

/*****************************************************************************
 * Destroy: destroy video thread
 *****************************************************************************
 * Terminate an output method created by Create
 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    vout_thread_t *p_vout = ( vout_thread_t * )p_this;

    /* Destroy structure */
    free( p_vout->p_sys );
}

/*****************************************************************************
 * LockPicture: lock a picture
 *****************************************************************************
 * This function locks the picture and prepares it for direct pixel access.
 *****************************************************************************/
static int LockPicture( vout_thread_t *p_vout, picture_t *p_pic )
{
    p_pic->p->p_pixels = p_vout->p_sys->pf_lock( p_vout->p_sys->p_data );

    return VLC_SUCCESS;
}

/*****************************************************************************
 * UnlockPicture: unlock a picture
 *****************************************************************************
 * This function unlocks a previously locked picture.
 *****************************************************************************/
static int UnlockPicture( vout_thread_t *p_vout, picture_t *p_pic )
{
    p_vout->p_sys->pf_unlock( p_vout->p_sys->p_data );

    (void)p_pic;

    return VLC_SUCCESS;
}

