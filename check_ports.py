from __future__ import print_function
import arexx
import os

def check():
    print("Listing all ARexx ports:")
    try:
        # Try different bridges to be safe
        for bridge in ["REXX", "AREXX", ""]:
            try:
                res = arexx.dorexx(bridge, "return show('P')")
                if res[0] == 0:
                    ports = res[2].split()
                    print("Bridge '%s' found %d ports: %s" % (bridge, len(ports), ", ".join(ports)))
                    return
            except:
                continue
        print("Failed to list ports via any bridge.")
    except Exception as e:
        print("Error: %s" % str(e))

if __name__ == "__main__":
    check()
