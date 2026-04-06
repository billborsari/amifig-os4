/****************************************************************
   This file was created automatically by `FlexCat 2.16'
   from "AmiFIG.cd".

   Do NOT edit by hand!
****************************************************************/

#ifndef AMIFIG_STRINGS_H
#define AMIFIG_STRINGS_H


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif


#ifdef CATCOMP_ARRAY
#ifndef CATCOMPARRAY
#define CATCOMPARRAY CatCompArray
#endif
#undef CATCOMP_NUMBERS
#undef CATCOMP_STRINGS
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#endif

#ifdef CATCOMP_BLOCK
#undef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif

/***************************************************************/

#ifdef CATCOMP_NUMBERS

#define msg_ActionCancelled 0
#define msg_AddPoints 1
#define msg_Align 2
#define msg_Antialising 3
#define msg_Arc 4
#define msg_ArrowHead 5
#define msg_AutoEndArrow 6
#define msg_AutoStartArrow 7
#define msg_BMAllocateErr 8
#define msg_Box2Polygon 9
#define msg_CanTMoveCenterPt 10
#define msg_CircleDiameter 11
#define msg_CircleRadius 12
#define msg_ClosedInterpSpline 13
#define msg_ClosedSpline 14
#define msg_ClosedSplineMinPoints 15
#define msg_CompoundBreak 16
#define msg_CompoundCreate 17
#define msg_CompoundGetAttrErr 18
#define msg_CompoundOpen 19
#define msg_ConvertLine2Spline 20
#define msg_CopyObjects 21
#define msg_DatatypeError 22
#define msg_DeleteObjects 23
#define msg_DeletePoints 24
#define msg_DeletingPrevFromMem 25
#define msg_DepthLimits 26
#define msg_DepthTooBig 27
#define msg_DepthTooSmall 28
#define msg_Done 29
#define msg_DotMinPoint 30
#define msg_EditObjects 31
#define msg_EllipseDiameter 32
#define msg_EllipseRadius 33
#define msg_EmptyName 34
#define msg_ExportAsSel 35
#define msg_ExportOutput 36
#define msg_ExportSel 37
#define msg_ExportWhole 38
#define msg_FIG2DEVNotConfigured 39
#define msg_Fig30Bug 40
#define msg_FigModCont 41
#define msg_FigResOrCoordMissing 42
#define msg_FigureDelete 43
#define msg_FigureFormatConvert 44
#define msg_FigureScaling 45
#define msg_FigureShift 46
#define msg_FlipHoriz 47
#define msg_FlipVert 48
#define msg_FontNotAvailable 49
#define msg_GetAttributes 50
#define msg_Height 51
#define msg_IllegalPaperSize 52
#define msg_IncompleteObject 53
#define msg_IncorrectFormatLine 54
#define msg_IncorrectObjectCode 55
#define msg_InterpSpline 56
#define msg_InvalidColor 57
#define msg_LatexArrow 58
#define msg_LatexLine 59
#define msg_LayerErr 60
#define msg_LayerInfoErr 61
#define msg_Left 62
#define msg_Manhattan 63
#define msg_MenuAbout 64
#define msg_MenuAsPNG 65
#define msg_MenuClear 66
#define msg_MenuColor 67
#define msg_MenuEdit 68
#define msg_MenuEditSettings 69
#define msg_MenuExport 70
#define msg_MenuImport 71
#define msg_MenuInsert 72
#define msg_MenuNew 73
#define msg_MenuOpen 74
#define msg_MenuOpenLibWnd 75
#define msg_MenuPrint 76
#define msg_MenuPrintPS 77
#define msg_MenuProject 78
#define msg_MenuQuit 79
#define msg_MenuR2V 80
#define msg_MenuRedisplay 81
#define msg_MenuSandQ 82
#define msg_MenuSave 83
#define msg_MenuSaveAs 84
#define msg_MenuSettings 85
#define msg_MenuStatus 86
#define msg_MenuUndo 87
#define msg_MenuViaF2D 88
#define msg_MenuZoom 89
#define msg_Mountain 90
#define msg_MountHattan 91
#define msg_moveobjects 92
#define msg_movepoints 93
#define msg_NegCoordShift 94
#define msg_NoAlignSpec 95
#define msg_NoFigFile 96
#define msg_NoFile 97
#define msg_NoMagnSpec 98
#define msg_NoMemForBM 99
#define msg_NoMultiPageSpec 100
#define msg_NoOrientSpec 101
#define msg_NoPaperSizeSpec 102
#define msg_NoResolutionSpec 103
#define msg_NothingToSave 104
#define msg_NoTranspColorSpec 105
#define msg_NoUndo 106
#define msg_NoUnitSpec 107
#define msg_ObjectsInFile 108
#define msg_ObjectsInModFile 109
#define msg_ObjectsSavedIn 110
#define msg_ObjectUpdated 111
#define msg_OffScreenRendering 112
#define msg_OK 113
#define msg_Only90degRotation 114
#define msg_OpenFileErr 115
#define msg_OpenScreenErr 116
#define msg_OutOfMem 117
#define msg_PaintWndErr 118
#define msg_Picture 119
#define msg_PointAngle 120
#define msg_Polygon 121
#define msg_PolygonMinPoints 122
#define msg_PolyLine 123
#define msg_PreferredScreenErr 124
#define msg_PrefsDrawInBackDrop 125
#define msg_PrefsUsePubScr 126
#define msg_PRINT 127
#define msg_PrintBMErr 128
#define msg_PrinterDeviceErr 129
#define msg_PrinterErr 130
#define msg_PrinterPrefs 131
#define msg_PrintingProgress 132
#define msg_PrintNoFigure 133
#define msg_PrintSel 134
#define msg_PublicScreenErr 135
#define msg_RAS2VECNotConfigured 136
#define msg_RastportInitErr 137
#define msg_ReadingFrom 138
#define msg_ReadPicFile 139
#define msg_RectBox 140
#define msg_RotAngle 141
#define msg_RotateObjectCCW 142
#define msg_RotateObjectCW 143
#define msg_SavingBitmap 144
#define msg_Spline 145
#define msg_SplineMinPoint 146
#define msg_TextInput 147
#define msg_TextJustErr 148
#define msg_TextStringParsingErr 149
#define msg_TooManyEllPoint 150
#define msg_TooMuchUserColors 151
#define msg_Top 152
#define msg_TruncateText 153
#define msg_Uncontraint 154
#define msg_UndoDone 155
#define msg_UndoFigureRestore 156
#define msg_UpdateAttributes 157
#define msg_VersionNOK 158
#define msg_Width 159
#define msg_WriteFileErr 160
#define msg_Writing 161
#define msg_WrongColorDefPosition 162
#define msg_WrongRotationAngle 163
#define msg_1stAngle 164
#define msg_AA 165
#define msg_AbutHorError 166
#define msg_AbutVerError 167
#define msg_AccesDeniedError 168
#define msg_AddLibrary 169
#define msg_AddNewLibraryObjectsToList 170
#define msg_AddOneColorStop 171
#define msg_AddStop 172
#define msg_AddTangentOrNormal 173
#define msg_All 174
#define msg_AllowNegCoords 175
#define msg_RadOrDimAndAngle 176
#define msg_AngleEndPtError 177
#define msg_AngleError 178
#define msg_ArcBoxAreaError 179
#define msg_ArcBoxSelected 180
#define msg_ArcChopped 181
#define msg_ArcDoesntIntersect 182
#define msg_ArcType 183
#define msg_AreaOfIs 184
#define msg_AreaOfIsAcc 185
#define msg_AreaError 186
#define msg_AreaReset 187
#define msg_Arrow 188
#define msg_AutoactivateWin 189
#define msg_AutoChopError 190
#define msg_AutoloadLibrary 191
#define msg_AutomaticEndArrow 192
#define msg_AutomaticStartArrow 193
#define msg_AutoScroll 194
#define msg_AxeListCleared 195
#define msg_AxeObjSelected 196
#define msg_Bevel 197
#define msg_Boring 198
#define msg_BoxAreaError 199
#define msg_BoxColor 200
#define msg_BoxThickness 201
#define msg_Butt 202
#define msg_Cancel 203
#define msg_Cap 204
#define msg_CapStyle 205
#define msg_Center 206
#define msg_CenterPtIgnored 207
#define msg_ChangeDispFont 208
#define msg_ChopObject 209
#define msg_ChopOnlyUnconstPolyX 210
#define msg_CircleNoEndPt 211
#define msg_ClickToChange 212
#define msg_Close 213
#define msg_CloseCompound 214
#define msg_Closed 215
#define msg_ClosedFigReq2intersects 216
#define msg_ClosedSpline3ptsError 217
#define msg_ClosestPtNotOnArc 218
#define msg_Color 219
#define msg_Command 220
#define msg_ConstMoveCircleError 221
#define msg_Copy 222
#define msg_CopyNewGradient 223
#define msg_CouldntOpenCutFile 224
#define msg_CreateIcon 225
#define msg_CreateNewGradient 226
#define msg_CutFileReadOnly 227
#define msg_Dash1Dot 228
#define msg_Dash2Dot 229
#define msg_Dash3Dot 230
#define msg_Dashed 231
#define msg_DashLength 232
#define msg_Debug 233
#define msg_DefaultTool 234
#define msg_DefineSystemFonts 235
#define msg_Degrees 236
#define msg_Delete 237
#define msg_DeleteGradient 238
#define msg_DeletePolygon3pts 239
#define msg_DeleteZeroSizeObject 240
#define msg_Density 241
#define msg_Depth 242
#define msg_Dim 243
#define msg_DimensionLine 244
#define msg_DiscardModifications 245
#define msg_Display 246
#define msg_DisplayUnit 247
#define msg_DistHorError 248
#define msg_DistVerError 249
#define msg_Dotted 250
#define msg_DuplicateAxeError 251
#define msg_EditColors 252
#define msg_EllipseAngle 253
#define msg_EllipseArcNotSupported 254
#define msg_EllipseChopError 255
#define msg_EllipseChopped 256
#define msg_EllipseSplineIntersetNotImpl 257
#define msg_EmptyCompoundIgnored 258
#define msg_EmptyRegionUnchanged 259
#define msg_EnterPictName 260
#define msg_Export 261
#define msg_F2Dexport 262
#define msg_Figure 263
#define msg_FileHasNoVersionNumber 264
#define msg_FileIsADirectory 265
#define msg_FileIsEmpty 266
#define msg_FileIsNotAccessible 267
#define msg_FilenameIsTooLong 268
#define msg_FileNotExisting 269
#define msg_Fill 270
#define msg_FillColor 271
#define msg_FillStyle 272
#define msg_FillType 273
#define msg_FixedFocalPt 274
#define msg_FixedText 275
#define msg_Flags 276
#define msg_Fonts 277
#define msg_Format 278
#define msg_Fractionnal 279
#define msg_FreehandRes 280
#define msg_Front_Back 281
#define msg_GapLength 282
#define msg_Gradient 283
#define msg_Grid 284
#define msg_GridUnit 285
#define msg_HeightPercent 286
#define msg_HollowNice 287
#define msg_HollowSpear 288
#define msg_HollowTriangle 289
#define msg_HQDisplay 290
#define msg_Imperial 291
#define msg_InputOption 292
#define msg_InsertFile 293
#define msg_InvalidArcGeometry 294
#define msg_InvalidTextFont 295
#define msg_IO 296
#define msg_IsaDirectoryError 297
#define msg_Join 298
#define msg_JoinStyle 299
#define msg_Justification 300
#define msg_Landscape 301
#define msg_LatexArrows 302
#define msg_LatexLines 303
#define msg_LeftArrow 304
#define msg_LengthMeas 305
#define msg_LengthError 306
#define msg_LengthOfIs 307
#define msg_LengthOfIsAcc 308
#define msg_LengthReset 309
#define msg_Line 310
#define msg_Linear 311
#define msg_LineStyle 312
#define msg_LineWidth 313
#define msg_CreateGradient 314
#define msg_MeasAngle 315
#define msg_MeasArea 316
#define msg_MeasLength 317
#define msg_Metric 318
#define msg_Misc 319
#define msg_Miter 320
#define msg_New 321
#define msg_NoFill 322
#define msg_NoIntersection 323
#define msg_None 324
#define msg_NoPoint 325
#define msg_NoPriorPtForDiameter 326
#define msg_NoPriorPtForNormal 327
#define msg_NoPriorPtForTangent 328
#define msg_NormalAdded 329
#define msg_NormalImpossible 330
#define msg_NormalPtNotOnArc 331
#define msg_NotADirectory 332
#define msg_NotEnoughPtForOpenSpline 333
#define msg_NotEnoughPtForPolygon 334
#define msg_NotEnoughPtForSpline 335
#define msg_Object 336
#define msg_ObjectCopiedToScrapFile 337
#define msg_ObjectDeletedFromScrapFile 338
#define msg_OnScreenAA 339
#define msg_Open 340
#define msg_OpenColorWindow 341
#define msg_OpenFile 342
#define msg_OpenPrefFile 343
#define msg_Orientation 344
#define msg_OutputOption 345
#define msg_Pad 346
#define msg_Page 347
#define msg_Pattern 348
#define msg_PenColor 349
#define msg_PictureFile 350
#define msg_PictureSelection 351
#define msg_PictureSize 352
#define msg_PlaceLibObj 353
#define msg_Plain 354
#define msg_PlainNice 355
#define msg_PlainSpear 356
#define msg_PlainTriangle 357
#define msg_PolylineChopError 358
#define msg_PolylineChopped 359
#define msg_PolylineSplineIntersectNotImpl 360
#define msg_PolylineTangentError 361
#define msg_Portrait 362
#define msg_Precision 363
#define msg_PrintBuffer 364
#define msg_Project 365
#define msg_Proportional 366
#define msg_PubScreen 367
#define msg_R2VImport 368
#define msg_Radial 369
#define msg_Radius 370
#define msg_Radiuses 371
#define msg_ReadAccessBlocked 372
#define msg_RegpolySelected 373
#define msg_RemoveLastColorStop 374
#define msg_RemovePtPictError 375
#define msg_RemoveStop 376
#define msg_Repeat 377
#define msg_Reset 378
#define msg_Right 379
#define msg_RightArrow 380
#define msg_RigidText 381
#define msg_Round 382
#define msg_Save 383
#define msg_SaveFile 384
#define msg_SavePalette 385
#define msg_SavePrefFile 386
#define msg_ScaleSelected 387
#define msg_Scaling 388
#define msg_ScreenMode 389
#define msg_ScreenModeSelect 390
#define msg_ScreenOption 391
#define msg_Select2ndPt 392
#define msg_SelectAllFields 393
#define msg_SelectDirectory 394
#define msg_SelectedCirclesAreCongruent 395
#define msg_SelectedEllAndPolylineNoInter 396
#define msg_SelectedPolylineNoIntersect 397
#define msg_SelectPattern 398
#define msg_SelectSecondColor 399
#define msg_SetOriginalPicSize 400
#define msg_SFactorEdit 401
#define msg_SFactorValue 402
#define msg_ShowLength 403
#define msg_ShowNums 404
#define msg_SideRuler 405
#define msg_SignedAreaReset 406
#define msg_SingularityError 407
#define msg_Size 408
#define msg_Skin 409
#define msg_SkinSelect 410
#define msg_Snap 411
#define msg_SoftScroll 412
#define msg_Solid 413
#define msg_SpecialText 414
#define msg_SplineArcInterNotImpl 415
#define msg_SplineDiaNotImpl 416
#define msg_SplineEndPtNotImpl 417
#define msg_SplineMidPtNotImpl 418
#define msg_SplineOnePtCutError 419
#define msg_SplineSnapNotImpl 420
#define msg_SplineSplineInterNotImpl 421
#define msg_SplineTangentNotImpl 422
#define msg_SplineTextInterNotImpl 423
#define msg_SplinsNormalNotImpl 424
#define msg_StartColor 425
#define msg_StatCheckErrorCutFile 426
#define msg_StdIO 427
#define msg_StepNb 428
#define msg_Stroke 429
#define msg_Style 430
#define msg_SVGImport 431
#define msg_Symetric 432
#define msg_TangentAdded 433
#define msg_TangentImpossible 434
#define msg_TangentOnlyForPolylines 435
#define msg_TangentPtNotOnArc 436
#define msg_Tenth 437
#define msg_Text 438
#define msg_TextAdjust 439
#define msg_TextAngle 440
#define msg_TextBox 441
#define msg_TextColor 442
#define msg_TextFont 443
#define msg_TextRigid 444
#define msg_TextSize 445
#define msg_TextSpecial 446
#define msg_Thickness 447
#define msg_TickThickness 448
#define msg_TopRuler 449
#define msg_Unconstrained 450
#define msg_Undo 451
#define msg_UnselectAll 452
#define msg_Updates 453
#define msg_Use 454
#define msg_UsePref 455
#define msg_UserColors 456
#define msg_UserScale 457
#define msg_UseTicks 458
#define msg_WidthHeightFactor 459
#define msg_WidthLengthFactor 460
#define msg_WidthPercent 461
#define msg_WriteBackup 462
#define msg_ZoomFactor 463
#define msg_ZoomIn 464
#define msg_ZoomOut 465
#define msg_OtherOptions 466
#define msg_SelectShadeTint 467
#define msg_SelectSVGfile 468
#define msg_LastMsg 469
#define msg_MouseFunction 470
#define msg_BoxScaleMsg1 471
#define msg_BoxScaleMsg2 472
#define msg_Diameter 473
#define msg_Length 474
#define msg_Distance 475
#define msg_Locate_Object 476
#define msg_first_point 477
#define msg_next_point 478
#define msg_center_point 479
#define msg_mid_point 480
#define msg_final_point 481
#define msg_corner_point 482
#define msg_delete_point 483
#define msg_direction 484
#define msg_Circle_center 485
#define msg_Ellipse_center 486
#define msg_Ellipse_corner 487
#define msg_Circle_diameter 488
#define msg_final_corner 489
#define msg_set_radius 490
#define msg_final_angle 491
#define msg_freehand 492
#define msg_single_point 493
#define msg_dimension_line 494
#define msg_position_cursor 495
#define msg_break_add_here 496
#define msg_place_new_point 497
#define msg_align_compound 498
#define msg_align_canvas 499
#define msg_tag_object 500
#define msg_tag_region 501
#define msg_align_tagged 502
#define msg_add_arrow 503
#define msg_delete_arrow 504
#define msg_break_compound 505
#define msg_break_and_tag 506
#define msg_Put_Front 507
#define msg_Put_Back 508
#define msg_Select_axe_object 509
#define msg_Select_log_object 510
#define msg_Clear_axe_list 511
#define msg_open_compound 512
#define msg_open_keep_visible 513
#define msg_spline_line 514
#define msg_open_closed 515
#define msg_copy_object 516
#define msg_horiz_vert_copy 517
#define msg_copy_to_cut_buf 518
#define msg_place_object 519
#define msg_array_placement 520
#define msg_delete_object 521
#define msg_delete_region 522
#define msg_del_to_cut_buf 523
#define msg_Modify_SFactor 524
#define msg_Set 525
#define msg_flip 526
#define msg_copy_flip 527
#define msg_set_anchor 528
#define msg_unset_anchor 529
#define msg_compound_tagged 530
#define msg_Join_Lines_Splines 531
#define msg_Split_Line_Spline 532
#define msg_Choose_next_Line 533
#define msg_Choose_next_Spline 534
#define msg_select_save 535
#define msg_select_object 536
#define msg_angle_tip 537
#define msg_save_angle 538
#define msg_select_add 539
#define msg_reset_to_0 540
#define msg_move_object 541
#define msg_horiz_vert_move 542
#define msg_move_point 543
#define msg_new_posn 544
#define msg_new_object 545
#define msg_cancel_library 546
#define msg_rotate_object 547
#define msg_copy_rotate 548
#define msg_set_center 549
#define msg_unset_center 550
#define msg_scale_box 551
#define msg_scale_about_center 552
#define msg_add_tangent 553
#define msg_add_normal 554
#define msg_update_object 555
#define msg_Get_settings 556
#define msg_place_and_edit 557
#define msg_change_draw_mode 558
#define msg_place_at_orig 559
#define msg_HelpWnd 560
#define msg_SkinChange 561
#define msg_SwapColorStop 562
#define msg_SecondSwapStop 563
#define msg_SwapDone 564
#define msg_Swap 565
#define msg_Add 566
#define msg_Del 567
#define msg_CopyColor 568
#define msg_SwapColor 569
#define msg_SelectCopyColor 570
#define msg_SelectSwapColor 571
#define msg_ColorPicker 572
#define msg_GradDSpread 573
#define msg_FreeHandMode 574
#define msg_AutoSaveSettings 575
#define msg_OffScreenFailed 576
#define msg_TooManyPts 577

