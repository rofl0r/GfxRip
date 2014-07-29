This is an up-to-date version of readme.txt.  The last official release
in 1998's text file is called origread.txt
------------------------------------------------------------------------

Original program author: Dan Huizenga (Skis)
Web: http://www.concentric.net/~skis
E/M: skis@concentric.net


    T  h  e       B  G  u  i       R  e  a  d  m  e
    -----------------------------------------------
             Proudly Created with Notepad.

=================
=  Intro Stuff  =
=================

  BGui    -- A win-95 like drop in replacement for the  
             Allegro GUI.

  License -- 
    BGui is gift-ware. It was created by a number of people working in 
    cooperation, and is given to you freely as a gift. You may use, modify, 
    redistribute, and generally hack it about in any way you like, and you do 
    not have to give us anything in return. However, if you like this product 
    you are encouraged to thank us by making a return gift to the Allegro 
    community. This could be by writing an add-on package, providing a useful 
    bug report, making an improvement to the library, or perhaps just 
    releasing the sources of your program so that other people can learn from 
    them. If you redistribute parts of this code or make a game using it, it 
    would be nice if you mentioned BGUI somewhere in the credits, but you 
    are not required to do this. We trust you not to abuse our generosity.

  You Break it.. -- If this breaks your computer, printer, body,
                    brain, or anything else it's not my fault and I
                    cannot be held responsible.

  Updates -- New versions will be available at
             http://www.rit.edu/~jpw9607/

  New in this release --
             see readme!.txt

==========================
=  Contributors to BGui  =
==========================
This was a hard section to write.
There are just so MANY!
BGui is based on the Allegro GUI,
therefore, anybody who contributed
to that contributed to BGui.

People who have contributed specifically to
BGui include:

 Jason Winnebeck -- Updated library to fix segfault bugs, improve performace, and bring
                    BGui up to date with the Allegro 3.9 WIP

 Dan Huizenga (Skis) -- the original author of this program.

 Shawn Hargreaves -- Shawn wrote many of the
  (in an indirect    origional GUI routines.
   sort of way :-)   Much of BGui's code came
                     from them.  Please see
                     the Allegro credits for
                     information on the contributors
                     to the origional Allegro
                     GUI.

 Johan J”nsson -- Wrote the vertical 3D line.
                  (Will be used in a future
                   release for "subdividing"
                   frames )
 Jason Eccles  -- Made tdbox slightly faster
                  by cutting the number of
                  calls to get_bill_color
                  in half.
 Julien Audran -- Contributed Drop Down List,
                  Group Box, fixed BillCheck
                  to respond to D_DISABLED.
                  Also contributed BitmapButton
                  and ToolBar, but I still have
                  to add those in (at least, I
                  did when I wrote this :-)

==================
=  Notes on Use  =
==================

1. Compatibility notes:
    A. There are several small differences from BGui and Allegro. Although
       I have tried to maintain as much compatibility as possible, there are
       a few minor differences, please see the "differences from Allegro"
       section before using BGui.

2. Quick Start:
    To give your program the Windows 95 look, just put "bill_" in front
    of all of the procedures in your dialog array. Also remember to
    make everything 2-6 pixels taller and/or wider, to accommodate the
    "3D" look. You will also have to do a search and replace of "do_menu"
    to "do_billmenu".  Finally, be sure to remember to initialize BGui's
    which is covered under "Initializing BGui" under "Notes on use".

    *Note: You will also have to follow the compiling/installation
           instructions.  BGui no longer comes with a precompiled
           library.

3. Compiling with BGui -- for DJGPP users
    You will have to compile with "gcc myprog.c -lalleg -lbgui".
    That is the absolute minimum you can have on your command line.
    Be sure to copy BGui to you djgpp\lib directory, and tdgui.h
    to you djgpp\include directory.

4. Initializing BGui.

    Step 1: Palette initialization
    ------------------------------
     This must always be done before the other steps.

     BGui requires that certain colors be available to it in order to
     create the 3D look. You can initialize this palette in 3 ways. Please
     note that in modes greater than 8-bit you do not have to do this.

     1. bill_init_pal();
      This creates a gray palette with blue for title bars. This uses the
      entire palette.

     2. bill_init_smallpal(index of where you want the small palette to start);
      This uses only a part of the palette, taking one palette index for each
      BGui color constant.  The integer passed to it is where the palette will
      begin.  This is the recommended method of setting up the palette.

     3. bill_init_very_smallpal(palette starting position);
      This is not recommended unless you _really_ need the few palette entries
      this will save you. (When I typed this, it saved 3 or 4. That may change
      in the future).

    Step 2: BGui Initialization
    ---------------------------
     Simply call Bill_init.  This must be done after you've initialized
     the palette (if you are in 8-bit mode), and before you use any
     BGui functions

    Step 3: Re-initializing upon graphics mode/palette change
    ---------------------------------------------------------
     Call bill_init_radio_buttons().
     BGui does this automatically when you call bill_init, but you
     must do this again every time you change and/or move BGui's
     palette in 8 bit mode, and also every time you change
     graphics modes.  I havn't changed the name of the function
     to maintain compatibility with existing code, but it is
     inaccurate as BGui will crash wether or not you use radio
     buttons if you fail to call it.

