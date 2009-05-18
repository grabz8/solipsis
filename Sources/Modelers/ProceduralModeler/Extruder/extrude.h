/*
 * FILE:
 * extrude.h
 *
 * FUNCTION:
 * Prototypes for privately used subroutines for the tubing library.
 * Applications should not include this file.
 *
 * HISTORY:
 * Copyright (C) 1991,2003 Linas Vepstas <linas@linas.org>
 */

#ifndef GLE_EXTRUDE_H_
#define GLE_EXTRUDE_H_

/* ============================================================ */
/* 
 * Provides choice of calling subroutine, vs. invoking macro.
 * Basically, inlines the source, or not.
 * Trades performance for executable size.
 */

#define INLINE_INTERSECT
#ifdef INLINE_INTERSECT
#define INNERSECT(sect,p,n,v1,v2) { int retval; INTERSECT(retval,sect,p,n,v1,v2); }
#else
#define INNERSECT(sect,p,n,v1,v2) intersect(sect,p,n,v1,v2)
#endif /* INLINE_INTERSECT */

/* ============================================================ */
/* The folowing defines give a kludgy way of accessing the qmesh primitive */

/*
#define bgntmesh _emu_qmesh_bgnqmesh
#define endtmesh _emu_qmesh_endqmesh
#define c3f _emu_qmesh_c3f
#define n3f _emu_qmesh_n3f
#define v3f _emu_qmesh_v3f
*/

/* ============================================================ */
/* Mid-level  drawing routines.  Note that some of these differ
 * only due to the different color array argument 
 */

extern void 
up_sanity_check (gleDouble up[3],      /* up vector for contour */
                    int npoints,              /* numpoints in poly-line */
                    gleDouble** point_array);   /* polyline */


extern void 
extrusion_angle_join (int ncp,      /* number of contour points */
                    gleDouble** contour,    /* 2D contour */
                    gleDouble** cont_normal,/* 2D contour normal vecs */
                    gleDouble up[3],           /* up vector for contour */
                    int npoints,               /* numpoints in poly-line */
                    gleDouble** point_array,  /* polyline */
                    gleColor color_array[],      /* color of polyline */
                    gleDouble*** xform_array);  /* 2D contour xforms */

#endif /* GLE_EXTRUDE_H_ */
/* -------------------------- end of file -------------------------------- */
