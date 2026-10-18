#!/usr/bin/env python3
"""Send test keystrokes to the focused window on an isolated Xvfb display."""
import ctypes as C
import sys
import time
x = C.CDLL('libX11.so.6')
t = C.CDLL('libXtst.so.6')
x.XOpenDisplay.argtypes = [C.c_char_p]
x.XOpenDisplay.restype = C.c_void_p
x.XStringToKeysym.argtypes = [C.c_char_p]
x.XStringToKeysym.restype = C.c_ulong
x.XKeysymToKeycode.argtypes = [C.c_void_p, C.c_ulong]
x.XKeysymToKeycode.restype = C.c_uint
x.XFlush.argtypes = [C.c_void_p]
t.XTestFakeKeyEvent.argtypes = [C.c_void_p, C.c_uint, C.c_int, C.c_ulong]
d = x.XOpenDisplay(None)
if not d:
    raise SystemExit('Cannot open test display')
for key in sys.argv[1:]:
    if key.startswith('wait:'):
        time.sleep(float(key[5:]))
        continue
    duration=.15
    if key.startswith('hold:'):
        _, key, seconds = key.split(':')
        duration=float(seconds)
    code = x.XKeysymToKeycode(d, x.XStringToKeysym(key.encode()))
    if not code:
        raise SystemExit(f'Unknown key: {key}')
    t.XTestFakeKeyEvent(d, code, 1, 0)
    x.XFlush(d)
    time.sleep(duration)
    t.XTestFakeKeyEvent(d, code, 0, 0)
    x.XFlush(d)
    time.sleep(.25)
