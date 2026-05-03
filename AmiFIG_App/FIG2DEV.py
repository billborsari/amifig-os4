#!/usr/bin/env python
#
# Fig2Dev GUI for AmigaOS
# Ported from FIG2DEV.lua to Python 2.5 + ProAction
#

import sys
import os
import arexx
import time

# Global Debug Toggle
DEBUG = False
if "-debug" in sys.argv:
    DEBUG = True

def Log(msg):
    if DEBUG:
        print "DEBUG: %s" % msg
    try:
        # File logging for AmigaOS debugging
        f = open("T:FIG2DEV_LOG.txt", "a")
        f.write("%s: %s\n" % (time.ctime(), msg))
        f.close()
    except:
        pass

class Application:
    def __init__(self):
        self.PORTNAME = "FIG2DEV_PY"
        self.UNIQUE = True
        self.scr = "Workbench"
        self.InFile = ""
        self.OutputDir = None
        
        # Ensure T: is used for temporary files (netpbm/fig2dev often want TMPDIR)
        os.putenv("TMPDIR", "T:")
        os.putenv("TEMP", "T:")
        
        # UI Lists
        self.ExportFormats = ["box","cgm","dxf","eepic","eepicemu","emf","epic","eps","gbx","ge","gif","ibmgl",
                            "jpeg","latex","map","mf","mp","pcx","pdf","pdftex","pdftex_t","pic",
                            "pict2e","pictex","png","ppm","ps","pstex","pstex_t","pstricks","ptk","shape",
                            "sld","svg","textyl","tiff","tikz","tk","tpic","xbm","xpm"]
        self.UnitList = ["cm","in"]
        self.DimList = ["None","0.1","0.25","0.5","1","5","10"]
        self.PaperList = ["A5","A4","A3","A2","A1","Letter"]
        self.OrientList = ["Portrait","Landscape"]
        
        # Gadget IDs
        # Container for internal ObjectIDs (assigned at runtime)
        self.ids = {}
        
        # Static GA_IDs (still used for gadget creation)
        self.ID_INFILE     = 101
        self.ID_FORMAT     = 102
        self.ID_MAGNIF     = 103
        self.ID_MAXDIM     = 104
        self.ID_MAXDIMUNIT = 105
        self.ID_GRIDMINOR  = 106
        self.ID_GRIDMAJOR  = 107
        self.ID_GRIDUNIT   = 108
        self.ID_FONTSIZE   = 109
        self.ID_PAPER      = 110
        self.ID_ORIENT     = 111
        self.ID_GENBUTTON  = 112
        self.ID_CMDLINE    = 113
        self.ID_OUTFILE    = 114
        self.ID_EXPORT     = 115
        self.ID_CANCEL     = 116
        
        # Node List IDs
        self.fmt_list_id = None
        self.unit_list_id = None
        self.dim_list_id = None
        self.paper_list_id = None
        self.orient_list_id = None

def ErrorExit(msg):
    command = "RequestChoice TYPE \"ERROR\" \"Fig2Dev GUI\" \"" + msg + "\" \"OK\" >NIL:"
    os.system(command)
    sys.exit()

def GetPorts():
    try:
        ports = os.getports()
        ports = " ".join(ports)
    except AttributeError:
        (rc,rc2,ports) = arexx.dorexx("REXX","return show('P')")
        if rc != 0: ErrorExit("Couldn't Find ARexx!")
    return ports

def MakeNodeList(guikey, items, type="lb"):
    res = arexx.dorexx("PROACTION","NEWGUIOBJECT GUIID " + str(guikey) + " OBJECTTYPE \"GUIOBJ_List\"")
    lid = res[2]
    
    for item in items:
        if type == "lb":
            tags = "LBNA_Column,0,LBNCA_Text," + str(item) + ",TAG_DONE"
            node_res = arexx.dorexx("PROACTION","ALLOCLISTBROWSERNODE GUIID " + str(guikey) + " COLUMNS 1 TAGSTRING \"" + tags + "\"")
        else:
            tags = "CNA_Text," + str(item) + ",TAG_DONE"
            node_res = arexx.dorexx("PROACTION","ALLOCCHOOSERNODE GUIID " + str(guikey) + " TAGSTRING \"" + tags + "\"")
        
        if node_res[0] == 0:
            arexx.dorexx("PROACTION", "ADDTAIL GUIID " + str(guikey) + " LISTID " + str(lid) + " NODEID " + str(node_res[2]))

    return lid

