#!/usr/bin/env python3

# keyboard automute for linux
# makes you less of an ass while using your MECHANICAL KEYBOARD in zoom/teams/whatever meetings
#
# usage:
# 1. build the kernel module in keycounts, then insmod keycounts.ko
# 2. run this script as root
# Note, you may need to modify the mute() and unmute() functions to actually mute and unmute
# on your system
# YMMV
# if you like this, buy me a beer or support your local charity

import sys,os,time

keycount = 0
lastkeyat=0

def mute():
    #os.system("pacmd list-sources | grep -oP 'index: \d+' | awk '{ print $2 }' | xargs -I{} pactl set-source-mute {} on")
    os.system("amixer set Capture toggle >/dev/null 2>&1")
def unmute():
    #os.system("pacmd list-sources | grep -oP 'index: \d+' | awk '{ print $2 }' | xargs -I{} pactl set-source-mute {} off")
    os.system("amixer set Capture toggle >/dev/null 2>&1")

muted=False

try:
    while True:
        newcount = int(open('/sys/kernel/debug/keypresscount','r').read().strip())
        time.sleep(.05)
        if newcount == 0:
            keycount = newcount
            continue
        if newcount != keycount:   # new keypresses since last time
            lastkeyat=time.time()
            keycount = newcount
            if not muted:
                print("c=%4d mute"%newcount)
                mute()
            muted = True
        if time.time()-lastkeyat > .25 and muted:
            print("c=%4d unmute"%keycount)
            muted = False
            unmute()
        time.sleep(.05)
except KeyboardInterrupt:
    sys.exit(0)
            
