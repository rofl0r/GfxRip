   Graphics Ripper   by TC
===============================
v 1.08  Released on 06.jul.2007
-------------------------------
    email: tomcat@sgn.net
-------------------------------
New in 1.08: Palette searching now uses WinUAE way of setting values
New in 1.07: Added CTRL + INSERT to copy the first image to ClipBoard
New in 1.06: Added R/F + SHIFT to quickly add/remove Skip bytes
             Added P + SHIFT to do an incremental Save
New in 1.05: Added two new Palette search modes - WinUAE Snapshot direct
             Copper List searching for ECS and AGA palettes. Will set the
             palette to the one that was active when the UNCOMPRESSED
             Snapshot (SaveState) was taken in WinUAE.
New in 1.04: Key 'U' turns on/off a zoom window at the bottom/right
             corner. Just a quick 'fix ... for EAB people ;-)

NOTE: You must know atleast some basics about how amiga displays the
      pictures on the screen! Since the program like this cannot be
      automatic at all you are on your own ! The only automatic thing
      is palette searching (and even that fails on some occasions).
      There is a little guide for those who don't exactly know what
      they are doing... hope that one helps !

Q/A                - X Size by 8 pixels
W/S                - Y Size by 1 pixel
W/S + SHIFT        - Y Size by 8 pixels
E/D                - Bits (Number of colors)
R/F                - Skip Bytes between pictures or bitplanes
R/F + SHIFT        - Skip Bytes Faster (by 8 bytes)
LEFT/RIGHT         - Picture Left/Right by 8 pixels (Offset by 1 byte)
UP/DOWN            - Picture Up/Down by 1 line (Offset by X Size/8)
UP/DOWN + SHIFT    - Picture Up/Down by 8 lines (Offset by X Size)
LEFT/RIGHT + SHIFT - Next/Previous Picture (Offset by X Size * Y Size)
G                  - Set Offset
H                  - Set Skip Value
X                  - Set Skip Mode (P - Pictures , B - Bitplanes)
P                  - Save picture(s)
P + SHIFT          - Save picture(s) with Auto-Incremental Name
L                  - Search for Amiga Palette (Press again for more)
Z                  - Palette search mode (look below for description)
K                  - Revert to standard Palette
J                  - Revert to last found Amiga palette
B                  - Set colour 0 to Magenta (255,0,255) - Transparent
N                  - Normal or Reverse order of bitplanes
0-7                - Select which picture goes into which bitplane
0-7 + SHIFT        - Same as above only in reverse direction
M                  - Bitplane Mode , see below !
C                  - Get Colour palette from a file
U                  - Turns on/off a Zoom window on the bottom/right corner
ESC                - Go out
CTRL + INSERT      - Copy the first image to Clipboard

Command Line Arguments:
-----------------------
Only one - Name of the file you want to process. If none is given then
a file selector will pop up where you can select the file.

Save Pictures:
--------------
Will save into current directory.
You can save any number of pictures to this filename. Instead of
number of pictures you specify how many of them will fit into X and
Y direction. The actual number of saved pictures is X*Y (ofcourse).
Also you can specify the direction in which the pictures will be
put into the file. The left->right direction is the same as you see
them on the screen. The up->down will do the opposite.

Bitplane Mode:
--------------
AM = Amiga standard : Each bitplane is in its own picture,
ST = ST standard :    Same picture holds all bitplanes - each other line
                      is a new bitplane - some tiles are in this format)
SP = Amiga Sprite :   This is a special mode to rip the AMIGA format
                      sprites in 16 colours (4 bitplanes). The sprites are
                      always 16 pixels wide (X Size) and can be any Y size.
                      You can spot them by seeing two bitmaps with 2
                      pictures in each bitmap (if looking in AM mode).
C+ = CPC Double :     CPC Standard Double Byte mode. One Character is one
                      bitplane - the Bytes follow each other for Plane 0,1.
                      (Head Over Heels, Batman)
C- = CPC Single :     CPC Standard Single Byte mode. One Character holds
                      BOTH bitplanes (4 pixels per Byte). This is used in
                      many games. Sometimes Bitplanes are mirrored.
                      (Amaurote, Ultimate games, etc.)