==========================================
= Compiling BGui and things made with it =
==========================================
 1. Making and installing the library
   run the makefile with make
   Make sure to place the resulting library in your compiler's library
    directory.
   Place the header file with your projects or your standard includes.

 2. Compiling with BGui 
   #include "tdgui.h" into your program

=================
=  Known Bugs   =
=================
  see readme!.txt

============================
= Differences from Allegro =
============================
Notes on Everything:
  - The FG and BG properties are now ignored. You will have to
    change the color constants (you can do this at run-time) in
    order to change the colors.  If you do change the color
    constants, be sure to call bill_init_smallpal if you are in
    8-bit mode. _DO NOT_ use bill_init_pal, or bill_init_verysmallpal.
    Also, I reccomend calling bill_init_radio_buttons after changing
    color constants.

  - All of your controls will have to be made a little taller and
    wider.

Notes on "alert":
  In BGui, the first message string passed to alert and alert3
  is used as the window title. This can cause some programs to
  look very bad.  Maybe I will change this in later versions,
  and make this behavior user selectable, with the other choice
  being to have a global string for message box titles that you
  could change.

Notes on menus:
  Due to the way "do_menu" and "_do_menu" were written in
  the original Allegro, I had to change them.  If your
  program uses these, please call "do_billmenu" and
  "_do_billmenu" instead.

Notes on d_billclear_proc:
  Unlike it's Allegro counterpart, d_billclear_proc DOES obey
  it's x, y, w, h. So be sure to take those 0's out of w/h and
  make them values as larg as or larger than your resolution
  is.

Notes on radio buttons:
  Radio buttons can be round or square. They are square if
  the button's d2 is 1.  Square radio buttons look like a
  standard BillButton.  Also, the size of the round
  "button" part will always be the same, since it is a bitmap,
  and is not drawn with an algorithm.
  You MUST call bill_init_radio_buttons every time you switch
  graphics modes, unless you have already called bill_init in
  the current graphics mode.

======================
= BGui F A Q         =
======================
1. Q - Why are BGui's colors all funny in the 8 bit graphics mode
       program that I just made?

   A - You forgot to initialize the palette.  Information on doing
       this can be found under "initializing BGui", which is in
       "Notes on use"

2. Q - Can I use BGui with my C++ program?

   A - Yes.

5. Q - My program that uses BGui crashes, what can I do to fix it?

   A - Solving Mysterious Crashes
   ------------------------------
       i.   - Have you properly initialized BGui?  Information on this
              is in "Notes on uses" under "initializing BGui".

       ii.  - Have you read this entire file?  Usefull information is
              scattered througout it in a very abstract mannor.

       iii. - Have you compared your program with BTest.c to see what
              you are doing different?

=======================================================
= The Mysteries of The Things In The TDGui.h File ... =
=======================================================

There are also explanations of stuff in .H file.
Info on most of the #defines can be found in BGui.c
before the tdbox() proc.

See the readme! file for more information.

-=>#define WM_XXXXXX
  Messages for the BGui Window Manager which will allow you to have
  multiple standard dialogs open in different windows at one time.
  ** Unused.  The BGui Window Manager has been implemented in the
     form of BWinSys.  These are left in case I decide to write
     a true Window Manager some day, which would replace Allegro's
     Dialog Manager.

-=>#define W_XXXXXX
  Responses for w_xxxxxxx_proc's
  ** See above note

-=>typedef struct WS
  WinSystem structure. Like dialog, only for a window w/ a pointer
  to the dialog it will contain.
  ** See above note

-=>typedef struct MOVABLE_INFO
  Currently unused

-=>#define STL_XXXXXXX
  Used by tdbox()

-=>#define SCL_XXXXXXX
  Used by scrollbar

-=>#define BILL_SMALLPAL_SIZE 9 
  This is just the number of colors required
  for the bill_init_smallpal  

-=>#define BILL_VERY_SMALLPAL_SIZE 7
  This is just the number of colors required
  for the bill_init_very_smallpal  

-=>extern float bg_ver;
-=>extern char bg_ver_str;
  Version information.

-=>extern RGB bill_XXXXXXXX
  Color constants that control how BGui looks.
  Want to change the colors within BGui? Change
  these, and then call bill_init_smallpal()

