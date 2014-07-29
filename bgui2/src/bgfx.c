/*
 * BGui version 2.0 unoffical repackage and rerelease by Jason Winnebeck.
 * Original author: Dan Huizenga (Skis)
 *
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "allegro.h"
#include "tdgui.h"
#include "binterna.h"

RGB bill_light = {224, 224, 224};
RGB bill_hlight = {250, 250, 250};
RGB bill_face = {192, 192, 192};
RGB bill_shadow = {128, 128, 128};
RGB bill_dkshadow = {30, 30, 30};
RGB bill_text = {30, 30, 30};
RGB bill_disabledtext = {128, 128, 128};
RGB bill_activetitle = {0, 0, 128};
RGB bill_titletext = {255, 255, 255};
RGB bill_framebg = {255, 255, 255};

PALETTE bill_pal;

int bill_appearexact = FALSE;

//Unselected
static unsigned char radio_unsel_data[144] =
{
 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0,
 0, 0, 4, 4, 5, 5, 5, 5, 4, 4, 0, 0,
 0, 4, 5, 5, 2, 2, 2, 2, 5, 5, 1, 0,
 0, 4, 5, 2, 2, 2, 2, 2, 2, 3, 1, 0,
 4, 5, 2, 2, 2, 2, 2, 2, 2, 2, 3, 1,
 4, 5, 2, 2, 2, 2, 2, 2, 2, 2, 3, 1,
 4, 5, 2, 2, 2, 2, 2, 2, 2, 2, 3, 1,
 4, 5, 2, 2, 2, 2, 2, 2, 2, 2, 3, 1,
 0, 4, 5, 2, 2, 2, 2, 2, 2, 3, 1, 0,
 0, 4, 3, 3, 2, 2, 2, 2, 3, 3, 1, 0,
 0, 0, 1, 1, 3, 3, 3, 3, 1, 1, 0, 0,
 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0
};

//Selected
static unsigned char radio_sel_data[144] =
{
 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0,
 0, 0, 4, 4, 5, 5, 5, 5, 4, 4, 0, 0,
 0, 4, 5, 5, 2, 2, 2, 2, 5, 5, 1, 0,
 0, 4, 5, 2, 2, 2, 2, 2, 2, 3, 1, 0,
 4, 5, 2, 2, 2, 6, 6, 2, 2, 2, 3, 1,
 4, 5, 2, 2, 6, 6, 6, 6, 2, 2, 3, 1,
 4, 5, 2, 2, 6, 6, 6, 6, 2, 2, 3, 1,
 4, 5, 2, 2, 2, 6, 6, 2, 2, 2, 3, 1,
 0, 4, 5, 2, 2, 2, 2, 2, 2, 3, 1, 0,
 0, 4, 3, 3, 2, 2, 2, 2, 3, 3, 1, 0,
 0, 0, 1, 1, 3, 3, 3, 3, 1, 1, 0, 0,
 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0
};

//greyed
static unsigned char radio_grey_data[144] =
{
 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 0, 0,
 0, 0, 4, 4, 5, 5, 5, 5, 4, 4, 0, 0,
 0, 4, 5, 5, 7, 7, 7, 7, 5, 5, 1, 0,
 0, 4, 5, 7, 7, 7, 7, 7, 7, 3, 1, 0,
 4, 5, 7, 7, 7, 7, 7, 7, 7, 7, 3, 1,
 4, 5, 7, 7, 7, 7, 7, 7, 7, 7, 3, 1,
 4, 5, 7, 7, 7, 7, 7, 7, 7, 7, 3, 1,
 4, 5, 7, 7, 7, 7, 7, 7, 7, 7, 3, 1,
 0, 4, 5, 7, 7, 7, 7, 7, 7, 3, 1, 0,
 0, 4, 3, 3, 7, 7, 7, 7, 3, 3, 1, 0,
 0, 0, 1, 1, 3, 3, 3, 3, 1, 1, 0, 0,
 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0
};


void bill_init_radio_buttons()
{
 if(!radio_unsel)               //Assume that the color mode has changed.
  destroy_bitmap(radio_unsel);
 if(!radio_sel)
  destroy_bitmap(radio_sel);
 if(!radio_grey)
  destroy_bitmap(radio_grey);

  radio_unsel = create_bitmap(12, 12);
  radio_sel = create_bitmap(12, 12);
  radio_grey = create_bitmap(12, 12);

  memverify_str(radio_unsel, " \n Radio button ran out of memory \n ");
  memverify_str(radio_sel, " \n Radio button ran out of memory \n ");
  memverify_str(radio_grey, " \n Radio button ran out of memory \n ");

  data2bmp(radio_unsel_data, radio_unsel, 12, 12);
  data2bmp(radio_sel_data,   radio_sel,   12, 12);
  data2bmp(radio_grey_data,  radio_grey,  12, 12);
}

/* ALL of the BillGui routines are based on the tdbox routine.
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
   STL_TITLEBAR   (128)  Window titles and menus.
*/