#endif /* CATCOMP_NUMBERS */

/***************************************************************/

#ifdef CATCOMP_STRINGS

#define msg_ActionCancelled_STR "Action canceled"
#define msg_AddPoints_STR "Add points (to polyline, polygon, closed-spline and spline)"
#define msg_Align_STR "Align objects with bounding box or via shift select"
#define msg_Antialising_STR "Antialiasing"
#define msg_Arc_STR "Arc drawing: specify three points on the arc"
#define msg_ArrowHead_STR "Add/delete arrow head (left button)"
#define msg_AutoEndArrow_STR "Auto forward arrow (for arc, polyline and spline)"
#define msg_AutoStartArrow_STR "Auto backward arrow (for arc, polyline and spline)"
#define msg_BMAllocateErr_STR "Could not allocate bitmap"
#define msg_Box2Polygon_STR "Converted box into polygon!"
#define msg_CanTMoveCenterPt_STR "Cannot move center point"
#define msg_CircleDiameter_STR "Circle drawing: specify diameter"
#define msg_CircleRadius_STR "Circle drawing: specify radius"
#define msg_ClosedInterpSpline_STR "Closed interpolated spline drawing"
#define msg_ClosedSpline_STR "Closed spline drawing: specify control points"
#define msg_ClosedSplineMinPoints_STR "A closed spline cannot have less than %d points"
#define msg_CompoundBreak_STR "Break compound object"
#define msg_CompoundCreate_STR "Glue objects into compound object with bounding box or via shift select"
#define msg_CompoundGetAttrErr_STR "Geting attributes from compound objects is not possible!"
#define msg_CompoundOpen_STR "Open compound object for editing"
#define msg_ConvertLine2Spline_STR "Convert polygon (polyline) into closed-spline (spline) or vice versa"
#define msg_CopyObjects_STR "Copy objects"
#define msg_DatatypeError_STR "Writing through datatypes failed"
#define msg_DeleteObjects_STR "Remove objects"
#define msg_DeletePoints_STR "Delete points (from polyline, polygon, closed-spline and spline)"
#define msg_DeletingPrevFromMem_STR "Deleting previous figure from memory"
#define msg_DepthLimits_STR "Depth must be from 0 to 999"
#define msg_DepthTooBig_STR "Depth > %d, setting to %d in line %d"
#define msg_DepthTooSmall_STR "Depth < 0, setting to 0 in line %d"
#define msg_Done_STR "Done"
#define msg_DotMinPoint_STR "A dot must have at least 1 point"
#define msg_EditObjects_STR "Edit \"%s\" %d objects"
#define msg_EllipseDiameter_STR "Ellipse drawing: specify diameters"
#define msg_EllipseRadius_STR "Ellipse drawing: specify radiuses"
#define msg_EmptyName_STR "Empty name"
#define msg_ExportAsSel_STR "As Selection"
#define msg_ExportOutput_STR "Output Resolution"
#define msg_ExportSel_STR "Selection"
#define msg_ExportWhole_STR "Whole Page"
#define msg_FIG2DEVNotConfigured_STR "Exporting via fig2dev is not configured"
#define msg_Fig30Bug_STR "Warning, because of a bug in version 3.0 you may need to offset\nyour figure by 14 fig units in X and Y if this figure was\nconverted from an older version of xfig. See the File panel."
#define msg_FigModCont_STR "Figure modified\nContinue?"
#define msg_FigResOrCoordMissing_STR "Figure resolution or coordinate specifier missing in line %d"
#define msg_FigureDelete_STR "Deleting figure from memory"
#define msg_FigureFormatConvert_STR "Converting figure from %ld format to %ld"
#define msg_FigureScaling_STR "Scaling figure by a factor of %.3f for new %d pixel per inch resolution"
#define msg_FigureShift_STR "Shifting entire figure %ld pixels right and %ld pixels down to keep on page"
#define msg_FlipHoriz_STR "Flip objects (left button) or copy & flip (right button) up or down"
#define msg_FlipVert_STR "Flip objects (left button) or copy & flip (right button) left or right"
#define msg_FontNotAvailable_STR "Font \"%s\" not available in %d pts"
#define msg_GetAttributes_STR "Get current settings from objects"
#define msg_Height_STR "Height"
#define msg_IllegalPaperSize_STR "Illegal paper size in file, using default"
#define msg_IncompleteObject_STR "Incomplete %s object at line %d"
#define msg_IncorrectFormatLine_STR "Incorrect format at line %d"
#define msg_IncorrectObjectCode_STR "Incorrect object code at line %d"
#define msg_InterpSpline_STR "Interpolated spline drawing"
#define msg_InvalidColor_STR "Invalid color definition: %s, setting to black (#000000)"
#define msg_LatexArrow_STR "Latex arrow geometry: allow only LaTeX arrow slopes"
#define msg_LatexLine_STR "Latex line geometry: allow only LaTeX line slopes"
#define msg_LayerErr_STR "Could not create layer"
#define msg_LayerInfoErr_STR "Could not create LayerInfo"
#define msg_Left_STR "Left"
#define msg_Manhattan_STR "Manhattan geometry (for polyline and spline)"
#define msg_MenuAbout_STR "About"
#define msg_MenuAsPNG_STR "PNG"
#define msg_MenuClear_STR "Clear"
#define msg_MenuColor_STR "Color Window"
#define msg_MenuEdit_STR "Edit"
#define msg_MenuEditSettings_STR "Edit"
#define msg_MenuExport_STR "Export"
#define msg_MenuImport_STR "Import"
#define msg_MenuInsert_STR "Insert"
#define msg_MenuNew_STR "New"
#define msg_MenuOpen_STR "Open"
#define msg_MenuOpenLibWnd_STR "Library"
#define msg_MenuPrint_STR "Print"
#define msg_MenuPrintPS_STR "Print PS"
#define msg_MenuProject_STR "Project"
#define msg_MenuQuit_STR "Quit"
#define msg_MenuR2V_STR "Raster to Vector"
#define msg_MenuRedisplay_STR "Redisplay"
#define msg_MenuSandQ_STR "Save & Quit"
#define msg_MenuSave_STR "Save"
#define msg_MenuSaveAs_STR "Save As"
#define msg_MenuSettings_STR "Settings"
#define msg_MenuStatus_STR "Status"
#define msg_MenuUndo_STR "Undo"
#define msg_MenuViaF2D_STR "via Fig2Dev"
#define msg_MenuZoom_STR "Zoom Window"
#define msg_Mountain_STR "Mountain geometry (for polyline and spline)"
#define msg_MountHattan_STR "Mount-hattan geometry (for polyline and spline)"
#define msg_moveobjects_STR "Move objects"
#define msg_movepoints_STR "Move points (of polyline, polygon, closed-spline, spline and box)"
#define msg_NegCoordShift_STR "The figure has objects which have negative coordinates,\ndo you wish to shift it back on the page?"
#define msg_NoAlignSpec_STR "No Center/Flushleft specification"
#define msg_NoFigFile_STR "Not a Fig file"
#define msg_NoFile_STR "No file"
#define msg_NoMagnSpec_STR "No Magnification specification"
#define msg_NoMemForBM_STR "Not enough memory for bitmap allocation"
#define msg_NoMultiPageSpec_STR "No multiple page flag specification"
#define msg_NoOrientSpec_STR "No portrait/landscape specification"
#define msg_NoPaperSizeSpec_STR "No paper size specification"
#define msg_NoResolutionSpec_STR "No resolution specification"
#define msg_NothingToSave_STR "No figure to save. Abort save operation"
#define msg_NoTranspColorSpec_STR "No transparent color specification"
#define msg_NoUndo_STR "Nothing to undo"
#define msg_NoUnitSpec_STR "No metric/inches specification"
#define msg_ObjectsInFile_STR "File \"%s\" %d objects"
#define msg_ObjectsInModFile_STR "Modified File \"%s\" %d objects"
#define msg_ObjectsSavedIn_STR "%d object(s) saved in \"%s\""
#define msg_ObjectUpdated_STR "Object(s) updated"
#define msg_OffScreenRendering_STR "Off screen rendering"
#define msg_OK_STR "OK"
#define msg_Only90degRotation_STR "This object can't be rotated by angles other than 90 degrees"
#define msg_OpenFileErr_STR "Couldn't open file %s"
#define msg_OpenScreenErr_STR "Failed to open screen"
#define msg_OutOfMem_STR "Running out of memory"
#define msg_PaintWndErr_STR "Could not open canvas window"
#define msg_Picture_STR "Picture object"
#define msg_PointAngle_STR "%d point%s, Angle = %.1f deg"
#define msg_Polygon_STR "Polygon drawing"
#define msg_PolygonMinPoints_STR "A polygon cannot have less than 3 points"
#define msg_PolyLine_STR "Polyline drawing"
#define msg_PreferredScreenErr_STR "Couldn't open preferred screen\nor preferred screen not configured\n\ncloning WB screen instead"
#define msg_PrefsDrawInBackDrop_STR "Draw in Backdrop"
#define msg_PrefsUsePubScr_STR "Use public screen"
#define msg_PRINT_STR "Print"
#define msg_PrintBMErr_STR "Could not allocate bitmap for printing!\nTry to free some  memory or decrease buffer size"
#define msg_PrinterDeviceErr_STR "Printing failed\nAnother program ist using the printer device"
#define msg_PrinterErr_STR "Printer returned error code %d"
#define msg_PrinterPrefs_STR "Printer Prefs"
#define msg_PrintingProgress_STR "Printing bitmap : part %d of %d"
#define msg_PrintNoFigure_STR "No figure to print"
#define msg_PrintSel_STR "Selection"
#define msg_PublicScreenErr_STR "Could not open on public screen"
#define msg_RAS2VECNotConfigured_STR "Converting Bitmap to Vector format is not configured"
#define msg_RastportInitErr_STR "Could not init rastport!"
#define msg_ReadingFrom_STR "Reading objects from \"%s\""
#define msg_ReadPicFile_STR "Reading picture file"
#define msg_RectBox_STR "Rectangular BOX drawing"
#define msg_RotAngle_STR "Angle"
#define msg_RotateObjectCCW_STR "Rotate objects (left button) or copy & rotate (right button) 90 degree"
#define msg_RotateObjectCW_STR "Rotate objects (left button) or copy & rotate (right button) -90 degree"
#define msg_SavingBitmap_STR "Saving bitmap to file \"%s\""
#define msg_Spline_STR "Spline drawing: specify control points"
#define msg_SplineMinPoint_STR "A spline cannot have less than %d points"
#define msg_TextInput_STR "Text input (from keyboard)"
#define msg_TextJustErr_STR "Invalid text justification at line %d, setting to left."
#define msg_TextStringParsingErr_STR "Error in parsing text string on line."
#define msg_TooManyEllPoint_STR "Too many points to fully display rotated ellipse. %d points max"
#define msg_TooMuchUserColors_STR "Can't allocate more than %d user colors, not enough colormap entries"
#define msg_Top_STR "Top"
#define msg_TruncateText_STR "Truncating text object to %d chars in line %d"
#define msg_Uncontraint_STR "Unconstraint geometry (for polyline and spline)"
#define msg_UndoDone_STR "Undo complete"
#define msg_UndoFigureRestore_STR "Immediate Undo will restore the figure"
#define msg_UpdateAttributes_STR "Update objects to current settings"
#define msg_VersionNOK_STR "You must have a newer version of AmiFIG to load this figure (%ld)"
#define msg_Width_STR "Width"
#define msg_WriteFileErr_STR "Error writing file %s"
#define msg_Writing_STR "Writing in progress"
#define msg_WrongColorDefPosition_STR "Color definitions must come before other objects (line %d)"
#define msg_WrongRotationAngle_STR "Invalid rotation angle for this compound object"
#define msg_1stAngle_STR "1st angle = %.2f degrees"
#define msg_AA_STR "Antialiasing"
#define msg_AbutHorError_STR "Can't ABUT horizontally with respect to the canvas"
#define msg_AbutVerError_STR "Can't ABUT vertically with respect to the canvas"
#define msg_AccesDeniedError_STR "Error: access denied to cut file"
#define msg_AddLibrary_STR "Add library"
#define msg_AddNewLibraryObjectsToList_STR "Add new library objects to list"
#define msg_AddOneColorStop_STR "Add one color stop"
#define msg_AddStop_STR "Add stop"
#define msg_AddTangentOrNormal_STR "Add tangent or normal to object"
#define msg_All_STR "All"
#define msg_AllowNegCoords_STR "Allow negative coordinates"
#define msg_RadOrDimAndAngle_STR "%s = %s, Angle = %.1f deg"
#define msg_AngleEndPtError_STR "Can't compute angle at endpoint"
#define msg_AngleError_STR "Can't compute angle"
#define msg_ArcBoxAreaError_STR "Arc box must have area"
#define msg_ArcBoxSelected_STR "Arcbox drawing selected"
#define msg_ArcChopped_STR "Arc chopped into %d pieces"
#define msg_ArcDoesntIntersect_STR "Arc does not intersect with any selected axe elements"
#define msg_ArcType_STR "Arc type"
#define msg_AreaOfIs_STR "Area of %s is %s"
#define msg_AreaOfIsAcc_STR "Area of %s is %s, accumulated %s"
#define msg_AreaError_STR "Sorry, can't measure area of this object"
#define msg_AreaReset_STR "area reset to 0"
#define msg_Arrow_STR "Arrow"
#define msg_AutoactivateWin_STR "Auto activate window"
#define msg_AutoChopError_STR "An axe cannot chop itself"
#define msg_AutoloadLibrary_STR "Autoload library"
#define msg_AutomaticEndArrow_STR "Automatic end arrow"
#define msg_AutomaticStartArrow_STR "Automatic start arrow"
#define msg_AutoScroll_STR "Autoscroll canvas"
#define msg_AxeListCleared_STR "Axe object list cleared"
#define msg_AxeObjSelected_STR "Axe object %d selected"
#define msg_Bevel_STR "Bevel"
#define msg_Boring_STR "Boring"
#define msg_BoxAreaError_STR "Box must have area"
#define msg_BoxColor_STR "Box color"
#define msg_BoxThickness_STR "Box thickness"
#define msg_Butt_STR "Butt"
#define msg_Cancel_STR "Cancel"
#define msg_Cap_STR "Cap"
#define msg_CapStyle_STR "Cap style"
#define msg_Center_STR "Center"
#define msg_CenterPtIgnored_STR "Center point selected, ignored"
#define msg_ChangeDispFont_STR "Change display fonts"
#define msg_ChopObject_STR "Chop object"
#define msg_ChopOnlyUnconstPolyX_STR "Only unconstrained polylines and polygons may be chopped"
#define msg_CircleNoEndPt_STR "Circles have no endpoints"
#define msg_ClickToChange_STR "Click to change"
#define msg_Close_STR "Close"
#define msg_CloseCompound_STR "Close compound"
#define msg_Closed_STR "Closed"
#define msg_ClosedFigReq2intersects_STR "Closed figures require two or more intersects"
#define msg_ClosedSpline3ptsError_STR "Closed splines must have 3 or more points, removing spline at line %d"
#define msg_ClosestPtNotOnArc_STR "The closest point found is not on the arc"
#define msg_Color_STR "Color"
#define msg_Command_STR "Command"
#define msg_ConstMoveCircleError_STR "Constrained move not supported for circles"
#define msg_Copy_STR "Copy"
#define msg_CopyNewGradient_STR "Copy to new gradient"
#define msg_CouldntOpenCutFile_STR "Error: couldn't open cut file %s"
#define msg_CreateIcon_STR "Create icon"
#define msg_CreateNewGradient_STR "Create a new gradient"
#define msg_CutFileReadOnly_STR "Error: cut file is read only"
#define msg_Dash1Dot_STR "Dash 1 Dot"
#define msg_Dash2Dot_STR "Dash 2 Dots"
#define msg_Dash3Dot_STR "Dash 3 Dots"
#define msg_Dashed_STR "Dashed"
#define msg_DashLength_STR "Dash Length"
#define msg_Debug_STR "Debug"
#define msg_DefaultTool_STR "Default tool"
#define msg_DefineSystemFonts_STR "Define corresponding system fonts"
#define msg_Degrees_STR "%s%.2f Degrees"
#define msg_Delete_STR "Delete"
#define msg_DeleteGradient_STR "Delete gradient"
#define msg_DeletePolygon3pts_STR "Deleting polygon containing fewer than 3 points at line %d"
#define msg_DeleteZeroSizeObject_STR "Deleting zero-size %s at line %d"
#define msg_Density_STR "Density"
#define msg_Depth_STR "Depth"
#define msg_Dim_STR "Dim."
#define msg_DimensionLine_STR "Dimension line: %s"
#define msg_DiscardModifications_STR "Discard modifications"
#define msg_Display_STR "Display"
#define msg_DisplayUnit_STR "Display Unit"
#define msg_DistHorError_STR "Can't distribute horizontally with respect to the canvas"
#define msg_DistVerError_STR "Can't distribute vertically with respect to the canvas"
#define msg_Dotted_STR "Dotted"
#define msg_DuplicateAxeError_STR "Duplicate axe object selected"
#define msg_EditColors_STR "Edit colors"
#define msg_EllipseAngle_STR "Ellipse angle"
#define msg_EllipseArcNotSupported_STR "Elliptical arcs not (yet) supported"
#define msg_EllipseChopError_STR "Ellipse does not intersect with any selected axe elements"
#define msg_EllipseChopped_STR "Ellipse chopped into %d pieces"
#define msg_EllipseSplineIntersetNotImpl_STR "Ellipse-spline intersections not yet implemented"
#define msg_EmptyCompoundIgnored_STR "Empty compound, ignored"
#define msg_EmptyRegionUnchanged_STR "Empty region, figure unchanged"
#define msg_EnterPictName_STR "Please enter name of Picture Object file in EDIT window"
#define msg_Export_STR "Export"
#define msg_F2Dexport_STR "Fig2dev export"
#define msg_Figure_STR "Figure"
#define msg_FileHasNoVersionNumber_STR "File \"%s\" has no version number in header"
#define msg_FileIsADirectory_STR "File \"%s\" is a directory"
#define msg_FileIsEmpty_STR "File \"%s\" is empty"
#define msg_FileIsNotAccessible_STR "File \"%s\" is not accessable; %s"
#define msg_FilenameIsTooLong_STR "File name \"%s\" is too long"
#define msg_FileNotExisting_STR "File \"%s\" does not exist"
#define msg_Fill_STR "Fill"
#define msg_FillColor_STR "Fill color"
#define msg_FillStyle_STR "Fill style"
#define msg_FillType_STR "Fill type"
#define msg_FixedFocalPt_STR "Fixed focal point"
#define msg_FixedText_STR "Fixed text"
#define msg_Flags_STR "Flags"
#define msg_Fonts_STR "Fonts"
#define msg_Format_STR "Format"
#define msg_Fractionnal_STR "Fractionnal"
#define msg_FreehandRes_STR "Freehand resolution"
#define msg_Front_Back_STR "Bring to front / send to back"
#define msg_GapLength_STR "Gap length"
#define msg_Gradient_STR "Gradient"
#define msg_Grid_STR "Grid"
#define msg_GridUnit_STR "Grid unit"
#define msg_HeightPercent_STR "Height (%)"
#define msg_HollowNice_STR "Hollow nice"
#define msg_HollowSpear_STR "Hollow spear"
#define msg_HollowTriangle_STR "Hollow triangle"
#define msg_HQDisplay_STR "High quality display"
#define msg_Imperial_STR "Imperial"
#define msg_InputOption_STR "Input option"
#define msg_InsertFile_STR "Insert file"
#define msg_InvalidArcGeometry_STR "Invalid arc geometry"
#define msg_InvalidTextFont_STR "Invalid text font (%d) at line %d, setting to default"
#define msg_IO_STR "I/O"
#define msg_IsaDirectoryError_STR "Error: \"%s\" is a directory"
#define msg_Join_STR "Join"
#define msg_JoinStyle_STR "Join style"
#define msg_Justification_STR "Justification"
#define msg_Landscape_STR "landscape"
#define msg_LatexArrows_STR "Latex arrows"
#define msg_LatexLines_STR "Latex lines"
#define msg_LeftArrow_STR "Left arrow"
#define msg_LengthMeas_STR "%d point%s, Length = %s, dx = %s, dy = %s (%.1f deg)"
#define msg_LengthError_STR "Sorry, can't measure length of this object"
#define msg_LengthOfIs_STR "Length of %s is %s"
#define msg_LengthOfIsAcc_STR "Length of %s is %s, accumulated %s"
#define msg_LengthReset_STR "length reset to 0"
#define msg_Line_STR "Line"
#define msg_Linear_STR "Linear"
#define msg_LineStyle_STR "Line style"
#define msg_LineWidth_STR "Line width"
#define msg_CreateGradient_STR "Create a gradient between 2 colors"
#define msg_MeasAngle_STR "Measure angle"
#define msg_MeasArea_STR "Measure area"
#define msg_MeasLength_STR "Measure length"
#define msg_Metric_STR "Metric"
#define msg_Misc_STR "Misc"
#define msg_Miter_STR "Miter"
#define msg_New_STR "New"
#define msg_NoFill_STR "No fill"
#define msg_NoIntersection_STR "No intersection found"
#define msg_None_STR "None"
#define msg_NoPoint_STR "No point found"
#define msg_NoPriorPtForDiameter_STR "No prior point from which to create a diameter"
#define msg_NoPriorPtForNormal_STR "No prior point from which to create a normal"
#define msg_NoPriorPtForTangent_STR "No prior point from which to create a tangent"
#define msg_NormalAdded_STR "added normal"
#define msg_NormalImpossible_STR "Normals can only be computed for polylines, boxes, and polygons"
#define msg_NormalPtNotOnArc_STR "The normal point found is not on the arc"
#define msg_NotADirectory_STR "A name in the path \"%s\" is not a directory"
#define msg_NotEnoughPtForOpenSpline_STR "Open splines must have 2 or more points, removing spline at line %d"
#define msg_NotEnoughPtForPolygon_STR "Not enough points for a polygon"
#define msg_NotEnoughPtForSpline_STR "Not enough points for a spline"
#define msg_Object_STR "Object"
#define msg_ObjectCopiedToScrapFile_STR "Object copied to scrapfile %s"
#define msg_ObjectDeletedFromScrapFile_STR "Object deleted to scrapfile %s"
#define msg_OnScreenAA_STR "Antialiased display"
#define msg_Open_STR "Open"
#define msg_OpenColorWindow_STR "Open user color & gradient edit window"
#define msg_OpenFile_STR "Open file"
#define msg_OpenPrefFile_STR "Open preference file"
#define msg_Orientation_STR "Orientation"
#define msg_OutputOption_STR "Output option"
#define msg_Pad_STR "Pad"
#define msg_Page_STR "Page"
#define msg_Pattern_STR "Pattern"
#define msg_PenColor_STR "Pen color"
#define msg_PictureFile_STR "Picture file"
#define msg_PictureSelection_STR "Please select a picture file"
#define msg_PictureSize_STR "Picture size"
#define msg_PlaceLibObj_STR "Placing library object %s.fig"
#define msg_Plain_STR "Plain"
#define msg_PlainNice_STR "Plain Nice"
#define msg_PlainSpear_STR "Plain Spear"
#define msg_PlainTriangle_STR "Plain Triangle"
#define msg_PolylineChopError_STR "Polyline does not intersect with any selected axe elements"
#define msg_PolylineChopped_STR "Polyline chopped into %d pieces"
#define msg_PolylineSplineIntersectNotImpl_STR "Polyline-spline intersections not yet implemented"
#define msg_PolylineTangentError_STR "Polylines have no tangents"
#define msg_Portrait_STR "portrait"
#define msg_Precision_STR "Precision"
#define msg_PrintBuffer_STR "Print buffer (Kb)"
#define msg_Project_STR "Project"
#define msg_Proportional_STR "Proportional"
#define msg_PubScreen_STR "PubScreen"
#define msg_R2VImport_STR "Ras2vec import"
#define msg_Radial_STR "Radial"
#define msg_Radius_STR "Radius"
#define msg_Radiuses_STR "Radius1 = %s, Radius2 = %s"
#define msg_ReadAccessBlocked_STR "Read access to file \"%s\" is blocked"
#define msg_RegpolySelected_STR "Regular Polygon drawing selected"
#define msg_RemoveLastColorStop_STR "Remove last color stop"
#define msg_RemovePtPictError_STR "Can't remove points from picture"
#define msg_RemoveStop_STR "Remove Stop"
#define msg_Repeat_STR "Repeat"
#define msg_Reset_STR "Reset"
#define msg_Right_STR "Right"
#define msg_RightArrow_STR "Right arrow"
#define msg_RigidText_STR "Rigid text"
#define msg_Round_STR "Round"
#define msg_Save_STR "Save"
#define msg_SaveFile_STR "Save file as"
#define msg_SavePalette_STR "Save palette"
#define msg_SavePrefFile_STR "Save preference file"
#define msg_ScaleSelected_STR "Scale selected"
#define msg_Scaling_STR "Scaling"
#define msg_ScreenMode_STR "Screen mode"
#define msg_ScreenModeSelect_STR "Please select a screen mode"
#define msg_ScreenOption_STR "Screen option"
#define msg_Select2ndPt_STR "Select second object"
#define msg_SelectAllFields_STR "Select all fields"
#define msg_SelectDirectory_STR "Select a directory"
#define msg_SelectedCirclesAreCongruent_STR "Selected circles/arcs are congruent"
#define msg_SelectedEllAndPolylineNoInter_STR "Selected ellipse and polyline do not intersect"
#define msg_SelectedPolylineNoIntersect_STR "Selected polylines do not intersect"
#define msg_SelectPattern_STR "Select pattern"
#define msg_SelectSecondColor_STR "Select second color"
#define msg_SetOriginalPicSize_STR "Set original picture size"
#define msg_SFactorEdit_STR "Edit S-Factors at spline points"
#define msg_SFactorValue_STR "S-Factor = %1.3f"
#define msg_ShowLength_STR "Show length"
#define msg_ShowNums_STR "Show nums"
#define msg_SideRuler_STR "Side ruler"
#define msg_SignedAreaReset_STR "signed area reset to 0"
#define msg_SingularityError_STR "singularity, can't draw tangent/normal"
#define msg_Size_STR "Size"
#define msg_Skin_STR "Skin"
#define msg_SkinSelect_STR "Please select a skin"
#define msg_Snap_STR "Snap"
#define msg_SoftScroll_STR "Softscroll canvas"
#define msg_Solid_STR "Solid"
#define msg_SpecialText_STR "Special text"
#define msg_SplineArcInterNotImpl_STR "Spline-arc intersections not yet implemented"
#define msg_SplineDiaNotImpl_STR "Spline diameters not yet implemented"
#define msg_SplineEndPtNotImpl_STR "Spline endpoints not yet implemented"
#define msg_SplineMidPtNotImpl_STR "Spline midpoints not yet implemented"
#define msg_SplineOnePtCutError_STR "Cutting there would result in a spline with only one point"
#define msg_SplineSnapNotImpl_STR "Spline snap nearest not yet implemented"
#define msg_SplineSplineInterNotImpl_STR "Spline-spline intersections not yet implemented"
#define msg_SplineTangentNotImpl_STR "Spline tangents not yet implemented"
#define msg_SplineTextInterNotImpl_STR "Spline-text intersections not yet implemented"
#define msg_SplinsNormalNotImpl_STR "Spline normals not yet implemented"
#define msg_StartColor_STR "Start from current color"
#define msg_StatCheckErrorCutFile_STR "Error: cut file didn't pass stat check"
#define msg_StdIO_STR "Standard I/O"
#define msg_StepNb_STR "Step nb"
#define msg_Stroke_STR "Stroke"
#define msg_Style_STR "Style"
#define msg_SVGImport_STR "SVG Import"
#define msg_Symetric_STR "Symetric"
#define msg_TangentAdded_STR "Added tangent"
#define msg_TangentImpossible_STR "No tangent can be drawn from the current point"
#define msg_TangentOnlyForPolylines_STR "Tangents can only be computed for polylines"
#define msg_TangentPtNotOnArc_STR "The tangent point found is not on the arc"
#define msg_Tenth_STR "Tenth"
#define msg_Text_STR "Text"
#define msg_TextAdjust_STR "Text adjust"
#define msg_TextAngle_STR "Text angle"
#define msg_TextBox_STR "Text box"
#define msg_TextColor_STR "Text color"
#define msg_TextFont_STR "Text font"
#define msg_TextRigid_STR "Text rigid"
#define msg_TextSize_STR "Text size"
#define msg_TextSpecial_STR "Text special"
#define msg_Thickness_STR "Thickness"
#define msg_TickThickness_STR "Tick thickness"
#define msg_TopRuler_STR "Top ruler"
#define msg_Unconstrained_STR "Unconstrained"
#define msg_Undo_STR "Undo"
#define msg_UnselectAll_STR "Unselect All fields"
#define msg_Updates_STR "Upd."
#define msg_Use_STR "Use"
#define msg_UsePref_STR "Use preference"
#define msg_UserColors_STR "User color"
#define msg_UserScale_STR "User scale"
#define msg_UseTicks_STR "Use ticks"
#define msg_WidthHeightFactor_STR "Width = %s, Height = %s, Factor = %.3f"
#define msg_WidthLengthFactor_STR "Width = %s, Length = %s, Factor = %.3f"
#define msg_WidthPercent_STR "Width (%)"
#define msg_WriteBackup_STR "Write backup"
#define msg_ZoomFactor_STR "Zoom factor"
#define msg_ZoomIn_STR "Zoom in"
#define msg_ZoomOut_STR "Zoom out"
#define msg_OtherOptions_STR "Other options"
#define msg_SelectShadeTint_STR "Select shade/tint"
#define msg_SelectSVGfile_STR "Select SVG file"
#define msg_LastMsg_STR "Last message"
#define msg_MouseFunction_STR "Mouse buttons functions"
#define msg_BoxScaleMsg1_STR "Can't use box scale on selected object"
#define msg_BoxScaleMsg2_STR "Can't use box scale on selected object; try putting it into a compound"
#define msg_Diameter_STR "Diameter"
#define msg_Length_STR "Length"
#define msg_Distance_STR "Distance"
#define msg_Locate_Object_STR "Locate Object"
#define msg_first_point_STR "first point"
#define msg_next_point_STR "next point"
#define msg_center_point_STR "center point"
#define msg_mid_point_STR "mid point"
#define msg_final_point_STR "final point"
#define msg_corner_point_STR "corner point"
#define msg_delete_point_STR "delete point"
#define msg_direction_STR "direction"
#define msg_Circle_center_STR "Circle center"
#define msg_Ellipse_center_STR "Ellipse center"
#define msg_Ellipse_corner_STR "Ellipse corner"
#define msg_Circle_diameter_STR "Circle diameter"
#define msg_final_corner_STR "final corner"
#define msg_set_radius_STR "set radius"
#define msg_final_angle_STR "final angle"
#define msg_freehand_STR "freehand"
#define msg_single_point_STR "single point"
#define msg_dimension_line_STR "dimension line"
#define msg_position_cursor_STR "position cursor"
#define msg_break_add_here_STR "break/add here"
#define msg_place_new_point_STR "place new point"
#define msg_align_compound_STR "align compound"
#define msg_align_canvas_STR "align canvas"
#define msg_tag_object_STR "tag object"
#define msg_tag_region_STR "tag region"
#define msg_align_tagged_STR "align tagged"
#define msg_add_arrow_STR "add arrow"
#define msg_delete_arrow_STR "delete arrow"
#define msg_break_compound_STR "break compound"
#define msg_break_and_tag_STR "break and tag"
#define msg_Put_Front_STR "Put front"
#define msg_Put_Back_STR "Put back"
#define msg_Select_axe_object_STR "Select axe object"
#define msg_Select_log_object_STR "Select log object"
#define msg_Clear_axe_list_STR "Clear axe list"
#define msg_open_compound_STR "open compound"
#define msg_open_keep_visible_STR "open, keep visible"
#define msg_spline_line_STR "spline<->line"
#define msg_open_closed_STR "open<->closed"
#define msg_copy_object_STR "copy object"
#define msg_horiz_vert_copy_STR "horiz/vert copy"
#define msg_copy_to_cut_buf_STR "copy to cut buf"
#define msg_place_object_STR "place object"
#define msg_array_placement_STR "array placement"
#define msg_delete_object_STR "delete object"
#define msg_delete_region_STR "delete region"
#define msg_del_to_cut_buf_STR "del to cut buf"
#define msg_Modify_SFactor_STR "Modify SFactor"
#define msg_Set_STR "Set"
#define msg_flip_STR "flip"
#define msg_copy_flip_STR "copy & flip"
#define msg_set_anchor_STR "set anchor"
#define msg_unset_anchor_STR "unset anchor"
#define msg_compound_tagged_STR "compound tagged"
#define msg_Join_Lines_Splines_STR "Join Lines/Splines"
#define msg_Split_Line_Spline_STR "Split Line/Spline"
#define msg_Choose_next_Line_STR "Choose next Line"
#define msg_Choose_next_Spline_STR "Choose next Spline"
#define msg_select_save_STR "select & save"
#define msg_select_object_STR "select object"
#define msg_angle_tip_STR "angle tip"
#define msg_save_angle_STR "save angle"
#define msg_select_add_STR "select & add"
#define msg_reset_to_0_STR "reset to 0"
#define msg_move_object_STR "move object"
#define msg_horiz_vert_move_STR "horiz/vert move"
#define msg_move_point_STR "move point"
#define msg_new_posn_STR "new posn"
#define msg_new_object_STR "new object"
#define msg_cancel_library_STR "cancel library"
#define msg_rotate_object_STR "rotate object"
#define msg_copy_rotate_STR "copy & rotate"
#define msg_set_center_STR "set center"
#define msg_unset_center_STR "unset center"
#define msg_scale_box_STR "scale box"
#define msg_scale_about_center_STR "scale about center"
#define msg_add_tangent_STR "add tangent"
#define msg_add_normal_STR "add normal"
#define msg_update_object_STR "update object"
#define msg_Get_settings_STR "Get settings"
#define msg_place_and_edit_STR "place and edit"
#define msg_change_draw_mode_STR "change draw mode"
#define msg_place_at_orig_STR "place at orig posn"
#define msg_HelpWnd_STR "Help Window"
#define msg_SkinChange_STR "Skin can't be changed on the fly, you need to\n1) Save as default settings\n2) Restart the application"
#define msg_SwapColorStop_STR "Swaps 2 color stops"
#define msg_SecondSwapStop_STR "First stop selected (%d), Select second one"
#define msg_SwapDone_STR "Stops %d and %d have been swapped successfully"
#define msg_Swap_STR "Swap"
#define msg_Add_STR "Add"
#define msg_Del_STR "Del"
#define msg_CopyColor_STR "Copy color to new cell"
#define msg_SwapColor_STR "Swap colors from 2 cells"
#define msg_SelectCopyColor_STR "Select cell to copy color to"
#define msg_SelectSwapColor_STR "Select cell to swap color with"
#define msg_ColorPicker_STR "Select color and left click on it"
#define msg_GradDSpread_STR "Gradient Dither Spread"
#define msg_FreeHandMode_STR "Turns on free hand mode for line and spline drawing"
#define msg_AutoSaveSettings_STR "Save settings at exit"
#define msg_OffScreenFailed_STR "failed to allocate off-screen rendering"
#define msg_TooManyPts_STR "Too Many points for curve definition"

