CC = gcc
CFLAGS   =  -O3 -DAMIFIG -D__USE_INLINE__ -Wall
LDFLAGS  = -lamiga -lauto
TARGET   =  AmiFIG_App/AmiFIG
SRC      =  d_arc.c           d_arcbox.c      d_box.c           d_regpoly.c     d_ellipse.c       d_line.c        d_picobj.c        d_spline.c      d_text.c          d_subspline.c   \
            e_addpt.c         e_align.c       e_convert.c       e_copy.c        e_arrow.c         e_compound.c    e_move.c          e_delete.c      e_deletept.c      e_flip.c        \
            e_glue.c          e_movept.c      e_rotate.c        e_scale.c       e_update.c        e_break.c       e_placelib.c      e_measure.c     e_chop.c          e_joinsplit.c   \
            e_tangent.c       e_editsfactor.c e_changedepth.c   \
            f_load.c          f_save.c        f_picobj.c        f_read.c        \
            u_draw_spline.c   u_bound.c       u_create.c        u_drag.c        u_draw.c          u_elastic.c     u_fonts.c         u_free.c        u_geom.c          u_list.c        \
            u_markers.c       u_redraw.c      u_scale.c         u_search.c      u_translate.c     u_undo.c        u_smartsearch.c   u_quartic.c     \
            w_rulers.c        w_cursor.c      w_grid.c          w_drawprim.c    w_msgpanel.c      w_snap.c        w_canvas.c        w_intersect.c   \
            z_fig.c           z_mode.c        z_edit.c          z_color.c       z_prefs.c         z_zoom.c        z_pict.c          z_export.c      z_about.c         z_print.c       \
			z_library.c       \
			mcc_graddisp.c    mcc_fontdisp.c  mcc_zoomdisp.c    mcc_imagedisp.c mcc_figdisp.c     mcc_arrowslider.c mcc_colorswatch.c \
            a_antialias.c     a_rexxiface.c   a_eventloop.c     a_patterns.c    a_color.c         a_area.c        a_io.c            a_savebitmap.c  a_print.c         a_icon.c        \
            a_coords.c        a_antialias_sse.c \
            resources.c       object.c        intui.c           mode.c          GradientFill.c    locale.c        main.c
OBJS     =  $(SRC:.c=.o)

# a_antialias_sse.o: CFLAGS+=-mfpmath=sse -msse3

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) 

#-include $(OBJS:.o=.d)

%.o: %.c
	$(CC) $(CFLAGS) $(DEFINES) -o $@ -c $<
#	$(CC) -MM $(CFLAGS) $*.c > $*.d

main.o z_fig.o z_about.o: version.h
clean:
	rm -f $(OBJS) *.d *~
