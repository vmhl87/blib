A simple braille rendering system written in C.

`blib.c` contains the core bitarr -> wchar conversion
code, and `bgui.c` has utilities which streamline
usage of blib in full-terminal graphical applications.

Quite a few demos are bundled:
- snow.c: an extremely simple demo of moving particles
- life.c: conway's game life
- tesseract.c: animated rotating tesseract

Unfortunately there is no handling for resizing of
terminal, at least not yet.