#endif /* CATCOMP_STRINGS */

/***************************************************************/

#ifdef CATCOMP_ARRAY

#ifndef HAVE_CATCOMP_ARRAYTYPE
#define HAVE_CATCOMP_ARRAYTYPE
struct CatCompArrayType
{
  IPTR   cca_ID;
  STRPTR cca_Str;
};
#endif

static const struct CatCompArrayType CATCOMPARRAY[] =
{
  {msg_ActionCancelled,(STRPTR)msg_ActionCancelled_STR},
  {msg_AddPoints,(STRPTR)msg_AddPoints_STR},
  {msg_Align,(STRPTR)msg_Align_STR},
  {msg_Antialising,(STRPTR)msg_Antialising_STR},
  {msg_Arc,(STRPTR)msg_Arc_STR},
  {msg_ArrowHead,(STRPTR)msg_ArrowHead_STR},
  {msg_AutoEndArrow,(STRPTR)msg_AutoEndArrow_STR},
  {msg_AutoStartArrow,(STRPTR)msg_AutoStartArrow_STR},
  {msg_BMAllocateErr,(STRPTR)msg_BMAllocateErr_STR},
  {msg_Box2Polygon,(STRPTR)msg_Box2Polygon_STR},
  {msg_CanTMoveCenterPt,(STRPTR)msg_CanTMoveCenterPt_STR},
  {msg_CircleDiameter,(STRPTR)msg_CircleDiameter_STR},
  {msg_CircleRadius,(STRPTR)msg_CircleRadius_STR},
  {msg_ClosedInterpSpline,(STRPTR)msg_ClosedInterpSpline_STR},
  {msg_ClosedSpline,(STRPTR)msg_ClosedSpline_STR},
  {msg_ClosedSplineMinPoints,(STRPTR)msg_ClosedSplineMinPoints_STR},
  {msg_CompoundBreak,(STRPTR)msg_CompoundBreak_STR},
  {msg_CompoundCreate,(STRPTR)msg_CompoundCreate_STR},
  {msg_CompoundGetAttrErr,(STRPTR)msg_CompoundGetAttrErr_STR},
  {msg_CompoundOpen,(STRPTR)msg_CompoundOpen_STR},
  {msg_ConvertLine2Spline,(STRPTR)msg_ConvertLine2Spline_STR},
  {msg_CopyObjects,(STRPTR)msg_CopyObjects_STR},
  {msg_DatatypeError,(STRPTR)msg_DatatypeError_STR},
  {msg_DeleteObjects,(STRPTR)msg_DeleteObjects_STR},
  {msg_DeletePoints,(STRPTR)msg_DeletePoints_STR},
  {msg_DeletingPrevFromMem,(STRPTR)msg_DeletingPrevFromMem_STR},
  {msg_DepthLimits,(STRPTR)msg_DepthLimits_STR},
  {msg_DepthTooBig,(STRPTR)msg_DepthTooBig_STR},
  {msg_DepthTooSmall,(STRPTR)msg_DepthTooSmall_STR},
  {msg_Done,(STRPTR)msg_Done_STR},
  {msg_DotMinPoint,(STRPTR)msg_DotMinPoint_STR},
  {msg_EditObjects,(STRPTR)msg_EditObjects_STR},
  {msg_EllipseDiameter,(STRPTR)msg_EllipseDiameter_STR},
  {msg_EllipseRadius,(STRPTR)msg_EllipseRadius_STR},
  {msg_EmptyName,(STRPTR)msg_EmptyName_STR},
  {msg_ExportAsSel,(STRPTR)msg_ExportAsSel_STR},
  {msg_ExportOutput,(STRPTR)msg_ExportOutput_STR},
  {msg_ExportSel,(STRPTR)msg_ExportSel_STR},
  {msg_ExportWhole,(STRPTR)msg_ExportWhole_STR},
  {msg_FIG2DEVNotConfigured,(STRPTR)msg_FIG2DEVNotConfigured_STR},
  {msg_Fig30Bug,(STRPTR)msg_Fig30Bug_STR},
  {msg_FigModCont,(STRPTR)msg_FigModCont_STR},
  {msg_FigResOrCoordMissing,(STRPTR)msg_FigResOrCoordMissing_STR},
  {msg_FigureDelete,(STRPTR)msg_FigureDelete_STR},
  {msg_FigureFormatConvert,(STRPTR)msg_FigureFormatConvert_STR},
  {msg_FigureScaling,(STRPTR)msg_FigureScaling_STR},
  {msg_FigureShift,(STRPTR)msg_FigureShift_STR},
  {msg_FlipHoriz,(STRPTR)msg_FlipHoriz_STR},
  {msg_FlipVert,(STRPTR)msg_FlipVert_STR},
  {msg_FontNotAvailable,(STRPTR)msg_FontNotAvailable_STR},
  {msg_GetAttributes,(STRPTR)msg_GetAttributes_STR},
  {msg_Height,(STRPTR)msg_Height_STR},
  {msg_IllegalPaperSize,(STRPTR)msg_IllegalPaperSize_STR},
  {msg_IncompleteObject,(STRPTR)msg_IncompleteObject_STR},
  {msg_IncorrectFormatLine,(STRPTR)msg_IncorrectFormatLine_STR},
  {msg_IncorrectObjectCode,(STRPTR)msg_IncorrectObjectCode_STR},
  {msg_InterpSpline,(STRPTR)msg_InterpSpline_STR},
  {msg_InvalidColor,(STRPTR)msg_InvalidColor_STR},
  {msg_LatexArrow,(STRPTR)msg_LatexArrow_STR},
  {msg_LatexLine,(STRPTR)msg_LatexLine_STR},
  {msg_LayerErr,(STRPTR)msg_LayerErr_STR},
  {msg_LayerInfoErr,(STRPTR)msg_LayerInfoErr_STR},
  {msg_Left,(STRPTR)msg_Left_STR},
  {msg_Manhattan,(STRPTR)msg_Manhattan_STR},
  {msg_MenuAbout,(STRPTR)msg_MenuAbout_STR},
  {msg_MenuAsPNG,(STRPTR)msg_MenuAsPNG_STR},
  {msg_MenuClear,(STRPTR)msg_MenuClear_STR},
  {msg_MenuColor,(STRPTR)msg_MenuColor_STR},
  {msg_MenuEdit,(STRPTR)msg_MenuEdit_STR},
  {msg_MenuEditSettings,(STRPTR)msg_MenuEditSettings_STR},
  {msg_MenuExport,(STRPTR)msg_MenuExport_STR},
  {msg_MenuImport,(STRPTR)msg_MenuImport_STR},
  {msg_MenuInsert,(STRPTR)msg_MenuInsert_STR},
  {msg_MenuNew,(STRPTR)msg_MenuNew_STR},
  {msg_MenuOpen,(STRPTR)msg_MenuOpen_STR},
  {msg_MenuOpenLibWnd,(STRPTR)msg_MenuOpenLibWnd_STR},
  {msg_MenuPrint,(STRPTR)msg_MenuPrint_STR},
  {msg_MenuPrintPS,(STRPTR)msg_MenuPrintPS_STR},
  {msg_MenuProject,(STRPTR)msg_MenuProject_STR},
  {msg_MenuQuit,(STRPTR)msg_MenuQuit_STR},
  {msg_MenuR2V,(STRPTR)msg_MenuR2V_STR},
  {msg_MenuRedisplay,(STRPTR)msg_MenuRedisplay_STR},
  {msg_MenuSandQ,(STRPTR)msg_MenuSandQ_STR},
  {msg_MenuSave,(STRPTR)msg_MenuSave_STR},
  {msg_MenuSaveAs,(STRPTR)msg_MenuSaveAs_STR},
  {msg_MenuSettings,(STRPTR)msg_MenuSettings_STR},
  {msg_MenuStatus,(STRPTR)msg_MenuStatus_STR},
  {msg_MenuUndo,(STRPTR)msg_MenuUndo_STR},
  {msg_MenuViaF2D,(STRPTR)msg_MenuViaF2D_STR},
  {msg_MenuZoom,(STRPTR)msg_MenuZoom_STR},
  {msg_Mountain,(STRPTR)msg_Mountain_STR},
  {msg_MountHattan,(STRPTR)msg_MountHattan_STR},
  {msg_moveobjects,(STRPTR)msg_moveobjects_STR},
  {msg_movepoints,(STRPTR)msg_movepoints_STR},
  {msg_NegCoordShift,(STRPTR)msg_NegCoordShift_STR},
  {msg_NoAlignSpec,(STRPTR)msg_NoAlignSpec_STR},
  {msg_NoFigFile,(STRPTR)msg_NoFigFile_STR},
  {msg_NoFile,(STRPTR)msg_NoFile_STR},
  {msg_NoMagnSpec,(STRPTR)msg_NoMagnSpec_STR},
  {msg_NoMemForBM,(STRPTR)msg_NoMemForBM_STR},
  {msg_NoMultiPageSpec,(STRPTR)msg_NoMultiPageSpec_STR},
  {msg_NoOrientSpec,(STRPTR)msg_NoOrientSpec_STR},
  {msg_NoPaperSizeSpec,(STRPTR)msg_NoPaperSizeSpec_STR},
  {msg_NoResolutionSpec,(STRPTR)msg_NoResolutionSpec_STR},
  {msg_NothingToSave,(STRPTR)msg_NothingToSave_STR},
  {msg_NoTranspColorSpec,(STRPTR)msg_NoTranspColorSpec_STR},
  {msg_NoUndo,(STRPTR)msg_NoUndo_STR},
  {msg_NoUnitSpec,(STRPTR)msg_NoUnitSpec_STR},
  {msg_ObjectsInFile,(STRPTR)msg_ObjectsInFile_STR},
  {msg_ObjectsInModFile,(STRPTR)msg_ObjectsInModFile_STR},
  {msg_ObjectsSavedIn,(STRPTR)msg_ObjectsSavedIn_STR},
  {msg_ObjectUpdated,(STRPTR)msg_ObjectUpdated_STR},
  {msg_OffScreenRendering,(STRPTR)msg_OffScreenRendering_STR},
  {msg_OK,(STRPTR)msg_OK_STR},
  {msg_Only90degRotation,(STRPTR)msg_Only90degRotation_STR},
  {msg_OpenFileErr,(STRPTR)msg_OpenFileErr_STR},
  {msg_OpenScreenErr,(STRPTR)msg_OpenScreenErr_STR},
  {msg_OutOfMem,(STRPTR)msg_OutOfMem_STR},
  {msg_PaintWndErr,(STRPTR)msg_PaintWndErr_STR},
  {msg_Picture,(STRPTR)msg_Picture_STR},
  {msg_PointAngle,(STRPTR)msg_PointAngle_STR},
  {msg_Polygon,(STRPTR)msg_Polygon_STR},
  {msg_PolygonMinPoints,(STRPTR)msg_PolygonMinPoints_STR},
  {msg_PolyLine,(STRPTR)msg_PolyLine_STR},
  {msg_PreferredScreenErr,(STRPTR)msg_PreferredScreenErr_STR},
  {msg_PrefsDrawInBackDrop,(STRPTR)msg_PrefsDrawInBackDrop_STR},
  {msg_PrefsUsePubScr,(STRPTR)msg_PrefsUsePubScr_STR},
  {msg_PRINT,(STRPTR)msg_PRINT_STR},
  {msg_PrintBMErr,(STRPTR)msg_PrintBMErr_STR},
  {msg_PrinterDeviceErr,(STRPTR)msg_PrinterDeviceErr_STR},
  {msg_PrinterErr,(STRPTR)msg_PrinterErr_STR},
  {msg_PrinterPrefs,(STRPTR)msg_PrinterPrefs_STR},
  {msg_PrintingProgress,(STRPTR)msg_PrintingProgress_STR},
  {msg_PrintNoFigure,(STRPTR)msg_PrintNoFigure_STR},
  {msg_PrintSel,(STRPTR)msg_PrintSel_STR},
  {msg_PublicScreenErr,(STRPTR)msg_PublicScreenErr_STR},
  {msg_RAS2VECNotConfigured,(STRPTR)msg_RAS2VECNotConfigured_STR},
  {msg_RastportInitErr,(STRPTR)msg_RastportInitErr_STR},
  {msg_ReadingFrom,(STRPTR)msg_ReadingFrom_STR},
  {msg_ReadPicFile,(STRPTR)msg_ReadPicFile_STR},
  {msg_RectBox,(STRPTR)msg_RectBox_STR},
  {msg_RotAngle,(STRPTR)msg_RotAngle_STR},
  {msg_RotateObjectCCW,(STRPTR)msg_RotateObjectCCW_STR},
  {msg_RotateObjectCW,(STRPTR)msg_RotateObjectCW_STR},
  {msg_SavingBitmap,(STRPTR)msg_SavingBitmap_STR},
  {msg_Spline,(STRPTR)msg_Spline_STR},
  {msg_SplineMinPoint,(STRPTR)msg_SplineMinPoint_STR},
  {msg_TextInput,(STRPTR)msg_TextInput_STR},
  {msg_TextJustErr,(STRPTR)msg_TextJustErr_STR},
  {msg_TextStringParsingErr,(STRPTR)msg_TextStringParsingErr_STR},
  {msg_TooManyEllPoint,(STRPTR)msg_TooManyEllPoint_STR},
  {msg_TooMuchUserColors,(STRPTR)msg_TooMuchUserColors_STR},
  {msg_Top,(STRPTR)msg_Top_STR},
  {msg_TruncateText,(STRPTR)msg_TruncateText_STR},
  {msg_Uncontraint,(STRPTR)msg_Uncontraint_STR},
  {msg_UndoDone,(STRPTR)msg_UndoDone_STR},
  {msg_UndoFigureRestore,(STRPTR)msg_UndoFigureRestore_STR},
  {msg_UpdateAttributes,(STRPTR)msg_UpdateAttributes_STR},
  {msg_VersionNOK,(STRPTR)msg_VersionNOK_STR},
  {msg_Width,(STRPTR)msg_Width_STR},
  {msg_WriteFileErr,(STRPTR)msg_WriteFileErr_STR},
  {msg_Writing,(STRPTR)msg_Writing_STR},
  {msg_WrongColorDefPosition,(STRPTR)msg_WrongColorDefPosition_STR},
  {msg_WrongRotationAngle,(STRPTR)msg_WrongRotationAngle_STR},
  {msg_1stAngle,(STRPTR)msg_1stAngle_STR},
  {msg_AA,(STRPTR)msg_AA_STR},
  {msg_AbutHorError,(STRPTR)msg_AbutHorError_STR},
  {msg_AbutVerError,(STRPTR)msg_AbutVerError_STR},
  {msg_AccesDeniedError,(STRPTR)msg_AccesDeniedError_STR},
  {msg_AddLibrary,(STRPTR)msg_AddLibrary_STR},
  {msg_AddNewLibraryObjectsToList,(STRPTR)msg_AddNewLibraryObjectsToList_STR},
  {msg_AddOneColorStop,(STRPTR)msg_AddOneColorStop_STR},
  {msg_AddStop,(STRPTR)msg_AddStop_STR},
  {msg_AddTangentOrNormal,(STRPTR)msg_AddTangentOrNormal_STR},
  {msg_All,(STRPTR)msg_All_STR},
  {msg_AllowNegCoords,(STRPTR)msg_AllowNegCoords_STR},
  {msg_RadOrDimAndAngle,(STRPTR)msg_RadOrDimAndAngle_STR},
  {msg_AngleEndPtError,(STRPTR)msg_AngleEndPtError_STR},
  {msg_AngleError,(STRPTR)msg_AngleError_STR},
  {msg_ArcBoxAreaError,(STRPTR)msg_ArcBoxAreaError_STR},
  {msg_ArcBoxSelected,(STRPTR)msg_ArcBoxSelected_STR},
  {msg_ArcChopped,(STRPTR)msg_ArcChopped_STR},
  {msg_ArcDoesntIntersect,(STRPTR)msg_ArcDoesntIntersect_STR},
  {msg_ArcType,(STRPTR)msg_ArcType_STR},
  {msg_AreaOfIs,(STRPTR)msg_AreaOfIs_STR},
  {msg_AreaOfIsAcc,(STRPTR)msg_AreaOfIsAcc_STR},
  {msg_AreaError,(STRPTR)msg_AreaError_STR},
  {msg_AreaReset,(STRPTR)msg_AreaReset_STR},
  {msg_Arrow,(STRPTR)msg_Arrow_STR},
  {msg_AutoactivateWin,(STRPTR)msg_AutoactivateWin_STR},
  {msg_AutoChopError,(STRPTR)msg_AutoChopError_STR},
  {msg_AutoloadLibrary,(STRPTR)msg_AutoloadLibrary_STR},
  {msg_AutomaticEndArrow,(STRPTR)msg_AutomaticEndArrow_STR},
  {msg_AutomaticStartArrow,(STRPTR)msg_AutomaticStartArrow_STR},
  {msg_AutoScroll,(STRPTR)msg_AutoScroll_STR},
  {msg_AxeListCleared,(STRPTR)msg_AxeListCleared_STR},
  {msg_AxeObjSelected,(STRPTR)msg_AxeObjSelected_STR},
  {msg_Bevel,(STRPTR)msg_Bevel_STR},
  {msg_Boring,(STRPTR)msg_Boring_STR},
  {msg_BoxAreaError,(STRPTR)msg_BoxAreaError_STR},
  {msg_BoxColor,(STRPTR)msg_BoxColor_STR},
  {msg_BoxThickness,(STRPTR)msg_BoxThickness_STR},
  {msg_Butt,(STRPTR)msg_Butt_STR},
  {msg_Cancel,(STRPTR)msg_Cancel_STR},
  {msg_Cap,(STRPTR)msg_Cap_STR},
  {msg_CapStyle,(STRPTR)msg_CapStyle_STR},
  {msg_Center,(STRPTR)msg_Center_STR},
  {msg_CenterPtIgnored,(STRPTR)msg_CenterPtIgnored_STR},
  {msg_ChangeDispFont,(STRPTR)msg_ChangeDispFont_STR},
  {msg_ChopObject,(STRPTR)msg_ChopObject_STR},
  {msg_ChopOnlyUnconstPolyX,(STRPTR)msg_ChopOnlyUnconstPolyX_STR},
  {msg_CircleNoEndPt,(STRPTR)msg_CircleNoEndPt_STR},
  {msg_ClickToChange,(STRPTR)msg_ClickToChange_STR},
  {msg_Close,(STRPTR)msg_Close_STR},
  {msg_CloseCompound,(STRPTR)msg_CloseCompound_STR},
  {msg_Closed,(STRPTR)msg_Closed_STR},
  {msg_ClosedFigReq2intersects,(STRPTR)msg_ClosedFigReq2intersects_STR},
  {msg_ClosedSpline3ptsError,(STRPTR)msg_ClosedSpline3ptsError_STR},
  {msg_ClosestPtNotOnArc,(STRPTR)msg_ClosestPtNotOnArc_STR},
  {msg_Color,(STRPTR)msg_Color_STR},
  {msg_Command,(STRPTR)msg_Command_STR},
  {msg_ConstMoveCircleError,(STRPTR)msg_ConstMoveCircleError_STR},
  {msg_Copy,(STRPTR)msg_Copy_STR},
  {msg_CopyNewGradient,(STRPTR)msg_CopyNewGradient_STR},
  {msg_CouldntOpenCutFile,(STRPTR)msg_CouldntOpenCutFile_STR},
  {msg_CreateIcon,(STRPTR)msg_CreateIcon_STR},
  {msg_CreateNewGradient,(STRPTR)msg_CreateNewGradient_STR},
  {msg_CutFileReadOnly,(STRPTR)msg_CutFileReadOnly_STR},
  {msg_Dash1Dot,(STRPTR)msg_Dash1Dot_STR},
  {msg_Dash2Dot,(STRPTR)msg_Dash2Dot_STR},
  {msg_Dash3Dot,(STRPTR)msg_Dash3Dot_STR},
  {msg_Dashed,(STRPTR)msg_Dashed_STR},
  {msg_DashLength,(STRPTR)msg_DashLength_STR},
  {msg_Debug,(STRPTR)msg_Debug_STR},
  {msg_DefaultTool,(STRPTR)msg_DefaultTool_STR},
  {msg_DefineSystemFonts,(STRPTR)msg_DefineSystemFonts_STR},
  {msg_Degrees,(STRPTR)msg_Degrees_STR},
  {msg_Delete,(STRPTR)msg_Delete_STR},
  {msg_DeleteGradient,(STRPTR)msg_DeleteGradient_STR},
  {msg_DeletePolygon3pts,(STRPTR)msg_DeletePolygon3pts_STR},
  {msg_DeleteZeroSizeObject,(STRPTR)msg_DeleteZeroSizeObject_STR},
  {msg_Density,(STRPTR)msg_Density_STR},
  {msg_Depth,(STRPTR)msg_Depth_STR},
  {msg_Dim,(STRPTR)msg_Dim_STR},
  {msg_DimensionLine,(STRPTR)msg_DimensionLine_STR},
  {msg_DiscardModifications,(STRPTR)msg_DiscardModifications_STR},
  {msg_Display,(STRPTR)msg_Display_STR},
  {msg_DisplayUnit,(STRPTR)msg_DisplayUnit_STR},
  {msg_DistHorError,(STRPTR)msg_DistHorError_STR},
  {msg_DistVerError,(STRPTR)msg_DistVerError_STR},
  {msg_Dotted,(STRPTR)msg_Dotted_STR},
  {msg_DuplicateAxeError,(STRPTR)msg_DuplicateAxeError_STR},
  {msg_EditColors,(STRPTR)msg_EditColors_STR},
  {msg_EllipseAngle,(STRPTR)msg_EllipseAngle_STR},
  {msg_EllipseArcNotSupported,(STRPTR)msg_EllipseArcNotSupported_STR},
  {msg_EllipseChopError,(STRPTR)msg_EllipseChopError_STR},
  {msg_EllipseChopped,(STRPTR)msg_EllipseChopped_STR},
  {msg_EllipseSplineIntersetNotImpl,(STRPTR)msg_EllipseSplineIntersetNotImpl_STR},
  {msg_EmptyCompoundIgnored,(STRPTR)msg_EmptyCompoundIgnored_STR},
  {msg_EmptyRegionUnchanged,(STRPTR)msg_EmptyRegionUnchanged_STR},
  {msg_EnterPictName,(STRPTR)msg_EnterPictName_STR},
  {msg_Export,(STRPTR)msg_Export_STR},
  {msg_F2Dexport,(STRPTR)msg_F2Dexport_STR},
  {msg_Figure,(STRPTR)msg_Figure_STR},
  {msg_FileHasNoVersionNumber,(STRPTR)msg_FileHasNoVersionNumber_STR},
  {msg_FileIsADirectory,(STRPTR)msg_FileIsADirectory_STR},
  {msg_FileIsEmpty,(STRPTR)msg_FileIsEmpty_STR},
  {msg_FileIsNotAccessible,(STRPTR)msg_FileIsNotAccessible_STR},
  {msg_FilenameIsTooLong,(STRPTR)msg_FilenameIsTooLong_STR},
  {msg_FileNotExisting,(STRPTR)msg_FileNotExisting_STR},
  {msg_Fill,(STRPTR)msg_Fill_STR},
  {msg_FillColor,(STRPTR)msg_FillColor_STR},
  {msg_FillStyle,(STRPTR)msg_FillStyle_STR},
  {msg_FillType,(STRPTR)msg_FillType_STR},
  {msg_FixedFocalPt,(STRPTR)msg_FixedFocalPt_STR},
  {msg_FixedText,(STRPTR)msg_FixedText_STR},
  {msg_Flags,(STRPTR)msg_Flags_STR},
  {msg_Fonts,(STRPTR)msg_Fonts_STR},
  {msg_Format,(STRPTR)msg_Format_STR},
  {msg_Fractionnal,(STRPTR)msg_Fractionnal_STR},
  {msg_FreehandRes,(STRPTR)msg_FreehandRes_STR},
  {msg_Front_Back,(STRPTR)msg_Front_Back_STR},
  {msg_GapLength,(STRPTR)msg_GapLength_STR},
  {msg_Gradient,(STRPTR)msg_Gradient_STR},
  {msg_Grid,(STRPTR)msg_Grid_STR},
  {msg_GridUnit,(STRPTR)msg_GridUnit_STR},
  {msg_HeightPercent,(STRPTR)msg_HeightPercent_STR},
  {msg_HollowNice,(STRPTR)msg_HollowNice_STR},
  {msg_HollowSpear,(STRPTR)msg_HollowSpear_STR},
  {msg_HollowTriangle,(STRPTR)msg_HollowTriangle_STR},
  {msg_HQDisplay,(STRPTR)msg_HQDisplay_STR},
  {msg_Imperial,(STRPTR)msg_Imperial_STR},
  {msg_InputOption,(STRPTR)msg_InputOption_STR},
  {msg_InsertFile,(STRPTR)msg_InsertFile_STR},
  {msg_InvalidArcGeometry,(STRPTR)msg_InvalidArcGeometry_STR},
  {msg_InvalidTextFont,(STRPTR)msg_InvalidTextFont_STR},
  {msg_IO,(STRPTR)msg_IO_STR},
  {msg_IsaDirectoryError,(STRPTR)msg_IsaDirectoryError_STR},
  {msg_Join,(STRPTR)msg_Join_STR},
  {msg_JoinStyle,(STRPTR)msg_JoinStyle_STR},
  {msg_Justification,(STRPTR)msg_Justification_STR},
  {msg_Landscape,(STRPTR)msg_Landscape_STR},
  {msg_LatexArrows,(STRPTR)msg_LatexArrows_STR},
  {msg_LatexLines,(STRPTR)msg_LatexLines_STR},
  {msg_LeftArrow,(STRPTR)msg_LeftArrow_STR},
  {msg_LengthMeas,(STRPTR)msg_LengthMeas_STR},
  {msg_LengthError,(STRPTR)msg_LengthError_STR},
  {msg_LengthOfIs,(STRPTR)msg_LengthOfIs_STR},
  {msg_LengthOfIsAcc,(STRPTR)msg_LengthOfIsAcc_STR},
  {msg_LengthReset,(STRPTR)msg_LengthReset_STR},
  {msg_Line,(STRPTR)msg_Line_STR},
  {msg_Linear,(STRPTR)msg_Linear_STR},
  {msg_LineStyle,(STRPTR)msg_LineStyle_STR},
  {msg_LineWidth,(STRPTR)msg_LineWidth_STR},
  {msg_CreateGradient,(STRPTR)msg_CreateGradient_STR},
  {msg_MeasAngle,(STRPTR)msg_MeasAngle_STR},
  {msg_MeasArea,(STRPTR)msg_MeasArea_STR},
  {msg_MeasLength,(STRPTR)msg_MeasLength_STR},
  {msg_Metric,(STRPTR)msg_Metric_STR},
  {msg_Misc,(STRPTR)msg_Misc_STR},
  {msg_Miter,(STRPTR)msg_Miter_STR},
  {msg_New,(STRPTR)msg_New_STR},
  {msg_NoFill,(STRPTR)msg_NoFill_STR},
  {msg_NoIntersection,(STRPTR)msg_NoIntersection_STR},
  {msg_None,(STRPTR)msg_None_STR},
  {msg_NoPoint,(STRPTR)msg_NoPoint_STR},
  {msg_NoPriorPtForDiameter,(STRPTR)msg_NoPriorPtForDiameter_STR},
  {msg_NoPriorPtForNormal,(STRPTR)msg_NoPriorPtForNormal_STR},
  {msg_NoPriorPtForTangent,(STRPTR)msg_NoPriorPtForTangent_STR},
  {msg_NormalAdded,(STRPTR)msg_NormalAdded_STR},
  {msg_NormalImpossible,(STRPTR)msg_NormalImpossible_STR},
  {msg_NormalPtNotOnArc,(STRPTR)msg_NormalPtNotOnArc_STR},
  {msg_NotADirectory,(STRPTR)msg_NotADirectory_STR},
  {msg_NotEnoughPtForOpenSpline,(STRPTR)msg_NotEnoughPtForOpenSpline_STR},
  {msg_NotEnoughPtForPolygon,(STRPTR)msg_NotEnoughPtForPolygon_STR},
  {msg_NotEnoughPtForSpline,(STRPTR)msg_NotEnoughPtForSpline_STR},
  {msg_Object,(STRPTR)msg_Object_STR},
  {msg_ObjectCopiedToScrapFile,(STRPTR)msg_ObjectCopiedToScrapFile_STR},
  {msg_ObjectDeletedFromScrapFile,(STRPTR)msg_ObjectDeletedFromScrapFile_STR},
  {msg_OnScreenAA,(STRPTR)msg_OnScreenAA_STR},
  {msg_Open,(STRPTR)msg_Open_STR},
  {msg_OpenColorWindow,(STRPTR)msg_OpenColorWindow_STR},
  {msg_OpenFile,(STRPTR)msg_OpenFile_STR},
  {msg_OpenPrefFile,(STRPTR)msg_OpenPrefFile_STR},
  {msg_Orientation,(STRPTR)msg_Orientation_STR},
  {msg_OutputOption,(STRPTR)msg_OutputOption_STR},
  {msg_Pad,(STRPTR)msg_Pad_STR},
  {msg_Page,(STRPTR)msg_Page_STR},
  {msg_Pattern,(STRPTR)msg_Pattern_STR},
  {msg_PenColor,(STRPTR)msg_PenColor_STR},
  {msg_PictureFile,(STRPTR)msg_PictureFile_STR},
  {msg_PictureSelection,(STRPTR)msg_PictureSelection_STR},
  {msg_PictureSize,(STRPTR)msg_PictureSize_STR},
  {msg_PlaceLibObj,(STRPTR)msg_PlaceLibObj_STR},
  {msg_Plain,(STRPTR)msg_Plain_STR},
  {msg_PlainNice,(STRPTR)msg_PlainNice_STR},
  {msg_PlainSpear,(STRPTR)msg_PlainSpear_STR},
  {msg_PlainTriangle,(STRPTR)msg_PlainTriangle_STR},
  {msg_PolylineChopError,(STRPTR)msg_PolylineChopError_STR},
  {msg_PolylineChopped,(STRPTR)msg_PolylineChopped_STR},
  {msg_PolylineSplineIntersectNotImpl,(STRPTR)msg_PolylineSplineIntersectNotImpl_STR},
  {msg_PolylineTangentError,(STRPTR)msg_PolylineTangentError_STR},
  {msg_Portrait,(STRPTR)msg_Portrait_STR},
  {msg_Precision,(STRPTR)msg_Precision_STR},
  {msg_PrintBuffer,(STRPTR)msg_PrintBuffer_STR},
  {msg_Project,(STRPTR)msg_Project_STR},
  {msg_Proportional,(STRPTR)msg_Proportional_STR},
  {msg_PubScreen,(STRPTR)msg_PubScreen_STR},
  {msg_R2VImport,(STRPTR)msg_R2VImport_STR},
  {msg_Radial,(STRPTR)msg_Radial_STR},
  {msg_Radius,(STRPTR)msg_Radius_STR},
  {msg_Radiuses,(STRPTR)msg_Radiuses_STR},
  {msg_ReadAccessBlocked,(STRPTR)msg_ReadAccessBlocked_STR},
  {msg_RegpolySelected,(STRPTR)msg_RegpolySelected_STR},
  {msg_RemoveLastColorStop,(STRPTR)msg_RemoveLastColorStop_STR},
  {msg_RemovePtPictError,(STRPTR)msg_RemovePtPictError_STR},
  {msg_RemoveStop,(STRPTR)msg_RemoveStop_STR},
  {msg_Repeat,(STRPTR)msg_Repeat_STR},
  {msg_Reset,(STRPTR)msg_Reset_STR},
  {msg_Right,(STRPTR)msg_Right_STR},
  {msg_RightArrow,(STRPTR)msg_RightArrow_STR},
  {msg_RigidText,(STRPTR)msg_RigidText_STR},
  {msg_Round,(STRPTR)msg_Round_STR},
  {msg_Save,(STRPTR)msg_Save_STR},
  {msg_SaveFile,(STRPTR)msg_SaveFile_STR},
  {msg_SavePalette,(STRPTR)msg_SavePalette_STR},
  {msg_SavePrefFile,(STRPTR)msg_SavePrefFile_STR},
  {msg_ScaleSelected,(STRPTR)msg_ScaleSelected_STR},
  {msg_Scaling,(STRPTR)msg_Scaling_STR},
  {msg_ScreenMode,(STRPTR)msg_ScreenMode_STR},
  {msg_ScreenModeSelect,(STRPTR)msg_ScreenModeSelect_STR},
  {msg_ScreenOption,(STRPTR)msg_ScreenOption_STR},
  {msg_Select2ndPt,(STRPTR)msg_Select2ndPt_STR},
  {msg_SelectAllFields,(STRPTR)msg_SelectAllFields_STR},
  {msg_SelectDirectory,(STRPTR)msg_SelectDirectory_STR},
  {msg_SelectedCirclesAreCongruent,(STRPTR)msg_SelectedCirclesAreCongruent_STR},
  {msg_SelectedEllAndPolylineNoInter,(STRPTR)msg_SelectedEllAndPolylineNoInter_STR},
  {msg_SelectedPolylineNoIntersect,(STRPTR)msg_SelectedPolylineNoIntersect_STR},
  {msg_SelectPattern,(STRPTR)msg_SelectPattern_STR},
  {msg_SelectSecondColor,(STRPTR)msg_SelectSecondColor_STR},
  {msg_SetOriginalPicSize,(STRPTR)msg_SetOriginalPicSize_STR},
  {msg_SFactorEdit,(STRPTR)msg_SFactorEdit_STR},
  {msg_SFactorValue,(STRPTR)msg_SFactorValue_STR},
  {msg_ShowLength,(STRPTR)msg_ShowLength_STR},
  {msg_ShowNums,(STRPTR)msg_ShowNums_STR},
  {msg_SideRuler,(STRPTR)msg_SideRuler_STR},
  {msg_SignedAreaReset,(STRPTR)msg_SignedAreaReset_STR},
  {msg_SingularityError,(STRPTR)msg_SingularityError_STR},
  {msg_Size,(STRPTR)msg_Size_STR},
  {msg_Skin,(STRPTR)msg_Skin_STR},
  {msg_SkinSelect,(STRPTR)msg_SkinSelect_STR},
  {msg_Snap,(STRPTR)msg_Snap_STR},
  {msg_SoftScroll,(STRPTR)msg_SoftScroll_STR},
  {msg_Solid,(STRPTR)msg_Solid_STR},
  {msg_SpecialText,(STRPTR)msg_SpecialText_STR},
  {msg_SplineArcInterNotImpl,(STRPTR)msg_SplineArcInterNotImpl_STR},
  {msg_SplineDiaNotImpl,(STRPTR)msg_SplineDiaNotImpl_STR},
  {msg_SplineEndPtNotImpl,(STRPTR)msg_SplineEndPtNotImpl_STR},
  {msg_SplineMidPtNotImpl,(STRPTR)msg_SplineMidPtNotImpl_STR},
  {msg_SplineOnePtCutError,(STRPTR)msg_SplineOnePtCutError_STR},
  {msg_SplineSnapNotImpl,(STRPTR)msg_SplineSnapNotImpl_STR},
  {msg_SplineSplineInterNotImpl,(STRPTR)msg_SplineSplineInterNotImpl_STR},
  {msg_SplineTangentNotImpl,(STRPTR)msg_SplineTangentNotImpl_STR},
  {msg_SplineTextInterNotImpl,(STRPTR)msg_SplineTextInterNotImpl_STR},
  {msg_SplinsNormalNotImpl,(STRPTR)msg_SplinsNormalNotImpl_STR},
  {msg_StartColor,(STRPTR)msg_StartColor_STR},
  {msg_StatCheckErrorCutFile,(STRPTR)msg_StatCheckErrorCutFile_STR},
  {msg_StdIO,(STRPTR)msg_StdIO_STR},
  {msg_StepNb,(STRPTR)msg_StepNb_STR},
  {msg_Stroke,(STRPTR)msg_Stroke_STR},
  {msg_Style,(STRPTR)msg_Style_STR},
  {msg_SVGImport,(STRPTR)msg_SVGImport_STR},
  {msg_Symetric,(STRPTR)msg_Symetric_STR},
  {msg_TangentAdded,(STRPTR)msg_TangentAdded_STR},
  {msg_TangentImpossible,(STRPTR)msg_TangentImpossible_STR},
  {msg_TangentOnlyForPolylines,(STRPTR)msg_TangentOnlyForPolylines_STR},
  {msg_TangentPtNotOnArc,(STRPTR)msg_TangentPtNotOnArc_STR},
  {msg_Tenth,(STRPTR)msg_Tenth_STR},
  {msg_Text,(STRPTR)msg_Text_STR},
  {msg_TextAdjust,(STRPTR)msg_TextAdjust_STR},
  {msg_TextAngle,(STRPTR)msg_TextAngle_STR},
  {msg_TextBox,(STRPTR)msg_TextBox_STR},
  {msg_TextColor,(STRPTR)msg_TextColor_STR},
  {msg_TextFont,(STRPTR)msg_TextFont_STR},
  {msg_TextRigid,(STRPTR)msg_TextRigid_STR},
  {msg_TextSize,(STRPTR)msg_TextSize_STR},
  {msg_TextSpecial,(STRPTR)msg_TextSpecial_STR},
  {msg_Thickness,(STRPTR)msg_Thickness_STR},
  {msg_TickThickness,(STRPTR)msg_TickThickness_STR},
  {msg_TopRuler,(STRPTR)msg_TopRuler_STR},
  {msg_Unconstrained,(STRPTR)msg_Unconstrained_STR},
  {msg_Undo,(STRPTR)msg_Undo_STR},
  {msg_UnselectAll,(STRPTR)msg_UnselectAll_STR},
  {msg_Updates,(STRPTR)msg_Updates_STR},
  {msg_Use,(STRPTR)msg_Use_STR},
  {msg_UsePref,(STRPTR)msg_UsePref_STR},
  {msg_UserColors,(STRPTR)msg_UserColors_STR},
  {msg_UserScale,(STRPTR)msg_UserScale_STR},
  {msg_UseTicks,(STRPTR)msg_UseTicks_STR},
  {msg_WidthHeightFactor,(STRPTR)msg_WidthHeightFactor_STR},
  {msg_WidthLengthFactor,(STRPTR)msg_WidthLengthFactor_STR},
  {msg_WidthPercent,(STRPTR)msg_WidthPercent_STR},
  {msg_WriteBackup,(STRPTR)msg_WriteBackup_STR},
  {msg_ZoomFactor,(STRPTR)msg_ZoomFactor_STR},
  {msg_ZoomIn,(STRPTR)msg_ZoomIn_STR},
  {msg_ZoomOut,(STRPTR)msg_ZoomOut_STR},
  {msg_OtherOptions,(STRPTR)msg_OtherOptions_STR},
  {msg_SelectShadeTint,(STRPTR)msg_SelectShadeTint_STR},
  {msg_SelectSVGfile,(STRPTR)msg_SelectSVGfile_STR},
  {msg_LastMsg,(STRPTR)msg_LastMsg_STR},
  {msg_MouseFunction,(STRPTR)msg_MouseFunction_STR},
  {msg_BoxScaleMsg1,(STRPTR)msg_BoxScaleMsg1_STR},
  {msg_BoxScaleMsg2,(STRPTR)msg_BoxScaleMsg2_STR},
  {msg_Diameter,(STRPTR)msg_Diameter_STR},
  {msg_Length,(STRPTR)msg_Length_STR},
  {msg_Distance,(STRPTR)msg_Distance_STR},
  {msg_Locate_Object,(STRPTR)msg_Locate_Object_STR},
  {msg_first_point,(STRPTR)msg_first_point_STR},
  {msg_next_point,(STRPTR)msg_next_point_STR},
  {msg_center_point,(STRPTR)msg_center_point_STR},
  {msg_mid_point,(STRPTR)msg_mid_point_STR},
  {msg_final_point,(STRPTR)msg_final_point_STR},
  {msg_corner_point,(STRPTR)msg_corner_point_STR},
  {msg_delete_point,(STRPTR)msg_delete_point_STR},
  {msg_direction,(STRPTR)msg_direction_STR},
  {msg_Circle_center,(STRPTR)msg_Circle_center_STR},
  {msg_Ellipse_center,(STRPTR)msg_Ellipse_center_STR},
  {msg_Ellipse_corner,(STRPTR)msg_Ellipse_corner_STR},
  {msg_Circle_diameter,(STRPTR)msg_Circle_diameter_STR},
  {msg_final_corner,(STRPTR)msg_final_corner_STR},
  {msg_set_radius,(STRPTR)msg_set_radius_STR},
  {msg_final_angle,(STRPTR)msg_final_angle_STR},
  {msg_freehand,(STRPTR)msg_freehand_STR},
  {msg_single_point,(STRPTR)msg_single_point_STR},
  {msg_dimension_line,(STRPTR)msg_dimension_line_STR},
  {msg_position_cursor,(STRPTR)msg_position_cursor_STR},
  {msg_break_add_here,(STRPTR)msg_break_add_here_STR},
  {msg_place_new_point,(STRPTR)msg_place_new_point_STR},
  {msg_align_compound,(STRPTR)msg_align_compound_STR},
  {msg_align_canvas,(STRPTR)msg_align_canvas_STR},
  {msg_tag_object,(STRPTR)msg_tag_object_STR},
  {msg_tag_region,(STRPTR)msg_tag_region_STR},
  {msg_align_tagged,(STRPTR)msg_align_tagged_STR},
  {msg_add_arrow,(STRPTR)msg_add_arrow_STR},
  {msg_delete_arrow,(STRPTR)msg_delete_arrow_STR},
  {msg_break_compound,(STRPTR)msg_break_compound_STR},
  {msg_break_and_tag,(STRPTR)msg_break_and_tag_STR},
  {msg_Put_Front,(STRPTR)msg_Put_Front_STR},
  {msg_Put_Back,(STRPTR)msg_Put_Back_STR},
  {msg_Select_axe_object,(STRPTR)msg_Select_axe_object_STR},
  {msg_Select_log_object,(STRPTR)msg_Select_log_object_STR},
  {msg_Clear_axe_list,(STRPTR)msg_Clear_axe_list_STR},
  {msg_open_compound,(STRPTR)msg_open_compound_STR},
  {msg_open_keep_visible,(STRPTR)msg_open_keep_visible_STR},
  {msg_spline_line,(STRPTR)msg_spline_line_STR},
  {msg_open_closed,(STRPTR)msg_open_closed_STR},
  {msg_copy_object,(STRPTR)msg_copy_object_STR},
  {msg_horiz_vert_copy,(STRPTR)msg_horiz_vert_copy_STR},
  {msg_copy_to_cut_buf,(STRPTR)msg_copy_to_cut_buf_STR},
  {msg_place_object,(STRPTR)msg_place_object_STR},
  {msg_array_placement,(STRPTR)msg_array_placement_STR},
  {msg_delete_object,(STRPTR)msg_delete_object_STR},
  {msg_delete_region,(STRPTR)msg_delete_region_STR},
  {msg_del_to_cut_buf,(STRPTR)msg_del_to_cut_buf_STR},
  {msg_Modify_SFactor,(STRPTR)msg_Modify_SFactor_STR},
  {msg_Set,(STRPTR)msg_Set_STR},
  {msg_flip,(STRPTR)msg_flip_STR},
  {msg_copy_flip,(STRPTR)msg_copy_flip_STR},
  {msg_set_anchor,(STRPTR)msg_set_anchor_STR},
  {msg_unset_anchor,(STRPTR)msg_unset_anchor_STR},
  {msg_compound_tagged,(STRPTR)msg_compound_tagged_STR},
  {msg_Join_Lines_Splines,(STRPTR)msg_Join_Lines_Splines_STR},
  {msg_Split_Line_Spline,(STRPTR)msg_Split_Line_Spline_STR},
  {msg_Choose_next_Line,(STRPTR)msg_Choose_next_Line_STR},
  {msg_Choose_next_Spline,(STRPTR)msg_Choose_next_Spline_STR},
  {msg_select_save,(STRPTR)msg_select_save_STR},
  {msg_select_object,(STRPTR)msg_select_object_STR},
  {msg_angle_tip,(STRPTR)msg_angle_tip_STR},
  {msg_save_angle,(STRPTR)msg_save_angle_STR},
  {msg_select_add,(STRPTR)msg_select_add_STR},
  {msg_reset_to_0,(STRPTR)msg_reset_to_0_STR},
  {msg_move_object,(STRPTR)msg_move_object_STR},
  {msg_horiz_vert_move,(STRPTR)msg_horiz_vert_move_STR},
  {msg_move_point,(STRPTR)msg_move_point_STR},
  {msg_new_posn,(STRPTR)msg_new_posn_STR},
  {msg_new_object,(STRPTR)msg_new_object_STR},
  {msg_cancel_library,(STRPTR)msg_cancel_library_STR},
  {msg_rotate_object,(STRPTR)msg_rotate_object_STR},
  {msg_copy_rotate,(STRPTR)msg_copy_rotate_STR},
  {msg_set_center,(STRPTR)msg_set_center_STR},
  {msg_unset_center,(STRPTR)msg_unset_center_STR},
  {msg_scale_box,(STRPTR)msg_scale_box_STR},
  {msg_scale_about_center,(STRPTR)msg_scale_about_center_STR},
  {msg_add_tangent,(STRPTR)msg_add_tangent_STR},
  {msg_add_normal,(STRPTR)msg_add_normal_STR},
  {msg_update_object,(STRPTR)msg_update_object_STR},
  {msg_Get_settings,(STRPTR)msg_Get_settings_STR},
  {msg_place_and_edit,(STRPTR)msg_place_and_edit_STR},
  {msg_change_draw_mode,(STRPTR)msg_change_draw_mode_STR},
  {msg_place_at_orig,(STRPTR)msg_place_at_orig_STR},
  {msg_HelpWnd,(STRPTR)msg_HelpWnd_STR},
  {msg_SkinChange,(STRPTR)msg_SkinChange_STR},
  {msg_SwapColorStop,(STRPTR)msg_SwapColorStop_STR},
  {msg_SecondSwapStop,(STRPTR)msg_SecondSwapStop_STR},
  {msg_SwapDone,(STRPTR)msg_SwapDone_STR},
  {msg_Swap,(STRPTR)msg_Swap_STR},
  {msg_Add,(STRPTR)msg_Add_STR},
  {msg_Del,(STRPTR)msg_Del_STR},
  {msg_CopyColor,(STRPTR)msg_CopyColor_STR},
  {msg_SwapColor,(STRPTR)msg_SwapColor_STR},
  {msg_SelectCopyColor,(STRPTR)msg_SelectCopyColor_STR},
  {msg_SelectSwapColor,(STRPTR)msg_SelectSwapColor_STR},
  {msg_ColorPicker,(STRPTR)msg_ColorPicker_STR},
  {msg_GradDSpread,(STRPTR)msg_GradDSpread_STR},
  {msg_FreeHandMode,(STRPTR)msg_FreeHandMode_STR},
  {msg_AutoSaveSettings,(STRPTR)msg_AutoSaveSettings_STR},
  {msg_OffScreenFailed,(STRPTR)msg_OffScreenFailed_STR},
  {msg_TooManyPts,(STRPTR)msg_TooManyPts_STR},
  {0,NULL}
};

#endif /* CATCOMP_ARRAY */

/***************************************************************/

#ifdef CATCOMP_BLOCK

#error CATCOMP_BLOCK not supported by this SD

#endif /* CATCOMP_BLOCK */

/***************************************************************/

#ifdef CATCOMP_CODE

#error CATCOMP_CODE not supported by this SD

#endif /* CATCOMP_CODE */

/***************************************************************/


#endif /* AMIFIG_STRINGS_H */
