/*
 * BGui version 2.0 unoffical repackage and rerelease by Jason Winnebeck.
 * Original author: Dan Huizenga (Skis)
 *
 */

/*
File: BGui.c (C) 1998 Dan Huizenga (Skis)
Web: http://www.concentric.net/~skis
E/M: skis@concentric.net

BGui -- A win-95 like drop in replacement for the Allegro GUI.
All errors in this file are (C) 1998 Dan Huizenga. If you used these
routines in your program, all that I ask is E-Mail me about what your
program does, and send me a copy if I request it.
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "allegro.h"
#include "tdgui.h"
#include "binterna.h"


float bg_ver = 1.1;
char bg_ver_str[255] = "BGui Version 1.1 Omega";

/* The following are used by the billalert dialog */
#define A_S1  0
#define A_S2  1
#define A_S3  2
#define A_B1  3
#define A_B2  4
#define A_B3  5


static DIALOG billalert_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp) */
   { d_billwin_proc,        0,    0,    0,    0,    0,    0,    0,    0,       0,    0,    NULL },
   { d_billctext_proc,      0,    0,    0,    0,    0,    0,    0,    0,       0,    0,    NULL },
   { d_billctext_proc,      0,    0,    0,    0,    0,    0,    0,    0,       0,    0,    NULL },
   { d_billbutton_proc,     0,    0,    0,    0,    0,    0,    0,    D_EXIT,  0,    0,    NULL },
   { d_billbutton_proc,     0,    0,    0,    0,    0,    0,    0,    D_EXIT,  0,    0,    NULL },
   { d_billbutton_proc,     0,    0,    0,    0,    0,    0,    0,    D_EXIT,  0,    0,    NULL },
   { NULL }
};

void memverify_str(void *pointer, char *str)
{
 if(!pointer)
 {
  set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
  if(str)
   allegro_message("The program that you are running has run out of memory, and cannot continue. %s ", str);
  else 
   allegro_message("The program that you are running has run out of memory, and cannot continue.");
  exit(EXIT_FAILURE);
 }
}

void memverify(void *pointer)
{
 memverify_str(pointer, NULL);
}


void bill_init() {
 bill_init_radio_buttons(); 
 gui_mouse_focus = 0;
}

int get_gui_mode()
{
 int ret = bill_appearexact;
 return ret;
}

void set_gui_mode(int mode)
{
 /* In this version, the mode is TRUE
  *  or FALSE.  This makes the buttons
  *  look a little different, but that's
  *  pretty much it.
  */
 if (mode)
 {
  bill_appearexact = TRUE;
  gui_mouse_focus = FALSE;
 }
 else
 {
  bill_appearexact = FALSE;
  gui_mouse_focus = TRUE;
 }
}

/*
 *======================
 *=   BILL GUI Helpers =
 *======================
 */

int is_in(int xa, int ya, int xb, int yb, int w, int h)
{
 if ((xa < xb) || (ya < yb) || (xa > xb+w) || (ya > yb+h))
  return FALSE;
 return TRUE;
}