def MakeColumnList(guikey, title):
    tags = "LBCIA_Column,0,LBCIA_Title," + str(title) + ",TAG_DONE"
    res = arexx.dorexx("PROACTION","ALLOCLISTBROWSERCOLUMNINFO GUIID " + str(guikey) + " COLUMNS 1 TAGSTRING \"" + tags + "\"")
    if res[0] == 0:
        return res[2]
    return None

def FreeNodeList(guikey, lid, type="lb"):
    if not lid: return
    (rc, rc2, node) = arexx.dorexx("PROACTION","REMTAIL GUIID " + guikey + " LISTID " + lid)
    while rc == 0 and node and node != "None":
        if type == "lb":
            arexx.dorexx("PROACTION","FREELISTBROWSERNODE GUIID " + guikey + " NODEID " + node)
        else:
            arexx.dorexx("PROACTION","FREECHOOSERNODE GUIID " + guikey + " NODEID " + node)
        (rc, rc2, node) = arexx.dorexx("PROACTION","REMTAIL GUIID " + guikey + " LISTID " + lid)

def IsValid(gid):
    if gid is None: return False
    sgid = str(gid)
    if sgid == "" or sgid == "0" or sgid == "None" or sgid.startswith("ERROR"):
        return False
    return True

def GetAttr(guikey, gid, attr):
    if not IsValid(guikey) or not IsValid(gid): return ""
    res = arexx.dorexx("PROACTION","GETATTR GUIID " + str(guikey) + " OBJECTID " + str(gid) + " TAGNAME \"" + str(attr) + "\"")
    if res[0] == 0:
        val = str(res[2]).strip()
        if val == "None" or val == "": return ""
        # Remove surrounding quotes recursively if needed
        while len(val) > 1 and ((val[0] == "'" and val[-1] == "'") or (val[0] == '"' and val[-1] == '"')):
            val = val[1:-1].strip()
        return val
    return ""

def SetAttr(guikey, gid, attr, value):
    if not IsValid(guikey) or not IsValid(gid): return
    v = str(value).replace('"', '')
    cmd = "SETATTRS GUIID " + str(guikey) + " OBJECTID " + str(gid) + " TAGSTRING \"" + str(attr) + "," + v + ",TAG_DONE\""
    Log("SetAttr CMD: %s" % cmd)
    res = arexx.dorexx("PROACTION", cmd)
    if res[0] != 0:
        Log("SetAttr Failed! RC: %s" % res[0])

def MatchGadget(gid, oid, target_id):
    if not IsValid(target_id): return False
    if IsValid(gid) and str(gid) == str(target_id): return True
    if IsValid(oid) and str(oid) == str(target_id): return True
    return False

