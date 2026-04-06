# ARexx Support in AmiFIG

AmiFIG provides an ARexx interface that allows for automation and external control. The default public port name is `AMIFIG.0`.

## How to Use

To send commands to AmiFIG from an ARexx script, use the `ADDRESS` command:

```rexx
/* Example */
ADDRESS AMIFIG.0
'Clear'
'DrawCircle 100 100 50'
```

## Coordinate Systems

Commands that accept coordinates (X, Y, Radius, etc.) are influenced by the `SetBaseSystem` command.

| System | Mode | Description |
| :--- | :--- | :--- |
| **dpi** | 0 | Coordinates are in pixels at 1200 DPI (default). |
| **metric** | 1 | Coordinates are in centimeters (cm). |
| **imperial** | 2 | Coordinates are in inches (in). |

## Command Reference

### General Commands

| Command | Template | Description |
| :--- | :--- | :--- |
| **Clear** | `NAME` | Clears the current figure. |
| **LoadFile** | `NAME/A` | Loads a FIG file. |
| **SaveFile** | `NAME/A` | Saves the current figure to a FIG file. |
| **ExportFile** | `NAME/A, MODE/A, LEFT/A, TOP/A, WIDTH/A, HEIGHT/A` | Exports the figure. Modes: `png`, `fig2dev`. |
| **Undo** | | Undoes the last action. |
| **SetZoomScale** | `ARG/A` | Sets the zoom scale (1-999). |
| **SetBaseSystem** | `ARG/A` | Sets the coordinate system: `dpi`, `metric`, or `imperial`. |
| **testfunc** | | Internal test function. Returns a result string. |

### Attribute Commands

| Command | Template | Description |
| :--- | :--- | :--- |
| **SetDepth** | `DEPTH/N/A` | Sets the current drawing depth (0-999). |
| **SetLineWidth** | `WIDTH/N/A` | Sets the line width (0-99). |
| **SetLineStyle** | `STYLE/N/A` | Sets the line style (0: Solid, 1: Dashed, 2: Dotted, 3: Dash-Dot, 4: Dash-2Dots, 5: Dash-3Dots). |
| **SetJoinStyle** | `STYLE/N/A` | Sets the join style (0: Miter, 1: Round, 2: Bevel). |
| **SetCapStyle** | `STYLE/N/A` | Sets the cap style (0: Butt, 1: Round, 2: Projecting). |
| **SetArrowStyle** | `TYPE/N/A, FILLED/N/A` | Sets arrow type (0-3) and fill style (0: Outline, 1: Filled). |
| **SetAutoArrows** | `FORWARD/N/A, BACKWARD/N/A` | Enables/disables auto-arrows (0: Off, 1: On). |
| **SetPenColor** | `COLOR/N/A` | Sets the pen color index. |
| **SetFillColor** | `COLOR/N/A` | Sets the fill color index. |
| **SetFillShade** | `SHADE/N/A` | Sets the fill shade/tint value. |
| **SetUnfilled** | | Disables filling for new objects. |
| **SetPattern** | `PATTERN/N/A` | Sets the fill pattern index. |

### Drawing Commands - Basic Shapes

| Command | Template | Description |
| :--- | :--- | :--- |
| **DrawCircle** | `X/A, Y/A, RADIUS/A` | Draws a circle. |
| **DrawEllipse** | `X/A, Y/A, RADIUS1/A, RADIUS2/A` | Draws an ellipse. |
| **DrawRectBox** | `X1/A, Y1/A, X2/A, Y2/A` | Draws a rectangular box. |
| **DrawText** | `X/A, Y/A, TEXT/A` | Draws text at the specified location. |
| **DrawArc** | `X1/A, Y1/A, X2/A, Y2/A, X3/A, Y3/A` | Draws an open arc through three points. |
| **DrawClosedArc** | `X1/A, Y1/A, X2/A, Y2/A, X3/A, Y3/A` | Draws a closed (pie wedge) arc. |
| **DrawPicture** | `X1/A, Y1/A, X2/A, Y2/A, FILE/A` | Places a picture object. |

### Drawing Commands - Multi-point Objects

Multi-point objects (Polylines, Polygons, Splines) are created in stages:

1. **Start** the object.
2. **Add** zero or more intermediate segments.
3. **End** the object.

| Command | Template | Description |
| :--- | :--- | :--- |
| **StartPolyline** | `X/A, Y/A` | Starts a new polyline. |
| **StartPolygon** | `X/A, Y/A` | Starts a new closed polygon. |
| **AddLineSegment** | `X/A, Y/A` | Adds a point to the current polyline/polygon. |
| **EndLine** | `X/A, Y/A` | Finishes the current polyline/polygon at the specified point. |
| **StartSpline** | `X/A, Y/A` | Starts an open approximated spline. |
| **StartClosedSpline** | `X/A, Y/A` | Starts a closed approximated spline. |
| **StartInterpSpline** | `X/A, Y/A` | Starts an open interpolated spline. |
| **StartClosedInterpSpline** | `X/A, Y/A` | Starts a closed interpolated spline. |
| **AddSplineSegment** | `X/A, Y/A` | Adds a control point to the current spline. |
| **EndSpline** | `X/A, Y/A` | Finishes the current spline at the specified point. |

### Text Attribute Commands

| Command | Template | Description |
| :--- | :--- | :--- |
| **SetDisplayFont** | `SIZE/N/A` | Sets the font size. |
| **SetTextAdjustment** | `ARG/N/A` | Sets text justification (0: Left, 1: Center, 2: Right). |
| **SetTextAngle** | `ANGLE/A` | Sets the text rotation angle in degrees. |
| **SetEllipseAngle** | `ANGLE/A` | Sets the rotation angle for ellipses and text. |
| **SetRigidTextFlag** | `ARG/N/A` | Sets the rigid text flag (0: Off, 1: On). |
| **SetSpecialTextFlag** | `ARG/N/A` | Sets the special text flag (0: Off, 1: On). |