-=>void set_gui_mode(int mode);
  Right now this only makes the buttons look
  more like Win95. Later I hope to add arrows
  to the scrollbars on the listbox.  Leaving
  this off will (after BGui is done) make the
  controls fit the same dimensions as their
  Allegro counterparts better.

-=>int get_bill_color(RGB billcolor);
  Just returns the closest index of an RGB.

-=>void bill_init_pal();
  sets up a gray palette w/ blue for the title bar.

-=>void bill_init_smallpal(int start);      
 use 9 colors of the palette for BGui, instead of all 255.

-=>void bill_init_very_smallpal(int start); 
  Same as smallpal, except it skips
  repeat colors, and takes the required
  color count down to 7. DONT USE THIS
  IF YOU ARE GOING TO CHANGE THE COLOR
  CONSTANTS

-=>void bill_init();                  
  As of now, this does absolutely nothing, but you should
  call it anyway in case I change that later.

-=>void tdbox(BITMAP *b, int x, int y, int w, int h, int stl);
  This routine is used to draw some part of almost EVERY object
  you see in BGui. If you can make this go faster, you can make
  BGui go faster.  By the way, does anybody want to write a 
  tdcircle for me? ;-)

-=>void tdhline(BITMAP *b, int x, int y, int x2);
  Horizontal 3D line

-=>void tdvline(BITMAP *b, int x, int y, int x2);
  Vertical 3D line

-=>static void bill_dotted_rect(int x1, int y1, int x2, int y2, int fg);
  Same as Allegro's dotted rect, except
  it leaves spaces inbetween the dots alone.

-=>void _draw_scrollable_billframe(DIALOG *d, int listsize, int offset,
                                   int height, int fg_color, int bg);
  Same as the counterpart function in
  Allegro, except it looks like windows
  95. (except there are no arrows on the
  scroll bar).

-=>void bill_fade_bmp(BITMAP *bmp, int clr);
  You know how Windows fades the screen
  when you hit shutdown? That's what this
  does.  Please note that it's kinda slow.

-=>int billalert3(char *s1, char *s2, char *s3, char *b1, char *b2,
                   char *b3, int c1, int c2, int c3);
  Same as in allegro, except 3D. NOTE: The char *s1 is put in the
  window's title bar, NOT in the window itself.

-=>int billalert(char *s1, char *s2, char *s3, char *b1, char *b2,
                   int c1, int c2);
  Same as above.



~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Dialog procedures
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

=============Origional BGui procedures which have no Allegro counterpart====================
-=>d_billprogress_proc(int msg, DIALOG *d, int c);
   Progress bar, or "percent finished" thing.
   d2 is current value, d1 is the maximum value
   that it will reach.  The percent is calculated
   with d2/d1.

-=>d_billgroupbox_proc(int msg, DIALOG *d, int c);
   A group box.  See the btest.c for an example.
   Optionally, dp1 can point to a string for the
   lable of the group.  This must appear in the
   dialog array befor the objects that it will contain.

-=>d_billddlist_proc(int msg,DIALOG *d,int c);
   Drop Down list.  Identicle to the standard list except
   that it "folds up", so that only the selected choice
   is visible.  See Btest.c for an example.

-=>d_billscrollablebitmap_proc(int msg, DIALOG *d, int c);
   This is almost exactly like Allegro's bitmap proc,
   except that it can contain x and y coordinants of
   where to blit from on the "from" bitmap.  These
   are stored in the d1 and d2 variables. Be sure to
   send it MSG_DRAW after changing them.

-=>int d_hbillscrollbar_proc(int msg, DIALOG *d, int c);
   Fully functional horizontal scroll bar. D1 value is
   the maximum, and the D2 value is the current value of the
   scroll bar. These do not respond to D_HIDDEN, instead just
   set the d1 value to 0, and they will hide themselves.
   The dp field can point to a proc in the format

   void scrollbar_change(int i).

   This will be called whenever the scrollbar's value changes,
   and the I will be equal to it's d2.


-=>int d_vbillscrollbar_proc(int msg, DIALOG *d, int c);
   Fully functional vertical scroll bar.  See above.

-=>int d_billwin_proc(int msg, DIALOG *d, int c);
  BillWindows now have the ability to move, but you
  have to show their dialog with movable_do_dialog, and
  everything in the dialog has to be contained by them.
  For using these in an update_dialog loop, use the
  functions shown in the "Added Bonus" section.

-=>int d_ex_billbutton_proc(int msg, DIALOG *d, int c);
  This is a button with a callback. It will not close the dialog,
  and it will not toggle, it just pops in and out when clicked, and
  calls the callback.
  The callback should look like this:
  int my_proc(void *dp3, int d2);

  The dp3 will be equal to the buttons's dp3, the d2 equal to the
  buttons's d2.