def GenerateCommandLine(guikey, app):
    # Find fig2dev executable
    fig2dev_exe = None
    
    # Try PROGDIR: paths first (AmigaOS)
    candidates = [
        "PROGDIR:fig2dev/fig2dev",
        "PROGDIR:fig2dev",
        "fig2dev/fig2dev",
    ]
    for c in candidates:
        Log("Checking fig2dev path: %s exists=%s" % (c, str(os.path.exists(c))))
        if os.path.exists(c):
            fig2dev_exe = c
            break
    
    if not fig2dev_exe:
        fig2dev_exe = "fig2dev/fig2dev"  # Use relative path as last resort
        Log("Using default fig2dev path: %s" % fig2dev_exe)
    
    cmd = "\"" + fig2dev_exe + "\""
    
    # Format
    fmt_idx = GetAttr(guikey, app.ids.get('FORMAT'), "LISTBROWSER_Selected") or "18"
    fmt = str(app.ExportFormats[int(fmt_idx)]).lower().strip()
    cmd += " -L " + fmt

    # Magnification
    magnif = GetAttr(guikey, app.ids.get('MAGNIF'), "INTEGER_Number") or "100"
    maxdim = GetAttr(guikey, app.ids.get('MAXDIM'), "STRINGA_TextVal")
    if not maxdim:
        cmd += " -m " + str(float(magnif)/100.0)
    else:
        unit_idx = GetAttr(guikey, app.ids.get('MAXDIMUNIT'), "CHOOSER_Selected") or "0"
        cmd += " -Z " + maxdim + app.UnitList[int(unit_idx)]
        
    # Grid
    minor_idx = GetAttr(guikey, app.ids.get('GRIDMINOR'), "CHOOSER_Selected") or "0"
    if int(minor_idx) > 0:
        opt = app.DimList[int(minor_idx)]
        major_idx = GetAttr(guikey, app.ids.get('GRIDMAJOR'), "CHOOSER_Selected") or "0"
        if int(major_idx) > 0:
            opt += ":" + app.DimList[int(major_idx)]
        unit_idx = GetAttr(guikey, app.ids.get('GRIDUNIT'), "CHOOSER_Selected") or "0"
        opt += app.UnitList[int(unit_idx)]
        cmd += " -G " + opt
    
    # FontSize
    font = GetAttr(guikey, app.ids.get('FONTSIZE'), "INTEGER_Number") or "12"
    cmd += " -s " + font
    
    # Paper & Orient - Only supported by specific drivers (PS, EPS, PDF, LaTeX, etc.)
    paper_formats = ["ps", "eps", "pdf", "latex", "pdftex", "pstex", "pdftex_t", "pstex_t", "pict2e", "pstricks", "tikz"]
    if fmt in paper_formats:
        paper_idx = GetAttr(guikey, app.ids.get('PAPER'), "CHOOSER_Selected") or "5"
        orient_idx = GetAttr(guikey, app.ids.get('ORIENT'), "CHOOSER_Selected") or "0"
        cmd += " -z " + app.PaperList[int(paper_idx)]
        if int(orient_idx) == 1: cmd += " -landscape"
    
    # Input File
    infile = GetAttr(guikey, app.ids.get('INFILE'), "GETFILE_FullFile")
    if not infile or infile == "None" or infile == "":
        infile = GetAttr(guikey, app.ids.get('INFILE'), "STRINGA_TextVal")
    if infile: infile = infile.strip().strip("'\"")
    if infile and infile.lower().startswith("ram disk:"):
        infile = "ram:" + infile[9:]
    if infile: cmd += " " + infile
    
    # Output File
    outfile = GetAttr(guikey, app.ids.get('OUTFILE'), "GETFILE_FullFile")
    if not outfile or outfile == "None" or outfile == "":
        outfile = GetAttr(guikey, app.ids.get('OUTFILE'), "STRINGA_TextVal")
    if outfile: outfile = outfile.strip().strip("'\"")
    if outfile and outfile.lower().startswith("ram disk:"):
        outfile = "ram:" + outfile[9:]
    if outfile and outfile != "Select output path...": 
        cmd += " " + outfile
    
    return cmd