Palette Search:
---------------
By pressing this button the file is searched for a possible copperlist.
Bu default the Mode E is selected which basically extracts the ECS
palette from the UAE SaveState snapshot. If the palette was not found in
this mode then you will have to use one of the other three modes.

There are four palette search modes:

E = ECS UAE SaveState - The DEFAULT mode : the palette is extracted from the
    UAE SaveState snapshot (if found). ECS (upto 64 colours) only !
A = AGA UAE SaveState - the palette is extracted from the UAE SaveState
    snapshot (if found). AGA colours (upto 256) are extracted !
S = Simple - Doesn't find so many palettes, and it is fast, searches whole
    memory for possible palette entries (2 possible entries, always in
    sequence).
X = Advanced - The copperlist is searched in a way that when 4 possible
    colour entries (not needed to be in sequence) are found it is reported
    as a valid palette.

Make sure you save the Snapshots (SaveStates) as UNCOMPRESSED in WinUAE !!!

If you want to set colour 0 to allegro standard transparent then press 'B' !

Bitplane Order:
---------------
Normal bitplane order has first picture for bit 0, Reverse bitplane order
has last picture for bit 0. You can also press numbers 0-7 on your
keyboard to have a totally manual order of bitplanes (press it with
SHIFT and you will decrease picture number of that bitplane) !

Skip Mode:
----------
If the skip mode is 'P' it will skip number of bytes between each picture.
All bitplanes follow each other in this mode. If the skip mode is set to
'B' it will skip a number of bytes between each bitplane for the picture.

A bit of help if you can't start with it:
-----------------------------------------
Use amiga memory dumps which can be done with WinUAE (Misc section).
They can be a bit big, since they are uncompressed, but they are exactly
what you need !

The main thing is that you don't set the Bitplanes in the beginning -
first you find the pictures in 2 colour mode and after that you fiddle
with the bitplanes. 

The other thing is that you must have a sense of what is a bitmap. If
you think that tiles are 16 pixels wide that almost never means that 
they are stored like that in the memory, infact they are mostly stored 
in some totally bogus width. 
For this reason it is vital that you can "SPOT" the bitmap in any 
X-size... this can only be done with doing this thing a lot. But 
normally you can spot it when diagonal lines in the bitmap start 
appering. But if you don't know what i am talking about then it is best 
to put your X- Size (with Q/A) keys to some normal value for amiga 
graphics - like 320 for example. Set the Y-Size to 200 or so, so that 
you can go quickly through the memory - with SHIFT left/right arrows. 

Now when you spot something you *think* might be tiles or bitmap or 
something then use thr Q/A keys to alter the X-Size and then it will 
show up (probably - again you need practice for this to work ).

Now you got your X-size down you have to adjust the Y-Size and 
position (size with W/S and position with UP/DOWN (and with SHIFT)) ... 
when you have done that you have it mostly done....

Now you can see the picture on the screen and bitmaps are aligned one 
next to each other. Sometimes there is some rubbish between them and 
here you can use the R/F keys to adjust skip bytes between bitmaps.

Now you can select number of bitplanes with E/D keys... when the 
picture comes up in desired number of bitplanes (you will see when 
the rubbish starts to appear - then it is too many ;-) ) - after 
that you can start searching for the pallete - first use simple mode 
(normally it works) and after that the advanced if the simple mode 
didn't work. Sometimes you cannot find the pallete.

OK, in about half games the tiles/sprites are stored separately and 
in the other half the tiles/sprites are on the same bitmap (which is 
much bigger) - if they are stored separately then you can select how 
many tiles you want to save at once in your file (and how many in x/y 
direction).

I think this should explain the most of the inner workings of the 
tool... as I said, since it is fully manual you must have some 
practice to "spot" the bitmap when it doesn't have the right X-size.

And then there are other things, like interleaved bitmaps (i.e. ST 
type of bitmaps) where each other bitmap is in each other line... and
the Amiga format sprites, but this is advanced already (look above) ;-)


Have fun with this one !

TC