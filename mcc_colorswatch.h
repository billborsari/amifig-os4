/*
 *       AmiFIG - Custom Color Swatch MUI class
 *
 *       Renders a colored rectangle directly via RPTAG_APenColor (truecolor),
 *       bypassing MUI's ColorfieldObject which allocates pens from the screen
 *       colormap and runs out when many instances are created.
 *
 *       AROS Public License
 */

#ifndef MCC_COLORSWATCH_H
#define MCC_COLORSWATCH_H

/* Custom attribute: 0x00RRGGBB format */
#define MUIA_ColorSwatch_RGB   0xFED40001UL

struct ColorSwatchData
{
    ULONG rgb;   /* 0x00RRGGBB format */
};

#ifdef __AROS__
AROS_UFP3(IPTR, ColorSwatchDispatcher,
    AROS_UFPA(Class  *, cl,  A0),
    AROS_UFPA(Object *, obj, A2),
    AROS_UFPA(Msg     , msg, A1));
#else
IPTR ColorSwatchDispatcher(Class *cl, Object *obj, Msg msg);
#endif

#endif /* MCC_COLORSWATCH_H */