def HandleInput(pyport, guikey, app):
    die = 0
    while die == 0:
        pyport.wait()
        msg = pyport.getmsg()
        while msg:
            try:
                cmd = msg.msg
                msg.reply()
                if cmd == "QUIT" or cmd[:5] == "CLOSE":
                    die = 1; break
                elif cmd[:8] == "GADGETUP":
                    Log("Msg: %s" % cmd)
                    parts = cmd.split()
                    gid = None; oid = None
                    if "GADTID" in parts:
                        idx = parts.index("GADTID")
                        if idx+1 < len(parts): gid = parts[idx+1].strip()
                    elif "GADGETID" in parts: 
                        idx = parts.index("GADGETID")
                        if idx+1 < len(parts): gid = parts[idx+1].strip()
                    elif "OBJECTID" in parts:
                        idx = parts.index("OBJECTID")
                        if idx+1 < len(parts): oid = parts[idx+1].strip()
                    
                    if gid is None and oid is None and len(parts) >= 4:
                        oid = parts[2].strip(); gid = parts[3].strip()
                    
                    is_input_event = False
                    if MatchGadget(gid, oid, app.ids.get('INFILE')):
                        arexx.dorexx("PROACTION","REQUESTFILE GUIID " + str(guikey) + " OBJECTID " + str(app.ids.get('INFILE')))
                        is_input_event = True
                    if MatchGadget(gid, oid, app.ids.get('OUTFILE')):
                        arexx.dorexx("PROACTION","REQUESTFILE GUIID " + str(guikey) + " OBJECTID " + str(app.ids.get('OUTFILE')))
                        is_input_event = True
                    
                    is_config_event = MatchGadget(gid, oid, app.ids.get('FORMAT')) or \
                                     MatchGadget(gid, oid, app.ids.get('MAGNIF')) or \
                                     MatchGadget(gid, oid, app.ids.get('MAXDIM')) or \
                                     MatchGadget(gid, oid, app.ids.get('MAXDIMUNIT')) or \
                                     MatchGadget(gid, oid, app.ids.get('GRIDMINOR')) or \
                                     MatchGadget(gid, oid, app.ids.get('GRIDMAJOR')) or \
                                     MatchGadget(gid, oid, app.ids.get('GRIDUNIT')) or \
                                     MatchGadget(gid, oid, app.ids.get('FONTSIZE')) or \
                                     MatchGadget(gid, oid, app.ids.get('PAPER')) or \
                                     MatchGadget(gid, oid, app.ids.get('ORIENT'))

                    if is_input_event or is_config_event:
                        cl = GenerateCommandLine(guikey, app)
                        SetAttr(guikey, app.ids.get('CMDLINE'), "STRINGA_TextVal", cl)
                    elif MatchGadget(gid, oid, app.ids.get('GENBUTTON')):
                        cl = GenerateCommandLine(guikey, app)
                        SetAttr(guikey, app.ids.get('CMDLINE'), "STRINGA_TextVal", cl)
                    elif MatchGadget(gid, oid, app.ids.get('EXPORT')):
                        Log("EXPORT button pressed")
                        outfile = GetAttr(guikey, app.ids.get('OUTFILE'), "GETFILE_FullFile")
                        Log("OUTFILE GETFILE_FullFile = '%s'" % str(outfile))
                        if not outfile or outfile == "None" or outfile == "":
                            outfile = GetAttr(guikey, app.ids.get('OUTFILE'), "STRINGA_TextVal")
                            Log("OUTFILE STRINGA_TextVal = '%s'" % str(outfile))
                        if outfile: outfile = outfile.strip().strip("'\"")
                        
                        # AMIGAOS Fix: convert "Ram Disk:" to "ram:" and check for spaces
                        if outfile and outfile.lower().startswith("ram disk:"):
                            outfile = "ram:" + outfile[9:]
                            Log("Normalized Ram Disk: to %s" % outfile)

                        if not outfile or outfile == "":
                            Log("Export Error: No output file selected")
                            os.system("RequestChoice \"Fig2Dev\" \"Please select an output file first.\" \"OK\"")
                            msg = pyport.getmsg()
                            continue
                        
                        if ' ' in outfile:
                            Log("Export Error: Spaces in output path: %s" % outfile)
                            os.system("RequestChoice TYPE \"ERROR\" \"Fig2Dev\" \"Spaces are not allowed in the output path!*NPlease rename your folders or files to remove spaces.\" \"OK\"")
                            msg = pyport.getmsg()
                            continue
                        
                        # Check Input File for spaces too
                        infile = GetAttr(guikey, app.ids.get('INFILE'), "GETFILE_FullFile")
                        if not infile or infile == "None" or infile == "":
                            infile = GetAttr(guikey, app.ids.get('INFILE'), "STRINGA_TextVal")
                        if infile:
                            infile = infile.strip().strip("'\"")
                            if infile.lower().startswith("ram disk:"): infile = "ram:" + infile[9:]
                            if ' ' in infile:
                                Log("Export Error: Spaces in input path: %s" % infile)
                                os.system("RequestChoice TYPE \"ERROR\" \"Fig2Dev\" \"Spaces are not allowed in the input file path!*NPlease rename your folders or files to remove spaces.\" \"OK\"")
                                msg = pyport.getmsg()
                                continue

                        Log("Export to: %s" % outfile)
                        
                        # Build the command line from current gadget values
                        cl = GenerateCommandLine(guikey, app)
                        Log("Command line: %s" % cl)
                        
                        # Show status window
                        status_tags = "WA_Width,250,WA_Height,80,WA_Title,Fig2Dev Status,WINDOW_Position,WPOS_CENTERSCREEN,WA_Activate,1,TAG_DONE"
                        (rcS, rc2S, statuskey) = arexx.dorexx("PROACTION","CREATEGUI PORTNAME " + pyport.name + " TAGSTRING \"" + status_tags + "\"")
                        arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + statuskey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_VERT,LAYOUT_SpaceOuter,True,TAG_DONE\"")
                        arexx.dorexx("PROACTION","ADDIMAGE GUIID " + statuskey + " IMAGECLASS \"label.image\" TAGSTRING \"LABEL_Text,Exporting...,TAG_DONE\"")
                        arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + statuskey)
                        arexx.dorexx("PROACTION","DOGUI GUIID " + statuskey)
                        arexx.dorexx("PROACTION","OPENGUIWINDOW GUIID " + statuskey)
                        
                        ret = -1
                        try:
                            fmt_idx = GetAttr(guikey, app.ids.get('FORMAT'), "LISTBROWSER_Selected") or "18"
                            fmt = str(app.ExportFormats[int(fmt_idx)]).lower().strip()
                            Log("Format index=%s fmt=%s" % (str(fmt_idx), fmt))
                            
                            # High-quality formats handled via Python GS call for stability
                            is_python_gs = (fmt in ["pdf", "png", "jpeg"])
                            Log("is_python_gs=%s" % str(is_python_gs))
                            
                            if is_python_gs:
                                # Two steps: fig2dev->EPS then gs->FinalFormat
                                temp_eps = "T:fig2dev_temp.eps"
                                cl_eps = cl.replace("-L " + fmt, "-L eps")
                                # Replace output file with temp EPS (careful with spaces/quotes)
                                cl_eps = cl_eps.replace(" " + outfile, " " + temp_eps)
                                
                                exec_cmd = cl_eps + " >T:FIG2DEV_EXEC.txt"
                                Log("EPS step: %s" % exec_cmd)
                                ret = os.system(exec_cmd)
                                Log("EPS return code: %d" % ret)
                                
                                if ret == 0:
                                    # Find gs executable
                                    gs_exe = None
                                    gs_candidates = ["PROGDIR:gs", "APPDIR:gs", "C:gs", "gs"]
                                    for g in gs_candidates:
                                        if os.path.exists(g):
                                            gs_exe = g; break
                                    if not gs_exe: gs_exe = "gs"
                                    
                                    # Determine GS device
                                    gs_dev = "pdfwrite"
                                    if fmt == "png": gs_dev = "png16m"
                                    elif fmt == "jpeg": gs_dev = "jpeg"
                                    
                                    # Run GS
                                    # Use the optimized flags we discovered
                                    gs_cmd = gs_exe + " -dSAFER -dNOPAUSE -dBATCH -sDEVICE=" + gs_dev + " -r80 -sOutputFile=" + outfile + " " + temp_eps + " >T:FIG2DEV_EXEC.txt"
                                    Log("GS step: %s" % gs_cmd)
                                    ret = os.system(gs_cmd)
                                    Log("GS return code: %d" % ret)
                                
                                # Clean up temp EPS
                                try: os.remove(temp_eps)
                                except: pass
                            else:
                                # All other formats: run fig2dev directly
                                exec_cmd = cl + " >T:FIG2DEV_EXEC.txt"
                                Log("Running: %s" % exec_cmd)
                                ret = os.system(exec_cmd)
                                Log("Return code: %d" % ret)
                            
                            # Log any output from fig2dev/gs
                            try:
                                ef = open("T:FIG2DEV_EXEC.txt", "r")
                                output = ef.read()
                                ef.close()
                                if output.strip():
                                    Log("Tool output: %s" % output.strip())
                            except:
                                pass
                        except Exception, e:
                            Log("Exception: %s" % str(e))
                            ret = -1
                        
                        arexx.dorexx("PROACTION","CLOSEGUIWINDOW GUIID " + statuskey)
                        arexx.dorexx("PROACTION","DESTROYGUI GUIID " + statuskey)
                        
                        if ret == 0:
                            Log("Export complete: %s" % outfile)
                            os.system("RequestChoice \"Fig2Dev\" \"Export completed successfully!\" \"OK\"")
                            die = 1; break
                        else:
                            Log("Export failed RC=%d" % ret)
                            os.system("RequestChoice \"Fig2Dev\" \"Export failed (RC=" + str(ret) + ").*NCheck T:FIG2DEV_LOG.txt\" \"OK\"")
                    elif MatchGadget(gid, oid, app.ids.get('CANCEL')):
                        die = 1; break
            except Exception, e:
                Log("Python Error: %s" % str(e))
                die = 1; break
            msg = pyport.getmsg()