-=>int d_samplex_billbutton_proc(int msg, DIALOG *d, int c); //Button w/ callback
  Same as above, only with a slightly different format for
  the callback.
  The callback should be in the format
  int my_proc();
  The dp2 feild should point to a sample.

-=>int d_callback_billbutton_proc(int msg, DIALOG *d, int c);
  Same as above, doesn't play a sample

-=>int d_billhline_proc(int msg, DIALOG *d, int c);
  Draw a horizontal 3D line at d->x, d->y with width d->w.

-=>int d_billvline_proc(int msg, DIALOG *d, int c);
  Same as above, verticle.


=============Here's some stuff that I threw in as an added bonus :-)========================
-=>void replace_col(BITMAP *b, int oc, int nc);
  Replaces all occurences of OC in a B with NC (kind of like set_color in 8-bit).

-=>void make_solid_pal(PALETTE pal, int start, int stop, int r, int g, int b);
  Sets all RGB's in a palette from index start to index stop to r, g, b.

-=>int moveable_do_dialog(DIALOG *dialog, int focus_obj);
  You need to use this in order to have movable BillWindows.

-=>int init_moveable(DIALOG *dialog);
  Call this before init_dialog in an update_dialog loop.

-=>int shutdown_moveable(DIALOG *dialog);
  Call this before shutdown_dialog in an update_dialog loop.

============== Allegro routines w/ the Win 95 look =========================================
-=>int do_billmenu(MENU *menu, int x, int y);
   You MUST use this in place of the old "do_menu"
   in your programs.

int d_billmenu_proc(int msg, DIALOG *d, int c);

-=>int d_billradio_proc(int msg, DIALOG *d, int c);
  Same as allegro with the following exceptions.
   1. They can only be round.
   2. You MUST call bill_init_radio_buttons()
      before you use them in a new graphics mode,
      unless you have called bill_init in the current
      graphics mode.

int d_billtextbox_proc(int msg, DIALOG *d, int c);
   Same as Allegro.

int d_billedit_proc(int msg, DIALOG *d, int c);
   Same as Allegro.

int d_billslider_proc(int msg, DIALOG *d, int c);
   Same as Allegro.

int d_billlist_proc(int msg, DIALOG *d, int c);
   Same as Allegro.

int d_billbutton_proc(int msg, DIALOG *d, int c);
   Same as Allegro.

-=>int d_billtext_proc(int msg, DIALOG *d, int c);
-=>int d_billctext_proc(int msg, DIALOG *d, int c);
  Changed: Now you can use "BILL_CLEAR" and "BILL_TITLE"
           for the background color, to make it
           transparent, or the color of the title bar.

           Using "BILL_TITLE" for the foreground make
           the text the same color as the text on title
           bars.

int d_billcheck_proc(int msg, DIALOG *d, int c);
   Same as Allegro.

int d_billclear_proc(int msg, DIALOG *d, int c);
   Same as Allegro, except it obeys the x, y, h, and w
   settings.

int billfile_select(char *message, char *path, char *ext);
   Same as Allegro.
 
If you don't understand what something is doing, here's the list of what
you can do to figure it out:
1. Look at the Allegro counterpart.

Here's some comments that I copied out of the .c and .h file.  I don't have
the time to write much more, I have to go study :-(.

Color constants used to draw billgui routines. Want to change the colors?
use these, NOT the fg/bg properties of the dialogs

   ALL of the BillGui routines are based on the tdbox routine.
   Styles include:
   STL_IN     (1)        "in" state border (used w/ buttuns and frames)
   STL_OUT    (2)        "out" state border (used w/ buttons)
   STL_FRAME  (8)        used in conjunction w/ STL_IN, it switches the order
                         of the "shadow" lines at the top of a frame, so that
                         the light one is towards the outside
   STL_BUTTONDOWN (64)   used for buttons in appearexact mode 

   STL_FACE   (4)        fills the bg w/ bill_face. otherwise bg isn't drawn.
   STL_FLAT   (4)        same as above
   STL_FILLED (4)        same as above

   STL_FRAMEFILLED (32)  used for filled frames (white bg).  This DOES NOT include
                         objects drawn w/ draw_scrollable_billframe.  They have
                         to draw their own background.
   STL_DITHERED    (16)  draws a dither of bill_light and bill_hlight as the bg,
                         for scroll bars

Several styles are re-defined under different names.  This is simply
because they are used for different purposes.  They do exactly the same thing,
it is just to make the code easier to read / understand

You shouldnt have to used these #define's in your programs, they are used
for the tdbox() routine.

By the way, I should mention that quite a few Allegro variables/functions
are included in this source, with the same names. It wouldn't compile otherwise.
If you know why this is, PLEASE tell me! (Are they not in Allegro.h?)

Please note the 80% of the code in here was taken straight out of Allegro,
I only made a few small changes to the drawing routines.