/*
  ==============================
  = BILL GUI dialogs           =
  ==============================
*/
/* Stolen directly from Allegro */
int billalert3(char *s1, char *s2, char *s3, char *b1, char *b2, char *b3, int c1, int c2, int c3)
{
   int maxlen = 0;
   int len1, len2, len3;
   int avg_w = text_length(font, " ");
   int avg_h = text_height(font);
   int buttons = 0;
   int b[3];
   int c;

   #define SORT_OUT_BUTTON(x) {                                            \
      if (b##x) {                                                          \
         billalert_dialog[A_B##x].flags &= ~D_HIDDEN;                          \
         billalert_dialog[A_B##x].key = c##x;                                  \
         billalert_dialog[A_B##x].dp = b##x;                                   \
	 len##x = gui_strlen(b##x);                                        \
	 b[buttons++] = A_B##x;                                            \
      }                                                                    \
      else {                                                               \
         billalert_dialog[A_B##x].flags |= D_HIDDEN;                           \
	 len##x = 0;                                                       \
      }                                                                    \
   }

   billalert_dialog[A_S1].dp = billalert_dialog[A_S2].dp = billalert_dialog[A_S3].dp = 
                           billalert_dialog[A_B1].dp = billalert_dialog[A_B2].dp = "";

   if (s1) {
      billalert_dialog[A_S1].dp = s1;
      maxlen = text_length(font, s1);
   }

   if (s2) {
      billalert_dialog[A_S2].dp = s2;
      len1 = text_length(font, s2);
      if (len1 > maxlen)
	 maxlen = len1;
   }

   if (s3) {
      billalert_dialog[A_S3].dp = s3;
      len1 = text_length(font, s3);
      if (len1 > maxlen)
	 maxlen = len1;
   }

   SORT_OUT_BUTTON(1);
   SORT_OUT_BUTTON(2);
   SORT_OUT_BUTTON(3);

   len1 = MAX(len1, MAX(len2, len3)) + avg_w*3;
   if (len1*buttons > maxlen)
      maxlen = len1*buttons;

   maxlen += avg_w*4;
   billalert_dialog[0].w = maxlen;
   billalert_dialog[A_S2].x = billalert_dialog[A_S3].x = 
                                                billalert_dialog[0].x + maxlen/2;

   billalert_dialog[A_B1].w = billalert_dialog[A_B2].w = billalert_dialog[A_B3].w = len1;

   billalert_dialog[A_B1].x = billalert_dialog[A_B2].x = billalert_dialog[A_B3].x = 
                                       billalert_dialog[0].x + maxlen/2 - len1/2;

   if (buttons == 3) {
      billalert_dialog[b[0]].x = billalert_dialog[0].x + maxlen/2 - len1*3/2 - avg_w;
      billalert_dialog[b[2]].x = billalert_dialog[0].x + maxlen/2 + len1/2 + avg_w;
   }
   else if (buttons == 2) {
      billalert_dialog[b[0]].x = billalert_dialog[0].x + maxlen/2 - len1 - avg_w;
      billalert_dialog[b[1]].x = billalert_dialog[0].x + maxlen/2 + avg_w;
   }

   billalert_dialog[0].h = avg_h*8;
   billalert_dialog[A_S2].y = billalert_dialog[0].y + avg_h*2 +5;
   billalert_dialog[A_S3].y = billalert_dialog[0].y + avg_h*3 +5;
   billalert_dialog[A_S2].h = avg_h;

   billalert_dialog[A_B1].y = billalert_dialog[A_B2].y = billalert_dialog[A_B3].y = 
                                                billalert_dialog[0].y + avg_h*5;

   billalert_dialog[A_B1].h = billalert_dialog[A_B2].h = billalert_dialog[A_B3].h = avg_h*2;

   centre_dialog(billalert_dialog);

   clear_keybuf();

   do {
   } while (mouse_b);

   c = moveable_do_dialog(billalert_dialog, A_B1);

   if (c == A_B1)
      return 1;
   else if (c == A_B2)
      return 2;
   else
      return 3;
}

int billalert(char *s1, char *s2, char *s3, char *b1, char *b2, int c1, int c2)
{
   int ret;

   ret = billalert3(s1, s2, s3, b1, b2, NULL, c1, c2, 0);

   if (ret > 2)
      ret = 2;

   return ret;
}

/* This is used by the scroll bars. */
/*
     y -|
   * *  |
 x * *  |
 * * *  |- basew
   * *  |
     * -|

 Things to notice about this:
        -It's half as long in the directing that it points,
          In the above, it's 3 pixels wide, and 6 tall.
        -Basew the width of the BASE, regardless of the
          direction that the arrow points. 
*/
void _draw_arrow(int x, int y, int basew, int colr, int stl) {
  /* Arrows are drawn using a series of lines, for example,
     in a left arrow, we start our X and Y at the point of the arrow.
     (X, and Y + half of  base height) we move our line position one left,
     and one up each time. We increase the length by two.  This makes
     the line stick out one pixel on top, and one on the bottom each time.
     The rest of the arrows work essentially the same, except in different
     directions. */

 int xp; /* Contains the current line's x position */
 int yp; /* Contains the current line's y position */
 int w, h; /*H is used for L and R pointing arrows, W for U and D
             pointing arrows */
 if(basew % 2 > 0) basew--;
  /* Arrows are always drawn with an even number of pixels in the base. */


 if (stl == SCL_LEFTARROW) {
  h=0; /* The left part of the arrow is 0 pixels high */

  for(xp = x, yp = y+basew/2; h<=basew; xp++, yp--, h+=2) {
   vline(screen, xp, yp, yp+h, colr); 
  }
 }
 else if (stl == SCL_RIGHTARROW) {
  h=basew;
  for(xp = x, yp = y; h>=0; xp++, yp++, h-=2) {
   vline(screen, xp, yp, yp+h, colr); 
  }
 }
 else if (stl == SCL_UPARROW) {
  w=0;
  for(yp = y, xp = x+basew/2; w<=basew; yp++, xp--, w+=2) {
   hline(screen, xp, yp, xp+w, colr); 
  }
 }
 else if (stl == SCL_DOWNARROW) {
  w=basew;
  for(yp = y, xp = x; w>=0; yp++, xp++, w-=2) {
   hline(screen, xp, yp, xp+w, colr); 
  }
 }
}


void bill_fade_bmp(BITMAP *bmp, int clr) {
/* This makes a checkerboard pattern of pixels of a specified color,
   it can be used to achieve a Windows "shudown" like effect */
 int yoff;
 int ysub;
 int xoff;
 int xadd;
 scare_mouse();
 acquire_screen();
 for(yoff=0; yoff<=SCREEN_H; yoff +=2) {
  for(ysub=0; ((yoff-ysub)>=0);ysub++)
   putpixel(bmp, ysub, yoff-ysub, clr);
 }

 for(xoff=0; xoff<=SCREEN_W; xoff +=2) {
  for(xadd=0; ((xoff+xadd)<=SCREEN_W); xadd++)
   putpixel(bmp, xoff+xadd, SCREEN_H-xadd, clr);
 }
 release_screen();
 unscare_mouse();
}

/*  PLEASE, do not use this.  It's not necessary. It wasts processor time.
    It's stupid.  I left it here for the sake of compatability. */    
void bill_slowfade_bmp(BITMAP *bmp, int clr) {
 int yoff;
 int ysub;
 int xoff;
 int xadd;

 for(yoff=0; yoff<=SCREEN_H; yoff +=2) {
 vsync();
  for(ysub=0; ((yoff-ysub)>=0);ysub++)
   putpixel(bmp, ysub, yoff-ysub, clr);
 }

 for(xoff=0; xoff<=SCREEN_W; xoff +=2) {
 vsync();
  for(xadd=0; ((xoff+xadd)<=SCREEN_W); xadd++)
   putpixel(bmp, xoff+xadd, SCREEN_H-xadd, clr);
 }
}

/* Replaces all pixels of color "oc" with pixels of color "nc" */
void replace_col(BITMAP *b, int oc, int nc)
{
 int x;
 int y;
 for (y=0; y<=b->h; y++) {
  for (x=0; x<=b->w; x++) {
   if (getpixel(b, x, y) == oc)
    putpixel(b, x, y, nc);
  }
 }
}

/* Sets a palette range to a specified color */
void make_solid_pal(PALETTE pal, int start, int stop, int r, int g, int b)
{
 int counter;
 RGB color = {r, g, b};
 for (counter=start; counter<=stop; counter++) {
  pal[counter] = color;
 }
}

/*
  ==============================
  =  Stuff that didn't fit in  =
  ==============================
*/
int init_moveable(DIALOG *dialog)
{
 BITMAP *backbuffer = create_bitmap(dialog->w+1, dialog->h+1);
 BILLWIN_INFO_STRUCTURE *infostruct = malloc(sizeof(BILLWIN_INFO_STRUCTURE));

 /* We do NOT need to memverify the above, if they aren't there, the window will
    notice, and disable the moving routine */

 if (backbuffer) {
  scare_mouse();
  blit(screen, backbuffer, dialog->x, dialog->y, 0, 0, dialog->w+1, dialog->h+1);
  unscare_mouse();
 }

 if(infostruct)
  infostruct->backbuffer = backbuffer;
 dialog->dp3 = infostruct;

 return 0;
}

int shutdown_moveable(DIALOG *dialog)
{
 BILLWIN_INFO_STRUCTURE *infostruct = dialog->dp3;
 if (infostruct)
 {
  if(infostruct->backbuffer)
  {
   scare_mouse();
   blit(infostruct->backbuffer, screen, 0, 0, dialog->x, dialog->y, dialog->w+1, dialog->h+1);
   destroy_bitmap(infostruct->backbuffer);
   unscare_mouse();
  }
  free(infostruct);
  dialog->dp3 = NULL;
 }
 return 0;

 return 0;
}

int moveable_do_dialog(DIALOG *dialog, int focus_obj)
{
   int ret;
   init_moveable(dialog);
   ret = do_dialog(dialog, focus_obj);
   shutdown_moveable(dialog);
   return ret;
}

