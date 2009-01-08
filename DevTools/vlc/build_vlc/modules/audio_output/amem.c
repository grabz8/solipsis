/*****************************************************************************
 * sdl.c : SDL audio output plugin for vlc
 *****************************************************************************
 * Copyright (C) 2000-2002 the VideoLAN team
 * $Id: sdl.c 14568 2006-03-02 13:35:43Z courmisch $
 *
 * Authors: Michel Kaempf <maxx@via.ecp.fr>
 *          Sam Hocevar <sam@zoy.org>
 *          Pierre Baillet <oct@zoy.org>
 *          Christophe Massiot <massiot@via.ecp.fr>
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
#include <string.h>                                            /* strerror() */
#include <unistd.h>                                      /* write(), close() */
#include <stdlib.h>                            /* calloc(), malloc(), free() */

#include <vlc/vlc.h>
#include <vlc/aout.h>
#include "aout_internal.h"

#define FRAME_SIZE 2048

/*****************************************************************************
 * Local prototypes
 *****************************************************************************/
static int  Open        ( vlc_object_t * );
static void Close       ( vlc_object_t * );
static void Play        ( aout_instance_t * );

/*****************************************************************************
 * Module descriptor
 *****************************************************************************/
#define T_OPENSB N_( "Open the sound buffer function" )
#define LT_OPENSB N_( "Address of the opensb callback function." )

#define T_PLAYSB N_( "Play function" )
#define LT_PLAYSB N_( "Address of the playsb callback function." )

#define T_CLOSESB N_( "Close the sound buffer function" )
#define LT_CLOSESB N_( "Address of the closesb callback function." )

#define T_DATA N_( "Callback data" )
#define LT_DATA N_( "Data for the openning, playing and closing functions" )

vlc_module_begin();
    set_description( _( "Audio memory module" ) );
    set_shortname( _("Audio memory") );

    set_category( CAT_AUDIO );
    set_subcategory( SUBCAT_AUDIO_AOUT );
    set_capability( "audio output", 40 );

    add_string( "amem-opensb", "0", NULL, T_OPENSB, LT_OPENSB, VLC_TRUE );
    add_string( "amem-playsb", "0", NULL, T_PLAYSB, LT_PLAYSB, VLC_TRUE );
    add_string( "amem-closesb", "0", NULL, T_CLOSESB, LT_CLOSESB, VLC_TRUE );
    add_string( "amem-data", "0", NULL, T_DATA, LT_DATA, VLC_TRUE );

    set_callbacks( Open, Close );
vlc_module_end();

/*****************************************************************************
 * aout_sys_t: audio output descriptor
 *****************************************************************************/
struct aout_sys_t
{
    void (*pf_opensb) (void *, unsigned int *frequency, unsigned int *nbChannels, unsigned int *fourCCFormat, unsigned int *frameSize);
    void (*pf_playsb) (void *, unsigned char *buffer, size_t bufferSize, unsigned int nbSamples);
    void (*pf_closesb) (void *);
    void *p_data;
};

/*****************************************************************************
 * Open: open the audio device
 *****************************************************************************/
static int Open ( vlc_object_t *p_this )
{
    aout_instance_t *p_aout = (aout_instance_t *)p_this;
    char *psz_tmp;

    /* Allocate instance and initialize some members */
    p_aout->output.p_sys = malloc( sizeof( aout_sys_t ) );
    if( ! p_aout->output.p_sys )
        return VLC_ENOMEM;

    psz_tmp = config_GetPsz( p_aout, "amem-opensb" );
    p_aout->output.p_sys->pf_opensb = (void (*) (void *, unsigned int *, unsigned int *, unsigned int *, unsigned int *))(intptr_t)atoll( psz_tmp );
    free( psz_tmp );

    psz_tmp = config_GetPsz( p_aout, "amem-playsb" );
    p_aout->output.p_sys->pf_playsb = (void (*) (void *, unsigned char *, size_t, unsigned int))(intptr_t)atoll( psz_tmp );
    free( psz_tmp );

    psz_tmp = config_GetPsz( p_aout, "amem-closesb" );
    p_aout->output.p_sys->pf_closesb = (void (*) (void *))(intptr_t)atoll( psz_tmp );
    free( psz_tmp );

    psz_tmp = config_GetPsz( p_aout, "amem-data" );
    p_aout->output.p_sys->p_data = (void *)(intptr_t)atoll( psz_tmp );
    free( psz_tmp );

    // Open the sound buffer
    unsigned int frequency = p_aout->output.output.i_rate;
    unsigned int nbChannels = aout_FormatNbChannels( &p_aout->output.output );
    if (nbChannels > 2) nbChannels = 2;
    unsigned int fourCCFormat = VLC_FOURCC('s','1','6','l');
    unsigned int frameSize = FRAME_SIZE;
    p_aout->output.p_sys->pf_opensb( p_aout->output.p_sys->p_data, &frequency, &nbChannels, &fourCCFormat, &frameSize);
    // get returned format
    p_aout->output.output.i_rate = frequency;
    p_aout->output.output.i_physical_channels = ((nbChannels >= 2) ? (AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT) : AOUT_CHAN_CENTER);
    p_aout->output.output.i_format = fourCCFormat;
    p_aout->output.i_nb_samples = frameSize;

    /* Volume is entirely done in software. */
    aout_VolumeSoftInit( p_aout );

    p_aout->output.pf_play = Play;

    return VLC_SUCCESS;
}

/*****************************************************************************
 * Play: play a sound samples buffer
 *****************************************************************************/
static void Play( aout_instance_t * p_aout )
{
    aout_buffer_t *   p_buffer;

    p_buffer = aout_FifoPop( p_aout, &p_aout->output.fifo );

    p_aout->output.p_sys->pf_playsb( p_aout->output.p_sys->p_data, p_buffer->p_buffer, p_buffer->i_nb_bytes, p_buffer->i_nb_samples );

    aout_BufferFree( p_buffer );
}

/*****************************************************************************
 * Close: close the audio device
 *****************************************************************************/
static void Close ( vlc_object_t *p_this )
{
    aout_instance_t *p_aout = (aout_instance_t *)p_this;

    p_aout->output.p_sys->pf_closesb( p_aout->output.p_sys->p_data );

    /* Destroy structure */
    free( p_aout->output.p_sys );
}
