
/*
 * MODULE: extrude.c
 *
 * FUNCTION:
 * Provides  code for the cylinder, cone and extrusion routines.
 * The cylinders/cones/etc. are built on top of general purpose
 * extrusions. The code that handles the general purpose extrusions
 * is in other modules.
 *
 * AUTHOR:
 * written by Linas Vepstas August/September 1991
 * added polycone, February 1993
 *
 * Copyright (C) 1991,1993,2003 Linas Vepstas <linas@linas.org>
 */


#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>	/* for the memcpy() subroutine */

#include "gle.h"
#include "port.h"
#include "vvector.h"
#include "tube_gc.h"
#include "extrude.h"
#include "intersect.h"

/* ============================================================ */
#ifndef COLOR_SIGNATURE
/* The routine below  determines the type of join style that will be
 * used for tubing. */

void gleSetJoinStyleOgre (int style)
{
   INIT_GC_OGRE();
   extrusion_join_style = style;
}

int gleGetJoinStyleOgre (void)
{
   INIT_GC_OGRE();
   return (extrusion_join_style);
}

#endif /* COLOR_SIGNATURE */
/* ============================================================ */
/*
 * draw a general purpose extrusion
 */

void gleSuperExtrusionOgre (int ncp,               /* number of contour points */
                gleDouble** contour,    /* 2D contour */
                gleDouble** cont_normal, /* 2D contour normals */
                gleDouble up[3],           /* up vector for contour */
                int npoints,           /* numpoints in poly-line */
                gleDouble** point_array,        /* polyline */
                gleColor color_array[],        /* color of polyline */
                gleDouble*** xform_array)   /* 2D contour xforms */
{
   INIT_GC_OGRE();
   _gle_gc_ogre -> ncp = ncp;
   _gle_gc_ogre -> contour = contour;
   _gle_gc_ogre -> cont_normal = cont_normal;
   _gle_gc_ogre -> up = up;
   _gle_gc_ogre -> npoints = npoints;
   _gle_gc_ogre -> point_array = point_array;
   _gle_gc_ogre -> color_array = color_array;
   _gle_gc_ogre -> xform_array = xform_array;
	

    (void) extrusion_angle_join_ogre (ncp, contour, cont_normal, up,
                            npoints,
                            point_array, color_array,
                            xform_array);
}
/* ============================================================ */