def DoGUI(app):
    pyport = arexx.Port(app.PORTNAME)
    if not pyport: ErrorExit("Couldn't create ARexx port")
    
    wintags = "WA_Width,600,WA_Height,600,WA_DragBar,1,WA_DepthGadget,1,WA_SizeGadget,1,WA_CloseGadget,1,"
    wintags += "WA_Title,Fig2Dev GUI (Python),WA_PubScreenName," + app.scr + ",WINDOW_Position,WPOS_CENTERSCREEN,WA_Activate,1,TAG_DONE"
    
    (rc,rc2,guikey) = arexx.dorexx("PROACTION","CREATEGUI PORTNAME " + pyport.name + " TAGSTRING \"" + wintags + "\"")
    if rc != 0: ErrorExit("ProAction CREATEGUI failed with RC " + str(rc))
    
    app.fmt_list_id = MakeNodeList(guikey, app.ExportFormats, "lb")
    app.fmt_col_id = MakeColumnList(guikey, "Format")
    app.unit_list_id = MakeNodeList(guikey, app.UnitList, "cho")
    app.dim_list_id = MakeNodeList(guikey, app.DimList, "cho")
    app.paper_list_id = MakeNodeList(guikey, app.PaperList, "cho")
    app.orient_list_id = MakeNodeList(guikey, app.OrientList, "cho")
    
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + str(guikey) + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_VERT,LAYOUT_SpaceInner,True,LAYOUT_SpaceOuter,True,TAG_DONE\"")
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + str(guikey) + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,LAYOUT_Label,Input File,LAYOUT_BevelStyle,BVAL_GROUP,LAYOUT_SpaceInner,True,TAG_DONE\"")
    
    path = str(app.InFile or "")
    tags = "GA_ID," + str(app.ID_INFILE) + ",GA_RelVerify,1,GA_TabCycle,1,GA_ReadOnly,1,GA_Weight,100"
    if path:
        tags += ",GETFILE_FullFile," + path
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + str(guikey) + " GADGETCLASS \"getfile.gadget\" TAGSTRING \"" + tags + ",TAG_DONE\"")
    app.ids['INFILE'] = res[2]
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + str(guikey))
    
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,TAG_DONE\"")
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_VERT,LAYOUT_Label,Format,LAYOUT_BevelStyle,BVAL_GROUP,TAG_DONE\"")
    tags = "GA_ID," + str(app.ID_FORMAT) + ",LISTBROWSER_Labels," + str(app.fmt_list_id) + ",LISTBROWSER_ColumnInfo," + str(app.fmt_col_id or 0) + ",LISTBROWSER_ShowSelected,1,LISTBROWSER_Selected,18,GA_RelVerify,1,TAG_DONE"
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + str(guikey) + " GADGETCLASS \"listbrowser.gadget\" TAGSTRING \"" + tags + "\"")
    app.ids['FORMAT'] = res[2]
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey)
    
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_VERT,TAG_DONE\"")
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_VERT,LAYOUT_Label,General,LAYOUT_BevelStyle,BVAL_GROUP,TAG_DONE\"")
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,TAG_DONE\"")
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"integer.gadget\" TAGSTRING \"GA_ID," + str(app.ID_MAGNIF) + ",INTEGER_Number,100,INTEGER_Minimum,10,INTEGER_Maximum,1000,TAG_DONE\"")
    app.ids['MAGNIF'] = res[2]
    arexx.dorexx("PROACTION","ADDIMAGE GUIID " + guikey + " IMAGECLASS \"label.image\" TAGSTRING \"LABEL_Text,%,TAG_DONE\"")
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey)
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,TAG_DONE\"")
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"string.gadget\" TAGSTRING \"GA_ID," + str(app.ID_MAXDIM) + ",STRINGA_TextVal,\"\",TAG_DONE\"")
    app.ids['MAXDIM'] = res[2]
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"chooser.gadget\" TAGSTRING \"GA_ID," + str(app.ID_MAXDIMUNIT) + ",CHOOSER_Labels," + app.unit_list_id + ",CHOOSER_Selected,0,TAG_DONE\"")
    app.ids['MAXDIMUNIT'] = res[2]
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey)
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey)
    
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,LAYOUT_Label,Grid,LAYOUT_BevelStyle,BVAL_GROUP,TAG_DONE\"")
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"chooser.gadget\" TAGSTRING \"GA_ID," + str(app.ID_GRIDMINOR) + ",CHOOSER_Labels," + app.dim_list_id + ",CHOOSER_Selected,0,TAG_DONE\"")
    app.ids['GRIDMINOR'] = res[2]
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"chooser.gadget\" TAGSTRING \"GA_ID," + str(app.ID_GRIDMAJOR) + ",CHOOSER_Labels," + app.dim_list_id + ",CHOOSER_Selected,0,TAG_DONE\"")
    app.ids['GRIDMAJOR'] = res[2]
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"chooser.gadget\" TAGSTRING \"GA_ID," + str(app.ID_GRIDUNIT) + ",CHOOSER_Labels," + app.unit_list_id + ",CHOOSER_Selected,0,TAG_DONE\"")
    app.ids['GRIDUNIT'] = res[2]
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey)
    
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,LAYOUT_Label,Text,LAYOUT_BevelStyle,BVAL_GROUP,TAG_DONE\"")
    arexx.dorexx("PROACTION","ADDIMAGE GUIID " + guikey + " IMAGECLASS \"label.image\" TAGSTRING \"LABEL_Text,Size,TAG_DONE\"")
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"integer.gadget\" TAGSTRING \"GA_ID," + str(app.ID_FONTSIZE) + ",INTEGER_Number,12,INTEGER_Minimum,4,INTEGER_Maximum,100,TAG_DONE\"")
    app.ids['FONTSIZE'] = res[2]
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey)
    
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,LAYOUT_Label,Page,LAYOUT_BevelStyle,BVAL_GROUP,TAG_DONE\"")
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"chooser.gadget\" TAGSTRING \"GA_ID," + str(app.ID_PAPER) + ",CHOOSER_Labels," + app.paper_list_id + ",CHOOSER_Selected,5,GA_RelVerify,1,TAG_DONE\"")
    app.ids['PAPER'] = res[2]
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"chooser.gadget\" TAGSTRING \"GA_ID," + str(app.ID_ORIENT) + ",CHOOSER_Labels," + app.orient_list_id + ",CHOOSER_Selected,0,GA_RelVerify,1,TAG_DONE\"")
    app.ids['ORIENT'] = res[2]
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey)
    
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey)
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey)
    
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_VERT,LAYOUT_Label,Template,LAYOUT_BevelStyle,BVAL_GROUP,LAYOUT_SpaceOuter,True,TAG_DONE\"")
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"button.gadget\" TAGSTRING \"GA_ID," + str(app.ID_GENBUTTON) + ",GA_Text,Generate,GA_RelVerify,1,TAG_DONE\"")
    app.ids['GENBUTTON'] = res[2]
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"string.gadget\" TAGSTRING \"GA_ID," + str(app.ID_CMDLINE) + ",GA_RelVerify,1,GA_TabCycle,1,STRINGA_TextVal,Click Generate to see fig2dev commandline,TAG_DONE\"")
    app.ids['CMDLINE'] = res[2]
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey)
    
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,LAYOUT_Label,Output File,LAYOUT_BevelStyle,BVAL_GROUP,TAG_DONE\"")
    tags = "GA_ID," + str(app.ID_OUTFILE) + ",GETFILE_DoSaveMode,1,GA_RelVerify,1,GA_TabCycle,1,GA_Weight,100"
    
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"getfile.gadget\" TAGSTRING \"" + tags + ",TAG_DONE\"")
    app.ids['OUTFILE'] = res[2]
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + guikey)
    
    arexx.dorexx("PROACTION","ADDLAYOUT GUIID " + guikey + " TAGSTRING \"LAYOUT_Orientation,LAYOUT_ORIENT_HORIZ,LAYOUT_EvenSize,True,TAG_DONE\"")
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"button.gadget\" TAGSTRING \"GA_ID," + str(app.ID_EXPORT) + ",GA_Text,_Export,GA_RelVerify,1,TAG_DONE\"")
    app.ids['EXPORT'] = res[2]
    res = arexx.dorexx("PROACTION","ADDGADGET GUIID " + guikey + " GADGETCLASS \"button.gadget\" TAGSTRING \"GA_ID," + str(app.ID_CANCEL) + ",GA_Text,_Cancel,GA_RelVerify,1,TAG_DONE\"")
    app.ids['CANCEL'] = res[2]
    arexx.dorexx("PROACTION","ENDLAYOUT GUIID " + str(guikey))
    
    arexx.dorexx("PROACTION","DOGUI GUIID " + str(guikey))
    arexx.dorexx("PROACTION","OPENGUIWINDOW GUIID " + guikey)
    
    if app.InFile:
        cl = GenerateCommandLine(guikey, app)
        SetAttr(guikey, app.ids.get('CMDLINE'), "STRINGA_TextVal", cl)
    
    HandleInput(pyport, guikey, app)
    
    arexx.dorexx("PROACTION","CLOSEGUIWINDOW GUIID " + guikey)
    FreeNodeList(guikey, app.fmt_list_id, "lb")
    FreeNodeList(guikey, app.unit_list_id, "cho")
    FreeNodeList(guikey, app.dim_list_id, "cho")
    FreeNodeList(guikey, app.paper_list_id, "cho")
    FreeNodeList(guikey, app.orient_list_id, "cho")
    arexx.dorexx("PROACTION","DESTROYGUI GUIID " + guikey)

if __name__ == "__main__":
    iapp = Application()
    args = sys.argv
    i = 1
    while i < len(args):
        arg = args[i]
        if arg == "-screen" and i+1 < len(args):
            iapp.scr = args[i+1]; i += 2
        elif (arg == "-i" or arg == "--input") and i+1 < len(args):
            iapp.InFile = args[i+1]; i += 2
        elif (arg == "-o" or arg == "--output") and i+1 < len(args):
            iapp.OutputDir = args[i+1]; i += 2
        elif arg == "-debug":
            i += 1
        elif not arg.startswith("-"):
            iapp.InFile = arg; i += 1
        else:
            i += 1
            
    ports = GetPorts()
    if -1 == ports.find("PROACTION"):
        os.system("RUN >NIL: APPDIR:PROACTION")
        os.system("C:WaitForPort PROACTION")
        time.sleep(1)
        ports = GetPorts()
        if -1 == ports.find("PROACTION"):
            ErrorExit("Unable to find or start ProAction")
            
    DoGUI(iapp)
