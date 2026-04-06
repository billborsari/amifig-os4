from __future__ import print_function
import arexx
import os
import sys

def diag():
    print("Checking ARexx Ports...")
    found = False
    for bridge in ["REXX", "AREXX", ""]:
        try:
            res = arexx.dorexx(bridge, "return show('P')")
            print("Bridge '%s' result: RC=%d, RC2=%d, RESULT='%s'" % (bridge, res[0], res[1], res[2]))
            if res[0] == 0:
                ports = res[2].split()
                print("Found %d ports: %s" % (len(ports), ", ".join(ports)))
                for p in ports:
                    if p.upper().find("PROACTION") != -1:
                        print(">>> PROACTION PORT FOUND: '%s'" % p)
                        found = True
                break
        except Exception as e:
            print("Bridge '%s' failed: %s" % (bridge, str(e)))

    if not found:
        print("PROACTION port not found.")
        print("Attempting to search for ProAction executable...")
        # Common locations
        for path in ["APPDIR:PROACTION", "PROACTION", "SYS:Utilities/ProAction/ProAction", "Work:ProAction/ProAction"]:
            print("Trying to start: %s" % path)
            os.system("RUN >NIL: %s" % path)
            os.system("C:WaitForPort PROACTION")
            # Re-check
            res = arexx.dorexx("REXX", "return show('P')")
            if res[0] == 0 and res[2].upper().find("PROACTION") != -1:
                print("Success! ProAction started via %s" % path)
                break

if __name__ == "__main__":
    diag()
