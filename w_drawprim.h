/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/w_drawprim.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
extern PointList GlobalPoints;

/* Point List help functions */
void init_pointlist(PointList* PolyPoints, int NPoints);
void free_pointlist(PointList* PolyPoints);
BOOL add_point_to_list( int x, int y, PointList* PolyPoints);

/* Low level Drawing functions */
void  pw_area(PointList* PolyPoints, Color pen_color, Color fill_color, int fill_style);
void  pw_line_array(PointList* PolyPoints, BOOL closed, int style, float style_val, int join_style, int cap_style, int thickness, Color color, int op);

/* helping functions */
extern int  SetTempRaster( int width, int height, BOOL force);

extern void elastic_ellipse( int center_x, int center_y, int radius_x, int radius_y, float angle, int op);

extern void pw_polygon( F_line *line, int op);
extern BOOL pw_spline( F_spline *spline, int op);
extern void pw_vector(struct Window *win, int x, int y, int x1, int y1, int op, int line_width, int line_style, float style_val, Color color);
extern void pw_ellipse( F_ellipse *e, int op);
extern void pw_arc( F_arc *a, int op);
extern void pw_lines(struct Window *win, zXPoint *points, int npoints, int op, int depth, int thickness,
					 int style, float style_val, int join_style, int cap_style,
					 int fill_style, Color pen_color, Color fill_color);
extern void pw_arcbox( F_line *line);