void tdbox(BITMAP *b, int x, int y, int w, int h, int stl) {
 BITMAP *pattern;
 int tempRGB;
 if (w > 0)
 {
  if (h > 0)
  {
  //Draw the frame
   if(stl & STL_OUT)
   {
    tempRGB = get_bill_color(bill_hlight);    // Only one call to get_bill_color
    hline(b, x, y, x+w-1, tempRGB);
    vline(b, x, y, y+h-1, tempRGB);

    tempRGB = get_bill_color(bill_light);  
    hline(b, x+1, y+1, x+w-2, tempRGB);
    vline(b, x+1, y+1, y+h-2, tempRGB);
  
    tempRGB = get_bill_color(bill_dkshadow);
    hline(b, x, y+h, x+w, tempRGB);
    vline(b, x+w, y, y+h, tempRGB);

    tempRGB = get_bill_color(bill_shadow);
    hline(b, x+1, y+h-1, x+w-1, tempRGB);
    vline(b, x+w-1, y+1, y+h-2, tempRGB);
   }
   else if(stl & STL_IN)
   {
    if (!(stl & STL_BUTTONDOWN))
    {
     tempRGB = get_bill_color(bill_hlight);
     hline(b, x, y+h, x+w, tempRGB);
     vline(b, x+w, y, y+h, tempRGB);

     tempRGB = get_bill_color(bill_light);
     hline(b, x+1, y+h-1, x+w-1, tempRGB);
     vline(b, x+w-1, y+1, y+h-2, tempRGB);
   
     if (stl & STL_FRAME)
     {
      tempRGB = get_bill_color(bill_dkshadow);
      hline(b, x+1, y+1, x+w-1, tempRGB);
      vline(b, x+1, y+1, y+h-1, tempRGB);

      tempRGB = get_bill_color(bill_shadow);
      hline(b, x, y, x+w, tempRGB);
      vline(b, x, y, y+h, tempRGB);
     }
     else {
      tempRGB = get_bill_color(bill_dkshadow);
      hline(b, x, y, x+w, tempRGB);
      vline(b, x, y, y+h, tempRGB);
      tempRGB = get_bill_color(bill_shadow);
      hline(b, x+1, y+1, x+w-1, tempRGB);
      vline(b, x+1, y+1, y+h-1, tempRGB);
     }
    }
    else {
     rect(b, x, y, x+w, y+h, get_bill_color(bill_dkshadow));
     rect(b, x+1, y+1, x+w-1, y+h-1, get_bill_color(bill_shadow));
    }
   }
  
   //The following HAS to be done AFTER THE FRAME!
   if ((stl & STL_DOWN) || (stl & STL_UP))
   {
    x+=2;
    y+=2;
    if(w > 0)
     w-=4;
    else
     w+=4;
    if(h>0)
     h-=4;
    else
     h+=4;
   }
   //Draw the BackGround
   if (stl & STL_FLAT)
    rectfill(b, x, y, x+w, y+h, get_bill_color(bill_face));
   else if(stl & STL_FRAMEFILLED) 
    rectfill(b, x, y, x+w, y+h, get_bill_color(bill_framebg));
   else if(stl & STL_DITHERED)
   {
    pattern = create_bitmap(2, 2);

    tempRGB = get_bill_color(bill_light);
    putpixel(pattern, 0, 1, tempRGB);
    putpixel(pattern, 1, 0, tempRGB);
    tempRGB = get_bill_color(bill_hlight);
    putpixel(pattern, 0, 0, tempRGB);
    putpixel(pattern, 1, 1, tempRGB);
 
    drawing_mode(DRAW_MODE_COPY_PATTERN, pattern, 0, 0);
    rectfill(b, x, y, x+w, y+h, 0);
    solid_mode();
    /*JW: Fixed as per author's site*/
    destroy_bitmap(pattern);
   }
   else if(stl & STL_TITLEBAR)
   {
    rectfill(b, x, y, x+w, y+h, get_bill_color(bill_activetitle));
   }
  }
 }
}

