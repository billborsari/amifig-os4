import arexx
import os

print "Checking ARexx Ports..."
for bridge in ["REXX", "AREXX", ""]:
    try:
        res = arexx.dorexx(bridge, "return show('P')")
        print "Bridge '%s' result: RC=%d, RC2=%d, RESULT='%s'" % (bridge, res[0], res[1], res[2])
        if res[0] == 0:
            ports = res[2].split()
            print "Found %d ports." % len(ports)
            for p in ports:
                if p.upper().find("PROACTION") != -1:
                    print ">>> PROACTION PORT FOUND: '%s'" % p
            break
    except Exception, e:
        print "Bridge '%s' failed: %s" % (bridge, str(e))

print "Attempting to start ProAction..."
os.system("RUN >NIL: APPDIR:PROACTION")
os.system("C:WaitForPort PROACTION")

res = arexx.dorexx("REXX", "return show('P')")
print "Ports after startup: %s" % res[2]
