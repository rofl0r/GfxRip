-------------------------------------------------------------------------------
BGUI 2.0 readme
10/18/00

This version of BGUI, 2.0 has been released unoffically by Jason Winnebeck,
gillius@webzone.net, http://www.rit.edu/~jpw9607/

The new readme file is called readme.txt and the original file is called 
origread.txt. The new readme file is simply a version of the origread with a
little bit of new information added and all of the irrelevant stuff taken out.

I've had this done now for 10 months and since the author hasn't rereleased it
himself in that time I have released it myself since so many people ask for it.
I have been using the library with all of my programs and updated it when the
WIP came out and I could no longer use the library.

NEW FEATURES:
The moveable dialog system has been TOTALLY REVAMPED.  The updating code is now
seriously faster, and new options are provided.  Originally the entire screen
was blitted to a buffer to be used as a temp buffer and then the temp buffer
was modified and blitted entirely to the screen even while the dialog was not
moving creating extreme flicker!

The new system values no flickering over update speed now, but it is still
MUCH faster as it updates only the changed areas of the screen rather than the
whole screen every time, and not updating at all while the dialog is idle and
not moving.

In the d1 field You may now place three options, depending on your preference:

BW_UPDATE: The original style of updating scheme, writing directly to the
screen and updating the controls as they wish.  This was the original
algorithm, although it has been rewritten and is now extremely fast and
removes almost all flickering.  BW_UPDATE is defined as 0, so your old
programs will port without changes.  BGUI updates the GUI by sending an IDLE
message followed by a redraw.

BW_BUFFEREDUPDATE: is the same style as above, but uses a double buffer and
vsync to remove even more flicker.  The GUI is constantly updated so the
controls will update even if the dialog is moving.

BW_BUFFERED: Basically "takes a screenshot" of the GUI and moves that around.
Fastest method and least flicker, but the GUI will not be updated while the
user holds down the mouse on the title bar.

CHANGES:

I used to have a change log of the huge amounts of things I changed since then.
A lot of the code was copied straight out of Allegro needlessly.  I deleted
entirely what code I could and replaced it with calls to the Allegro code.
Most of the other code was straight from Allegro with only changes to the
color.  I replaced almost all of the code with freshly modified code from
Allegro -- I chose this way since my only objective at the time was to get it
to compile and run without crashing on almost every control.

Because of this I updaded it to use the acquire/release paradigm of the new
Windows code and optimized the mouse hiding/showing as it was wrong.

In various places that I saw I optimized code where I could and fixed up lots
of small, little things.

The library now works for C++, unlike the original BGUI (the original readme
says this).

The moveable windows have been totally revamped.  Check NEW FEATURES for help
on the new system.

The drop boxes are fixed now and they don't segfault.

I forget if there were other broken controls but I believe they all work now
(as I said I lost the changelog and I did these changes 10 months ago).

The GFX selector hasn't been REMOVED.  I took it out, unless you want your
Windows user to try setting mode_x or your Linux user to try setting 400x600
Cirrus Logic mode ;).  Use Allegro's selector.

BUGS:

I haven't updated all of the widgets to work with unicode.  Stick with ASCII
if you want to be safe but some of the widgets I really worked on now have
unicode support.

I've tested this library and use it constantly in MSVC.  I tested it once
successfully in DJGPP, but never in any other Allegro port, but it SHOULD
work...
