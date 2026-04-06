/*
 * TransFig: Facility for Translating Fig code
 * Copyright (c) 1985 Supoj Sutantavibul
 * Copyright (c) 1991 Micah Beck
 * Parts Copyright (c) 1989-2002 by Brian V. Smith
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and
 * documentation files (the "Software"), including without limitation the
 * rights to use, copy, modify, merge, publish and/or distribute copies of
 * the Software, and to permit persons who receive copies from any such 
 * party to do so, with the only requirement being that this copyright 
 * notice remain intact.
 */

#pragma once

void arc_bound(F_arc *arc, int *xmin, int *ymin, int *xmax, int *ymax);
void compound_bound(F_compound *compound, int *xmin, int *ymin, int *xmax, int *ymax, int include);
void ellipse_bound(F_ellipse *e, int *xmin, int *ymin, int *xmax, int *ymax);
void line_bound(F_line *l, int *xmin, int *ymin, int *xmax, int *ymax);
void spline_bound(F_spline *s, int *xmin, int *ymin, int *xmax, int *ymax);
void text_bound(F_text *t, int *xmin, int *ymin, int *xmax, int *ymax, int inc_text);
void compute_arcarrow_angle(double x1, double y1, double x2, double y2, int direction, F_arrow *arrow, int *x, int *y);
void calc_arrow(int x1, int y1, int x2, int y2, int linethick, F_arrow *arrow, Point points[], int *npoints, Point  fillpoints[], int *nfillpoints, Point  clippts[], int *nclippts);