void tdhline(BITMAP *b, int x, int y, int x2)
{
 hline(b, x, y+1, x2, get_bill_color(bill_hlight));
 hline(b, x, y, x2, get_bill_color(bill_shadow));
}

void tdvline(BITMAP *b, int x, int y, int y2)
{
 vline(b, x+1, y, y2, get_bill_color(bill_hlight));
 vline(b, x, y, y2, get_bill_color(bill_shadow));
}


RGB make_8(RGB r)
{
 RGB i;
 i.r = (r.r*63)/255;
 i.g = (r.g*63)/255;
 i.b = (r.b*63)/255;
 return i;
}

void set_color8(int idx, RGB *r)
{
 RGB i;
 RGB b;
 b = *r;
 i = make_8(*r);
 set_color(idx, &i);
}

/*
 All of the BGui colors are gotten with this, in case I
 decide to use some twisted color-mapping scheme for 8-Bit.
 I don't think that I'll have to do this anymore, now that
 I figured out what was wrong with the bill_init_smallpall.
*/
int get_bill_color(RGB bc) {
 return makecol(bc.r, bc.g, bc.b);
}

/*this is a slightly modified version of something
  found in Allegro's mouse.c*/
void data2bmp(unsigned char d[], BITMAP *b, int w, int h)
{
 int pcolor;
 int x, y;
 for (y=0; y<h; y++) {
  for (x=0; x<w; x++) {
   if(d[y*w+x] == 1)
    pcolor = get_bill_color(bill_hlight);
   else if(d[y*w+x] == 2)
    pcolor = get_bill_color(bill_framebg);
   else if(d[y*w+x] == 3)
    pcolor = get_bill_color(bill_light);
   else if(d[y*w+x] == 4)
    pcolor = get_bill_color(bill_shadow);
   else if(d[y*w+x] == 5)
    pcolor = get_bill_color(bill_dkshadow);
   else if(d[y*w+x] == 6)
    pcolor = get_bill_color(bill_dkshadow);
   else if(d[y*w+x] == 7)
    pcolor = get_bill_color(bill_face);
   else
   {
    if (bitmap_color_depth(b) == 8)
     pcolor = 0;
    else 
     pcolor = makecol(255, 0, 255);
   }
   putpixel(b, x, y, pcolor);
  }
 } 
}

/*Sets up a gray palette w/ blue for the title bars*/
void bill_init_pal() {
 RGB cl = { 0, 0, 0 };
 int count;

 for(count = 0; count < 255; count++) {
   cl.r = (63*count)/255;
   cl.g = (63*count)/255;
   cl.b = (63*count)/255;
   set_color(count, &cl);
   bill_pal[count]=cl;
 }
 cl.r = 0;
 cl.g = 0;
 cl.b = 31;
 bill_pal[1]=cl;
 set_color(1, &cl);
 cl.r = 63;
 cl.g = 63;
 cl.b = 63;
 bill_pal[16]=cl;
 set_color(16, &cl);
 data2bmp(radio_unsel_data, radio_unsel, 12, 12);
 data2bmp(radio_sel_data,   radio_sel,   12, 12);
 data2bmp(radio_grey_data,  radio_grey,  12, 12);
}

void bill_init_smallpal(int start)
{
set_color8(start, &bill_light);
set_color8(start+1, &bill_hlight);
set_color8(start+2, &bill_face);
set_color8(start+3, &bill_shadow);
set_color8(start+4, &bill_dkshadow);
set_color8(start+5, &bill_text);
set_color8(start+6, &bill_activetitle);
set_color8(start+7, &bill_titletext);
set_color8(start+8, &bill_framebg);
set_color8(start+9, &bill_disabledtext);
 data2bmp(radio_unsel_data, radio_unsel, 12, 12);
 data2bmp(radio_sel_data,   radio_sel,   12, 12);
 data2bmp(radio_grey_data,  radio_grey,  12, 12);
}

void bill_init_very_smallpal(int start)
{
set_color8(start, &bill_light);
set_color8(start+1, &bill_hlight);
set_color8(start+2, &bill_face);
set_color8(start+3, &bill_shadow);
set_color8(start+4, &bill_dkshadow);
set_color8(start+5, &bill_activetitle);
set_color8(start+6, &bill_titletext);
 data2bmp(radio_unsel_data, radio_unsel, 12, 12);
 data2bmp(radio_sel_data,   radio_sel,   12, 12);
 data2bmp(radio_grey_data,  radio_grey,  12, 12);
}


