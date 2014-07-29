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

BITMAP *radio_unsel;
BITMAP *radio_sel;
BITMAP *radio_grey;

/* JW: Allegro "headers" -- these functions were replaced DIRECTLY with their
   allegro counterparts because they seem not to have an effect -- and the
   Allegro routines are MUCH more portable and stable in the new release.
*/

/*This replaced _draw_billtextbox*/
void _draw_textbox(char *thetext, int *listsize, int draw, int offset,
                   int wword, int tabsize, int x, int y, int w, int h,
                   int disabled, int fore, int deselect, int disable);


/*
  ==============================
  = BILL GUI Proc helpers      =
  ==============================
*/
/* Same as the dotted rect in allegro routines, except that it skips
   every other pixel instead of having a fg/bg pixel
*/
static void bill_dotted_rect(int x1, int y1, int x2, int y2, int fg)
{
   int c;

   for (c=x1; c<=x2; c+=2) {
      putpixel(screen, c, y1, fg);
      putpixel(screen, c, y2, fg);
   }

   for (c=y1; c<y2; c+=2) {
      putpixel(screen, x1, c, fg);
      putpixel(screen, x2, c, fg);
   }
}


void _draw_scrollable_billframe(DIALOG *d, int listsize, int offset, int height, int fg_color, int bg)
{
   int i, len;
   int xx, yy;
   /*scare_mouse();*/ 
   /* draw frame */
   tdbox(screen, d->x, d->y, d->w, d->h, STL_IN + STL_FRAME);

   /* possibly draw scrollbar */
   if (listsize > height) {
      /* create and draw the scrollbar */
      i = ((d->h-4) * height + listsize/2) / listsize;
      xx = d->x+d->w-12;
      yy = d->y+2;


      if (offset > 0) {
        len = (((d->h-4) * offset) + listsize/2) / listsize;
        //tdbox(screen, xx, yy, (d->x+d->w)-xx-2, (d->y+d->h)-yy-2, STL_DITHERED);
        tdbox(screen, xx, yy, (d->x+d->w)-xx-2, len-1, STL_DITHERED);
        yy += len;
      }

      if (yy+i<d->y+d->h-2) {
        tdbox(screen, xx, yy+i, (d->x+d->w)-xx-2, (d->y+d->h)-2-(i+yy), STL_DITHERED);
      }

      if (yy+i < d->y+d->h-2) {
       tdbox(screen, xx, yy, 10, i, STL_OUT + STL_FLAT);
      }
      else {
       tdbox(screen, xx, yy, 10, (d->y+d->h)-yy-2, STL_OUT + STL_FLAT);
      }
      rect(screen, d->x+2, d->y+2, xx-1, d->y+d->h-2, get_bill_color(bill_framebg));
   }
   else {
      rect(screen, d->x+2, d->y+2, d->x+d->w-2, d->y+d->h-2, get_bill_color(bill_framebg));
   }
 /*unscare_mouse();*/
}

/* draw_listbox:
 *  Helper function to draw a listbox object.
 */
void _draw_billlistbox(DIALOG *d)
{
  int height, listsize, i, len, bar, x, y, w;
  int fg_color, fg, bg;
  char *sel = d->dp2;
  char s[1024];
  
  (*(getfuncptr)d->dp)(-1, &listsize);
  height = (d->h-3) / text_height(font);
  bar = (listsize > height);
  w = (bar ? d->w-14 : d->w-2);
  fg_color = (d->flags & D_DISABLED) ? gui_mg_color : get_bill_color(bill_text);
  
  /* draw box contents */
  for (i=0; i<height; i++) {
    if (d->d2+i < listsize) {
      if (d->flags & D_DISABLED) {
        fg = get_bill_color(bill_text);
        bg = get_bill_color(bill_face);
      }
      else if (d->d2+i == d->d1) {
        fg = get_bill_color(bill_framebg);
        bg = get_bill_color(bill_text);
      } 
      else if ((sel) && (sel[d->d2+i])) { 
        fg = get_bill_color(bill_framebg);
        bg = get_bill_color(bill_text);
      }
      else {
        bg = get_bill_color(bill_framebg);
        fg = get_bill_color(bill_text);
      }
      usetc(s, 0);
      ustrncat(s, (*(getfuncptr)d->dp)(i+d->d2, NULL), sizeof(s)-ucwidth(0));
      x = d->x + 2;
      y = d->y + 3 + i*text_height(font);
      text_mode(bg);
      rectfill(screen, x, y, x+7, y+text_height(font)-1, bg); 
      x += 8;
      len = ustrlen(s);
      while (text_length(font, s) >= MAX(d->w - (bar ? 22 : 10), 1)) {
        len--;
        usetat(s, len, 0);
      }
      textout(screen, font, s, x, y, fg); 
      x += text_length(font, s);
      if (x <= d->x+w) 
        rectfill(screen, x, y, d->x+w, y+text_height(font)-1, bg);
    }
    else {
      if (d->flags & D_DISABLED)
        rectfill(screen, d->x+2,  d->y+2+i*text_height(font), 
        d->x+w, d->y+1+(i+1)*text_height(font), get_bill_color(bill_face));
      else
        rectfill(screen, d->x+2,  d->y+2+i*text_height(font), 
        d->x+w, d->y+1+(i+1)*text_height(font), get_bill_color(bill_framebg));
    }
  }
  
  if (d->y+2+i*text_height(font) <= d->y+d->h-2)
    if (d->flags & D_DISABLED)
      rectfill(screen, d->x+2, d->y+2+i*text_height(font), 
      d->x+w, d->y+d->h-2, get_bill_color(bill_face));
    else
      rectfill(screen, d->x+2, d->y+2+i*text_height(font), 
      d->x+w, d->y+d->h-2, get_bill_color(bill_framebg));
    
    /* draw frame, maybe with scrollbar */
    if (d->flags & D_DISABLED)
      tdbox(screen, d->x, d->y, d->w, d->h, STL_IN + STL_FRAME);
    else
      _draw_scrollable_billframe(d, listsize, d->d2, height, fg_color, get_bill_color(bill_framebg));
}

/*JW: completely removed _draw_billtextbox because it was unnessacary, and
  instead called Allegro's _draw_textbox.*/

/*
  ==============================
  = BILL GUI Procs             =
  ==============================
*/

/* d_billmenu_proc is in billmenu.c */

int d_billprogress_proc(int msg, DIALOG *d, int c)
{
 int vert = FALSE;
 float percent;
 float barlength;

 if(msg==MSG_DRAW)
 {
  scare_mouse();
  if (d->d1 <= 0) d->d1 = 1;
  if (d->d2 > d->d1) d->d2 = d->d1;
  percent = (float)d->d2/(float)d->d1;
  if (d->h > d->w) vert = TRUE;
  percent *= 100;
  if (percent > 100) percent = 100;
  if (percent < 0) percent = 0;

  if (!vert)
   barlength = ((float)(d->w - 4)/100.0) * percent;
  else
   barlength = ((float)(d->h - 4)/100.0) * percent;

  tdbox(screen, d->x, d->y, d->w, d->h, STL_DOWN);
  if(!vert)
  {
   tdbox(screen, d->x+2, d->y+2, barlength, d->h-4, STL_TITLEBAR);
   tdbox(screen, d->x+2+barlength, d->y+2, d->w-barlength-4, d->h-4, STL_FACE);
  }
  else
  {
   tdbox(screen, d->x+2, d->y+((d->h-4)-barlength)+2, d->w-4, barlength, STL_TITLEBAR);
   tdbox(screen, d->x+2, d->y+2, d->w-4, ((d->h-4)-barlength), STL_FACE);
  }
  unscare_mouse();
 }
 return D_O_K;
}

int d_billscrollablebitmap_proc(int msg, DIALOG *d, int c)
{
   int ret;
   BITMAP *b;
   int (*proc)(int *d1value, int *d2value);

   if (d->dp2)
   {
    proc = d->dp2;
    ret = (*proc)(&d->d1, &d->d2);
   }

   if (ret == D_REDRAW)
    msg = MSG_DRAW;

   if (msg==MSG_DRAW)
    if(d->dp)
    {
     scare_mouse();
     b = (BITMAP *)d->dp;
     blit(b, screen, d->d1, d->d2, d->x, d->y, d->w, d->h);
     unscare_mouse();
    }
   return D_O_K;
}

int d_hbillscrollbar_proc(int msg, DIALOG *d, int c)
{
 int retval = D_O_K;
 float rngl;
 float rngr;
 float rngw;
 int newval;
 int msx, msy;
 int objd1 = d->d1;
 int lastclock = 0;
 int hpos;
 int tmp = 0;
 void (*proc)(int d2value);

 rngl = d->x+d->h+1;
 rngr = d->x+d->w-(d->h*2)-1;
 rngw = rngr - rngl;

 if (msg==MSG_DRAW)
 {
  if (d->d1)
  {
  //Left buttun w/ arrow
   scare_mouse();

   if (d->flags & SCL_DOWNBUTTON_DOWN)
    tdbox(screen, d->x, d->y, d->h, d->h, STL_DOWN + STL_FACE);
   else
    tdbox(screen, d->x, d->y, d->h, d->h, STL_UP + STL_FACE);
   _draw_arrow(d->x+4, d->y+3, d->h-6, get_bill_color(bill_text), SCL_LEFTARROW);
   //Handle
 
   hpos = rngl + ((d->d2*rngw)/objd1);
 
   tdbox(screen, hpos, d->y, d->h, d->h, STL_UP + STL_FACE);
   //tdbox(screen, rngl +20, d->y, d->h, d->h, STL_UP + STL_FACE);
   //Dithered(Left)part
   tdbox(screen, rngl, d->y, hpos-(rngl+1), d->h, STL_DITHERED);
   //Dithered(right)part
   tdbox(screen, hpos+d->h+1, d->y, rngr+d->h-(hpos+d->h), d->h, STL_DITHERED);
   //Rbuttun w/ arrow
   if (d->flags & SCL_UPBUTTON_DOWN)
    tdbox(screen, d->x+d->w-d->h, d->y, d->h, d->h, STL_DOWN + STL_FACE);
   else
    tdbox(screen, d->x+d->w-d->h, d->y, d->h, d->h, STL_UP + STL_FACE);
   _draw_arrow(d->x+d->w-2-d->h/2, d->y+3, d->h-6, get_bill_color(bill_text), SCL_RIGHTARROW);
   unscare_mouse();
  }
 }
 else if(msg == MSG_CLICK)
 {
  if (d->d1)
  {
   msx = gui_mouse_x();
   msy = gui_mouse_y();
   /* Check for mouse button */
   if (gui_mouse_b()) {
    /*Check for Left Arrow*/
    if (is_in(msx, msy, d->x, d->y, d->h, d->h))
    {
     /* Mark the arrow as down */
     d->flags += SCL_DOWNBUTTON_DOWN;
     /* Loop until the mouse button is released*/
     while (gui_mouse_b()) {
      /* Keep everyting animating nicely */
      broadcast_dialog_message(MSG_IDLE, 0);
      /* This copies the mouse position into a couple of
         variabls for me, so it won't change half way
         through this.
       */
      msx = gui_mouse_x();
      msy = gui_mouse_y();
      /* Only move the slider if the mouse is still over the
         button
      */
      if (is_in(msx, msy, d->x, d->y, d->h, d->h))
      {
       /* Mark button as down if it is up*/
       if(!(d->flags & SCL_DOWNBUTTON_DOWN))
        d->flags += SCL_DOWNBUTTON_DOWN;
       /* Only move the slider after 5 clock ticks have gone by, this
          keeps it from jumping from one end to another
       */
       if( ((clock() - lastclock) > 5) || ((clock() - lastclock) < 0) )
       {
        d->d2--;
        if (d->d2 < 0) d->d2 = 0;
        scare_mouse();
        acquire_screen();
        SEND_MESSAGE(d, MSG_DRAW, 0);
        release_screen();
        unscare_mouse();
        lastclock = clock();
        if (d->dp) {
         proc = d->dp;
         (*proc)(d->d2);
        }
       }
      }
      else
      {
      /*If the mouse isn't in the button, mark it as up*/
       if(d->flags & SCL_DOWNBUTTON_DOWN)
       {
        d->flags -= SCL_DOWNBUTTON_DOWN;
        scare_mouse();
        acquire_screen();
        SEND_MESSAGE(d, MSG_DRAW, 0);
        release_screen();
        unscare_mouse();
       }
      }
     }
     /* if the button is still down, put it up */
     if(d->flags & SCL_DOWNBUTTON_DOWN)
     {
      d->flags -= SCL_DOWNBUTTON_DOWN;
      scare_mouse();
      acquire_screen();
      SEND_MESSAGE(d, MSG_DRAW, 0);
      release_screen();
      unscare_mouse();
     }
    }
  
    /*Check for Right Arrow*/
    else if (is_in(msx, msy, d->x+d->w-d->h, d->y, d->h, d->h))
    {
     d->flags += SCL_UPBUTTON_DOWN;
     while (gui_mouse_b())
     {
      broadcast_dialog_message(MSG_IDLE, 0);
      msx = gui_mouse_x();
      msy = gui_mouse_y();
      if (is_in(msx, msy, d->x+d->w-d->h, d->y, d->h, d->h))
      {
       if(!(d->flags & SCL_UPBUTTON_DOWN))
        d->flags += SCL_UPBUTTON_DOWN;
       if( ((clock() - lastclock) > 5) || ((clock() - lastclock) < 0) )
       {
        d->d2++;
        if (d->d2 > d->d1) d->d2 = d->d1;
        scare_mouse();
        acquire_screen();
        SEND_MESSAGE(d, MSG_DRAW, 0);
        release_screen();
        unscare_mouse();
        lastclock = clock();
        if (d->dp) {
         proc = d->dp;
         (*proc)(d->d2);
        }
       }
      }
      else
      {
      /*If the mouse isn't in the button, mark it as up*/
       if(d->flags & SCL_UPBUTTON_DOWN)
       {
        d->flags -= SCL_UPBUTTON_DOWN;
        scare_mouse();
        acquire_screen();
        SEND_MESSAGE(d, MSG_DRAW, 0);
        release_screen();
        unscare_mouse();
       }
      }
     }
     if(d->flags & SCL_UPBUTTON_DOWN)
     {
      d->flags -= SCL_UPBUTTON_DOWN;
      scare_mouse();
      acquire_screen();
      SEND_MESSAGE(d, MSG_DRAW, 0);
      release_screen();
      unscare_mouse();
     }
    }
    /*Check for other..*/
    else if (is_in(msx, msy, d->x+d->h, d->y, d->w-d->h*2, d->h))
    {
     while(gui_mouse_b())
     {
      msx = gui_mouse_x();
      msy = gui_mouse_y();
      hpos = msx;
      newval = (objd1*(hpos-rngl))/rngw;
      if (newval < 0) newval = 0;
      if (newval > objd1) newval = objd1;
      d->d2 = newval;
      if (newval != tmp)
      {
       scare_mouse();
       acquire_screen();
       SEND_MESSAGE(d, MSG_DRAW, 0);
       release_screen();
       unscare_mouse();
       tmp = newval;
       if (d->dp) {
        proc = d->dp;
        (*proc)(d->d2);
       }
      }
     }
    }
   }
   else {
    if (d->flags & SCL_UPBUTTON_DOWN) {
     d->flags -= SCL_UPBUTTON_DOWN;
     scare_mouse();
     acquire_screen();
     SEND_MESSAGE(d, MSG_DRAW, 0);
     release_screen();
     unscare_mouse();
    }
    if (d->flags & SCL_DOWNBUTTON_DOWN) {
      d->flags -= SCL_DOWNBUTTON_DOWN;
      scare_mouse();
      acquire_screen();
      SEND_MESSAGE(d, MSG_DRAW, 0);
      release_screen();
      unscare_mouse();
    }
   }
  
   if (d->d2 < 0) d->d2 = 0;
   if (d->d2 > d->d1) d->d2 = d->d1;
   scare_mouse();
   acquire_screen();
   SEND_MESSAGE(d, MSG_DRAW, 0);
   release_screen();
   unscare_mouse();
  }
 }
 else if(msg == MSG_IDLE)
 {
  if (!(gui_mouse_b()))
  {
   if (d->flags & SCL_UPBUTTON_DOWN) {
    d->flags -= SCL_UPBUTTON_DOWN;
    scare_mouse();
    acquire_screen();
    SEND_MESSAGE(d, MSG_DRAW, 0);
    release_screen();
    unscare_mouse();
   }
   if (d->flags & SCL_DOWNBUTTON_DOWN) {
    d->flags -= SCL_DOWNBUTTON_DOWN;
    scare_mouse();
    acquire_screen();
    SEND_MESSAGE(d, MSG_DRAW, 0);
    release_screen();
    unscare_mouse();
   }
  }
 }
 //if (redraw == TRUE)
 return retval;
}

int d_vbillscrollbar_proc(int msg, DIALOG *d, int c)
{
 int retval = D_O_K;
 float rngt;
 float rngb;
 float rngh;
 int newval;
 int msx, msy;
 int objd1 = d->d1;
 int lastclock = 0;
 int hpos;
 int tmp = 0;
 void (*proc)(int d2value);

 rngt = d->y+d->w+1;
 rngb = d->y+d->h-(d->w*2)-1;
 rngh = rngb - rngt;

 if (msg==MSG_DRAW)
 {
  if (d->d1)
  {
   //Left buttun w/ arrow
   scare_mouse();
 
   if (d->flags & SCL_DOWNBUTTON_DOWN)
    tdbox(screen, d->x, d->y, d->w, d->w, STL_DOWN + STL_FACE);
   else
    tdbox(screen, d->x, d->y, d->w, d->w, STL_UP + STL_FACE);
   _draw_arrow(d->x+3, d->y+4, d->w-6, get_bill_color(bill_text), SCL_UPARROW);
   //Handle
 
   hpos = rngt + ((d->d2*rngh)/objd1);
 
   tdbox(screen, d->x, hpos, d->w, d->w, STL_UP + STL_FACE);
   //tdbox(screen, rngl +20, d->y, d->h, d->h, STL_UP + STL_FACE);
   //Dithered(Left)part
   tdbox(screen, d->x, rngt, d->w, hpos-(rngt+1), STL_DITHERED);
   //Dithered(right)part
   tdbox(screen, d->x, hpos+d->w+1, d->w, rngb-hpos, STL_DITHERED);
   //Rbuttun w/ arrow
   if (d->flags & SCL_UPBUTTON_DOWN)
    tdbox(screen, d->x, d->y+d->h-d->w, d->w, d->w, STL_DOWN + STL_FACE);
   else
    tdbox(screen, d->x, d->y+d->h-d->w, d->w, d->w, STL_UP + STL_FACE);
   _draw_arrow (d->x+3, d->y+d->h-2-d->w/2, d->w-6, get_bill_color(bill_text), SCL_DOWNARROW);
   unscare_mouse();
  }
 }
 else if(msg == MSG_CLICK)
 {
  if (d->d1)
  {
   msx = gui_mouse_x();
   msy = gui_mouse_y();
   /* Check for mouse button */
   if (gui_mouse_b()) {
    /*Check for Left Arrow*/
    if (is_in(msx, msy, d->x, d->y, d->w, d->w))
    {
     /* Mark the arrow as down */
     d->flags += SCL_DOWNBUTTON_DOWN;
     /* Loop until the mouse button is released*/
     while (gui_mouse_b()) {
      /* Keep everyting animating nicely */
      broadcast_dialog_message(MSG_IDLE, 0);
      /* This copies the mouse position into a couple of
         variabls for me, so it won't change half way
         through this.
       */
      msx = gui_mouse_x();
      msy = gui_mouse_y();
      /* Only move the slider if the mouse is still over the
         button
      */
      if (is_in(msx, msy, d->x, d->y, d->w, d->w))
      {
       /* Mark button as down if it is up*/
       if(!(d->flags & SCL_DOWNBUTTON_DOWN))
        d->flags += SCL_DOWNBUTTON_DOWN;
       /* Only move the slider after 5 clock ticks have gone by, this
          keeps it from jumping from one end to another
       */
       if( ((clock() - lastclock) > 5) || ((clock() - lastclock) < 0) )
       {
        d->d2--;
        if (d->d2 < 0) d->d2 = 0;
        scare_mouse();
        SEND_MESSAGE(d, MSG_DRAW, 0);
        unscare_mouse();
        lastclock = clock();
        if (d->dp) {
         proc = d->dp;
         (*proc)(d->d2);
        }
       }
      }
      else
      {
      /*If the mouse isn't in the button, mark it as up*/
       if(d->flags & SCL_DOWNBUTTON_DOWN)
       {
        d->flags -= SCL_DOWNBUTTON_DOWN;
        scare_mouse();
        SEND_MESSAGE(d, MSG_DRAW, 0);
        unscare_mouse();
       }
      }
     }
     /* if the button is still down, put it up */
     if(d->flags & SCL_DOWNBUTTON_DOWN)
     {
      d->flags -= SCL_DOWNBUTTON_DOWN;
      scare_mouse();
      SEND_MESSAGE(d, MSG_DRAW, 0);
      unscare_mouse();
     }
    }
 
    /*Check for Right Arrow*/
    else if (is_in(msx, msy, d->x, d->y+d->h-d->w, d->w, d->w))
    {
     d->flags += SCL_UPBUTTON_DOWN;
     while (gui_mouse_b())
     {
      broadcast_dialog_message(MSG_IDLE, 0);
      msx = gui_mouse_x();
      msy = gui_mouse_y();
      if (is_in(msx, msy, d->x, d->y+d->h-d->w, d->w, d->w))
      {
       if(!(d->flags & SCL_UPBUTTON_DOWN))
        d->flags += SCL_UPBUTTON_DOWN;
       if( ((clock() - lastclock) > 5) || ((clock() - lastclock) < 0) )
       {
        d->d2++;
        if (d->d2 > d->d1) d->d2 = d->d1;
        scare_mouse();
        SEND_MESSAGE(d, MSG_DRAW, 0);
        unscare_mouse();
        lastclock = clock();
        if (d->dp) {
         proc = d->dp;
         (*proc)(d->d2);
        }
       }
      }
      else
      {
      /*If the mouse isn't in the button, mark it as up*/
       if(d->flags & SCL_UPBUTTON_DOWN)
       {
        d->flags -= SCL_UPBUTTON_DOWN;
        scare_mouse();
        SEND_MESSAGE(d, MSG_DRAW, 0);
        unscare_mouse();
       }
      }
     }
     if(d->flags & SCL_UPBUTTON_DOWN)
     {
      d->flags -= SCL_UPBUTTON_DOWN;
      scare_mouse();
      SEND_MESSAGE(d, MSG_DRAW, 0);
      unscare_mouse();
     }
    }
    /*Check for other..*/
    else if (is_in(msx, msy, d->x, d->y+d->w, d->w, d->h-d->w*2))
    {
     while(gui_mouse_b())
     {
      msx = gui_mouse_x();
      msy = gui_mouse_y();
      hpos = msy;
      newval = (objd1*(hpos-rngt))/rngh;
      if (newval < 0) newval = 0;
      if (newval > objd1) newval = objd1;
      d->d2 = newval;
      if (newval != tmp)
      {
       scare_mouse();
       SEND_MESSAGE(d, MSG_DRAW, 0);
       unscare_mouse();
       tmp = newval;
       if (d->dp) {
        proc = d->dp;
        (*proc)(d->d2);
       }
      }
     }
    }
   }
   else {
    if (d->flags & SCL_UPBUTTON_DOWN) {
     d->flags -= SCL_UPBUTTON_DOWN;
     scare_mouse();
     SEND_MESSAGE(d, MSG_DRAW, 0);
     unscare_mouse();
    }
    if (d->flags & SCL_DOWNBUTTON_DOWN) {
      d->flags -= SCL_DOWNBUTTON_DOWN;
      scare_mouse();
      SEND_MESSAGE(d, MSG_DRAW, 0);
      unscare_mouse();
    }
   }
 
   if (d->d2 < 0) d->d2 = 0;
   if (d->d2 > d->d1) d->d2 = d->d1;
   scare_mouse();
   SEND_MESSAGE(d, MSG_DRAW, 0);
   unscare_mouse();
  }
 }
 else if(msg == MSG_IDLE)
 {
  if (!(gui_mouse_b()))
  {
   if (d->flags & SCL_UPBUTTON_DOWN) {
    d->flags -= SCL_UPBUTTON_DOWN;
    scare_mouse();
    SEND_MESSAGE(d, MSG_DRAW, 0);
    unscare_mouse();
   }
   if (d->flags & SCL_DOWNBUTTON_DOWN) {
    d->flags -= SCL_DOWNBUTTON_DOWN;
    scare_mouse();
    SEND_MESSAGE(d, MSG_DRAW, 0);
    unscare_mouse();
   }
  }
 }
 //if (redraw == TRUE)
 return retval;
}

int d_billtextbox_proc(int msg, DIALOG *d, int c)
{
   int height, bar;
   int fg_color = get_bill_color(bill_text);

   /* calculate the actual height */
   height = (d->h-4) / text_height(font);

   if(d->d2 > d->d1)     /* Don't go past end of text */
    d->d2 = d->d1;

   if(msg == MSG_DRAW) {
	 /* tell the object to sort of draw, but only calculate the listsize */
         _draw_textbox(d->dp, &d->d1, 
		       0, /* DONT DRAW anything */
		       d->d2, !(d->flags & D_SELECTED), 8,
		       d->x, d->y, d->w, d->h,
		       (d->flags & D_DISABLED),
		       0, 0, 0);

	 if (d->d1 > height) {
	    bar = 12;
	 }
	 else {
	    bar = 0;
	    d->d2 = 0;
	 }

	 /* now do the actual drawing */
         _draw_textbox(d->dp, &d->d1, 1, d->d2,
		       !(d->flags & D_SELECTED), 8,
		       d->x, d->y, d->w-bar-1, d->h,
		       (d->flags & D_DISABLED),
                       fg_color, get_bill_color(bill_framebg), get_bill_color(bill_framebg));

	 /* draw the frame around */
         _draw_scrollable_billframe(d, d->d1, d->d2, height, fg_color, d->bg);
         return D_O_K;
   }
   else
    return d_textbox_proc(msg, d, c);
}


int d_billedit_proc(int msg, DIALOG *d, int c)
{
  int f, l, p, w, x, fg;
  int b; 
  int scroll; 
  char *s;
  char buf[2];
  
  if(msg == MSG_DRAW) {
    s = d->dp;
    l = strlen(s);
    if (d->d2 > l) 
      d->d2 = l;
    
    /* calculate maximal number of displayable characters */
    b = x = 0;
    if (d->d2 == l)  {
      buf[0] = ' ';
      buf[1] = 0;
      x = text_length(font, buf);
    }
    
    buf[1] = 0;
    for (p=d->d2; p>=0; p--) {
      buf[0] = s[p];
      b++;
      x += text_length(font, buf);
      if (x > d->w) 
        break;
    }
    
    if (x <= d->w) {
      b = l; 
      scroll = FALSE;
    }
    else {
      b--; 
      scroll = TRUE;
    }
    
    /* Actual drawing begins here*/
    scare_mouse();
    fg = get_bill_color(bill_text);
    x = 0;
    
    if (scroll) {
      p = d->d2-b+1; 
      b = d->d2; 
    }
    else 
      p = 0; 
    
    if (d->flags & D_DISABLED)
      tdbox(screen, d->x+2, d->y+2, d->w-4, d->h-4, STL_FLAT);
    else
      tdbox(screen, d->x+2, d->y+2, d->w-4, d->h-4, STL_FRAMEFILLED);
    for (; p<=b; p++) {
      buf[0] = s[p] ? s[p] : ' ';
      w = text_length(font, buf);
      if (x+w > d->w-4) 
        break;

      f = ((p == d->d2) && (d->flags & D_GOTFOCUS));
      if (d->flags & D_DISABLED)
        text_mode(get_bill_color(bill_face));
      else
        text_mode(f ? fg : get_bill_color(bill_framebg));
      textout(screen, font, buf, d->x+x+3, d->y+3, f ? get_bill_color(bill_framebg) : fg);
      x += w;
    }
    
    tdbox(screen, d->x, d->y, d->w, d->h, STL_IN+STL_FRAME);
    unscare_mouse();
    return D_O_K;
  }
  else
    return d_edit_proc(msg, d, c);
}

int d_billtext_proc(int msg, DIALOG *d, int c)
{
 int fg = get_bill_color(bill_text);
 if (msg==MSG_DRAW) {
  scare_mouse();
  if (d->fg == BILL_TITLE)
   fg = get_bill_color(bill_titletext);
  else
   fg = get_bill_color(bill_text);

  if (d->bg == BILL_CLEAR)
   text_mode(-1);
  else if (d->bg == BILL_TITLE)
   text_mode(get_bill_color(bill_activetitle));
  else
   text_mode(get_bill_color(bill_face));

  gui_textout(screen, d->dp, d->x, d->y, fg, FALSE);
  unscare_mouse();
 }
 return D_O_K;
}


int d_billctext_proc(int msg, DIALOG *d, int c)
{
 int fg = get_bill_color(bill_text);
 if (msg==MSG_DRAW) {
  scare_mouse();
  if (d->fg == BILL_TITLE)
   fg = get_bill_color(bill_titletext);
  else
   fg = get_bill_color(bill_text);

  if (d->bg == BILL_CLEAR)
   text_mode(-1);
  else if (d->bg == BILL_TITLE)
   text_mode(get_bill_color(bill_activetitle));
  else
   text_mode(get_bill_color(bill_face));

  gui_textout(screen, d->dp, d->x, d->y, fg, TRUE);
  unscare_mouse();
 }
 return D_O_K;
}


int d_billcheck_proc(int msg, DIALOG *d, int c)
{
 int x;
 int fg;

 if (msg==MSG_DRAW)
 {
  scare_mouse();

  if(d->flags & D_DISABLED)
   fg = get_bill_color(bill_disabledtext);
  else
   fg = get_bill_color(bill_text);


  text_mode(get_bill_color(bill_face));

  if ( d->d1 )   /* Check is to the right */
  {
   x = d->x +
       gui_textout(screen, d->dp, d->x, d->y+(d->h-(text_height(font)-gui_font_baseline))/2, fg, FALSE) +
       text_height(font)/2;
  } 
  else   /* Check is to the left */
  {
   x = d->x;
   gui_textout(screen, d->dp, d->x + d->h + text_height(font)/2,d->y+(d->h-(text_height(font)-gui_font_baseline))/2, fg, FALSE);
  }

  tdbox(screen, x, d->y, d->h, d->h, STL_IN + STL_FRAME + (fg == get_bill_color(bill_disabledtext) ? STL_FACE : STL_FRAMEFILLED));

  if (d->flags & D_SELECTED) {
   line(screen, x+2, d->y+2, x+d->h-2, d->y+d->h-2, fg);
   line(screen, x+2, d->y+d->h-2, x+d->h-2, d->y+2, fg); 
  }

  unscare_mouse();
  return D_O_K;
 }
 else
  return d_button_proc(msg, d, c);
}


int d_billslider_proc(int msg, DIALOG *d, int c)
{
   BITMAP *slhan = NULL;
   int sfg;                /* slider foreground color */
   int vert = TRUE;        /* flag: is slider vertical? */
   int hh = 10;             /* handle height (width for horizontal sliders) */
   int hmar;               /* handle margin */
   int slp;                /* slider position */
   int irange;
   int slx, sly, slh, slw;
   int ret;
   fixed slratio, slmax, slpos;


   if (msg == MSG_DRAW) {
      /* check for slider direction */
      if (d->h < d->w)
       vert = FALSE;

     /* set up the metrics for the control */
     if (d->dp != NULL) {
      slhan = (BITMAP *)d->dp;
      if (vert)
       hh = slhan->h;
      else
       hh = slhan->w;
     }

         hmar = hh/2;
         irange = (vert) ? d->h : d->w;
         slmax = itofix(irange-hh);
         slratio = slmax / (d->d1);
         slpos = slratio * d->d2;
         slp = fixtoi(slpos);

	 sfg = (d->flags & D_DISABLED) ? gui_mg_color : d->fg;
         
         scare_mouse();

	 if (vert) {
            rectfill(screen, d->x, d->y, d->x+d->w, d->y+d->h, get_bill_color(bill_face));
            tdbox(screen, d->x+d->w/2-2, d->y, 3, d->h, STL_IN + STL_FRAME + STL_FLAT);
	 }
	 else {
            rectfill(screen, d->x, d->y, d->x+d->w, d->y+d->h, get_bill_color(bill_face));
            tdbox(screen, d->x, d->y+d->h/2-2, d->w, 3, STL_IN + STL_FRAME + STL_FLAT);
	 }

	 /* okay, background and slot are drawn, now draw the handle */
	 if (slhan) {
	    if (vert) {
	       slx = d->x+(d->w/2)-(slhan->w/2);
	       sly = d->y+d->h-(hh+slp);
	    } 
	    else {
	       slx = d->x+slp;
	       sly = d->y+(d->h/2)-(slhan->h/2);
	    }
	    draw_sprite(screen, slhan, slx, sly);
	 } 
	 else {
	    /* draw default handle */
	    if (vert) {
	       slx = d->x;
	       sly = d->y+d->h-(hh+slp);
	       slw = d->w;
	       slh = hh;
              /* draw body */
              tdbox(screen, slx, sly+2, (slw), slh-2, STL_OUT + STL_FACE);
	    } else {
	       slx = d->x+slp;
	       sly = d->y;
	       slw = hh;
	       slh = d->h;
              /* draw body */
              tdbox(screen, slx+2, sly, (slw-2), slh, STL_OUT + STL_FACE);
	    }

	 }
         unscare_mouse();
      //   if (d->flags & D_GOTFOCUS)
      //      dotted_rect(d->x, d->y, d->x+d->w, d->y+d->h, sfg, d->bg);
   ret=D_O_K;
   }
   else {
    ret = d_slider_proc(msg, d, c);
   }
 return ret;
}

int d_billwin_proc(int msg, DIALOG *d, int c)
{
 int msx;
 int msy;
 int ymv, xmv;
 int tempx, tempy;
 /* This stores a copy of the screen without the window that is being
    moved showing */
 BITMAP *fullscreen = NULL;
 /* This stores a copy of the screen pointer (the routine makes "screen"
    point to the double buffer in buffered mode) */
 BITMAP *screenptr = screen;
 /* this stores a copy of the window, only updated during move operations if
    the user sets the flag */
 BITMAP *window_bitmap = NULL;

 int tbx, tby, tbw, tbh, texty; /* title bar positions */

 /* contains backbuffer, mousedown positions*/
 BILLWIN_INFO_STRUCTURE *myinfo = d->dp3;

 /* The following variables are used to hit-test and draw the titlebar */
 tbx = d->x+3; /* The title bar's X*/
 tby = d->y+3;
 tbw = d->w-6;
 tbh = text_height(font)+4;
 texty = 5;

 if (msg==MSG_DRAW) {
  tdbox(screen, d->x, d->y, d->w, d->h, STL_OUT + STL_FLAT); /* Frame + BG */
 
  if (d->dp && strlen(d->dp))  /* Short-Circut evaluation keeps the strlen(d->dp from ever evaluating NULL :-) */
  {
   text_mode(-1);
   tdbox(screen, tbx, tby, tbw, tbh, STL_TITLEBAR);
   gui_textout(screen, d->dp, d->x+4, d->y+texty, get_bill_color(bill_titletext), FALSE);
  }
 }
 else if (msg==MSG_CLICK)
 {
  broadcast_dialog_message(MSG_IDLE, 0);
  if (d->dp && strlen(d->dp))
  {
   if (myinfo && myinfo->backbuffer) /* We NEED the info structure and the BB */
   {
    /* "msx" is used to store the mouses current x, so that it will not change
     *while we are busy doing things.  It's initialized for the first time here
     */
    msx = gui_mouse_x(); 
    msy = gui_mouse_y();
    if (is_in(msx, msy, tbx, tby, tbw, tbh+1) && (gui_mouse_b()))
    {
     scare_mouse();
     /* Store the position of the pointer on window, to move based on it */
     myinfo->mdx = gui_mouse_x() - d->x;
     myinfo->mdy = gui_mouse_y() - d->y;
     /* this is used to store the screen, without the dialog on it */
     fullscreen = create_bitmap(SCREEN_W, SCREEN_H);
     /* If we'll be using a double buffer, create it now */
     window_bitmap = (d->d1==BW_BUFFERED || d->d1==BW_BUFFEREDUPDATE) ?
                                    create_bitmap(d->w+1, d->h+1) : NULL;
                                  /*Create on true         set null on flase*/

     /* Fullscreen is the only bitmap that we *really need* to move a window,
        the others are optional, so we don't check them */
     memverify(fullscreen);

     if(fullscreen)
     {
      /* We copy the screen to the buffer here, but it's not a complete copy.
         We skip the part with the window in it, and fill that in using
         the window's backbuffer */

      acquire_screen();
      /*Copy parts of the screen surrounding the dialog: top, rgt, btm, + lft */
      blit(screen, fullscreen, 0, 0, 0, 0, d->x, SCREEN_H);
      blit(screen, fullscreen, d->x, 0, d->x, 0, d->w+2, d->y);
      blit(screen, fullscreen, d->x + d->w + 1, 0, d->x + d->w + 1, 0, SCREEN_W - d->x - d->w - 1, SCREEN_H);
      blit(screen, fullscreen, d->x, d->y + d->h + 1, d->x, d->y + d->h + 1, d->w+1, SCREEN_H - d->y - d->h - 1);

      if(window_bitmap) /* As long as we have the screen, grab a copy of the window */
       blit(screen, window_bitmap, d->x, d->y, 0, 0, d->w+1, d->h+1);       
      release_screen();

      /* Now we copy the window's backbuffer into our image, on top of
         where the window is, and we have a complete screen */
      blit(myinfo->backbuffer, fullscreen, 0, 0, d->x, d->y, d->w+1, d->h+1);
      while (gui_mouse_b())
      {
       msx = gui_mouse_x();
       msy = gui_mouse_y();
       xmv = (msx - d->x) - myinfo->mdx;
       ymv = (msy - d->y) - myinfo->mdy;
       if (xmv || ymv)  /*JW: only if the window has moved*/
       {

        position_dialog(d, d->x + xmv, d->y + ymv); /* move the dialog */

        acquire_screen();
        /* We use sectional blitting here, and although it requires blitting
           more pixels than just erasing the dialog, it reduces flicer, because
           the dialog is *never erased*, we just blit background where the
           dialog used to be, not where it is right now. */

        /*JW: Hmm maybe dirty-rectangle-like-method would work better? The
              ifs aren't NESSACARY really because blit will clip to nothing
              but we save a function/DLL call if we do. The only flaw this
              has now that a box of size xmv wide and ymv tall will be
              blitted twice but you need a lot more ifs, plus this box will
              only be maybe 1-6 pixels max each width. */
        if (xmv > 0)
          /* left side strip blit */
          blit (fullscreen, screen, d->x - xmv, d->y - ymv, d->x - xmv, d->y - ymv, xmv, d->h + 1);
        else
          /* right side strip blit */
          blit (fullscreen, screen, d->x + d->w, d->y - ymv, d->x + d->w, d->y - ymv, -xmv + 1, d->h + 1);

        if (ymv > 0)
          /* top side strip blit */
          blit (fullscreen, screen, d->x - xmv, d->y - ymv, d->x - xmv, d->y - ymv, d->w + 1, ymv);
        else
          /* bottom side strip blit */
          blit (fullscreen, screen, d->x - xmv, d->y + d->h + 1, d->x - xmv, d->y + d->h + 1, d->w + 1, -ymv);

//        blit(fullscreen, screen, 0, 0, 0, 0, d->x, SCREEN_H);
//        blit(fullscreen, screen, d->x, 0, d->x, 0, d->w+2, d->y);
//        blit(fullscreen, screen, d->x + d->w + 1, 0, d->x + d->w + 1, 0, SCREEN_W - d->x - d->w - 1, SCREEN_H);
//        blit(fullscreen, screen, d->x, d->y + d->h + 1, d->x, d->y + d->h + 1, d->w+1, SCREEN_H - d->y - d->h - 1);
        release_screen();

        /* And now, we draw the dialog, using the user's settings */
        if(d->d1==BW_BUFFEREDUPDATE)
        {
         tempx = d->x;
         tempy = d->y;
         position_dialog(d, 0, 0);
         screen = window_bitmap;
         broadcast_dialog_message(MSG_IDLE, 0);
         broadcast_dialog_message(MSG_DRAW, 0);
         screen = screenptr;
         position_dialog(d, tempx, tempy);
         acquire_screen();
         blit(window_bitmap, screen, 0, 0, d->x, d->y, d->w + 1, d->h + 1);
         release_screen();
        }
        else if(d->d1==BW_BUFFERED)
        {
         blit(window_bitmap, screen, 0, 0, d->x, d->y, d->w + 1, d->h + 1);
        }
        else
        {
         broadcast_dialog_message(MSG_IDLE, 0);
         broadcast_dialog_message(MSG_DRAW, 0);
        }
       }
       else /* OK, the dialog hasn't moved, but let's keep the controls updating */
       {
        broadcast_dialog_message(MSG_IDLE, 0);
       }
      }
      /*JW: we need to refresh backbuffer for the new area*/
      blit(fullscreen, myinfo->backbuffer, d->x, d->y, 0, 0, d->w+1, d->h+1);
      destroy_bitmap(fullscreen);
      if(window_bitmap)
       destroy_bitmap(window_bitmap);
      /*JW: moved this up into if(tmp) as it should be*/
     }
     broadcast_dialog_message(MSG_DRAW, 0);
     unscare_mouse();
    }
   }
  }
 }
 return D_O_K;
} 

int d_billclear_proc(int msg, DIALOG *d, int c)
{
   if (msg == MSG_DRAW) {
    scare_mouse();
    tdbox(screen, d->x, d->y, d->w, d->h, STL_FACE);
    unscare_mouse();
   }
   return D_O_K;
}


int d_billlist_proc(int msg, DIALOG *d, int c)
{
int ret;
 if (msg == MSG_DRAW) {
  _draw_billlistbox(d);
  ret = D_O_K;
 }
 else {
  ret = d_list_proc(msg, d, c);
 }
 return ret;
}

int d_billradio_proc(int msg, DIALOG *d, int c)
{
 int ret;
 int fg;
 int x;
 if (msg == MSG_DRAW) {
  scare_mouse();
  if(d->d2 == 1)       /* Square radio button */
   d_billbutton_proc(MSG_DRAW, d, c);
  else
  {
   fg = get_bill_color(bill_text);
   text_mode(get_bill_color(bill_face));
   gui_textout(screen, d->dp, d->x+15, d->y+(d->h-(text_height(font)-gui_font_baseline))/2, fg, FALSE);
 
   x = d->x;
   /*rectfill(screen, x, d->y, x+12, d->y+12, get_bill_color(bill_face));*/

   if (d->flags & D_DISABLED)
    masked_blit(radio_grey, screen, 0, 0, x, d->y, 12, 12);
   else if (d->flags & D_SELECTED)
    masked_blit(radio_sel, screen, 0, 0, x, d->y, 12, 12);
   else
    masked_blit(radio_unsel, screen, 0, 0, x, d->y, 12, 12);
  }
  ret = D_O_K;
  unscare_mouse();
 }
 else {
  ret = d_radio_proc(msg, d, c);
 }
 return ret;
}


int d_billbutton_proc(int msg, DIALOG *d, int c)
{
 int state;
 int ret;
 int txoff = 0;
 int txcol;

 if (msg == MSG_DRAW) {
  scare_mouse();
  if (d->flags & D_SELECTED) {
   state = STL_IN;
   if (bill_appearexact) {
    txoff = 1;
    state += STL_BUTTONDOWN;
   }
  }
  else {
   state = STL_OUT;
  }

  if (bill_appearexact) {
   if ((d->flags & D_GOTFOCUS) && (!(d->flags & D_SELECTED))) {
    tdbox(screen, d->x+1, d->y+1, d->w-2, d->h-2, state + STL_FLAT);
    rect(screen, d->x, d->y, d->x+d->w, d->y+d->h, get_bill_color(bill_dkshadow));
   }
   else
    tdbox(screen, d->x, d->y, d->w, d->h, state + STL_FLAT);
  }
  else
   tdbox(screen, d->x, d->y, d->w, d->h, state + STL_FLAT);

  text_mode(-1);
  if (d->flags & D_DISABLED)
   txcol = get_bill_color(bill_disabledtext);
  else
   txcol = get_bill_color(bill_text);
  gui_textout(screen, d->dp, d->x+d->w/2+txoff, d->y+d->h/2-text_height(font)/2+1+txoff, txcol, TRUE);

  if ((d->flags & D_GOTFOCUS) && (!(d->flags & D_SELECTED) || !(d->flags & D_EXIT)))
    bill_dotted_rect(d->x+3, d->y+3, d->x+d->w - 3, d->y+d->h - 3, get_bill_color(bill_shadow));
  ret = D_O_K;
  unscare_mouse();
 }
 else {
  ret = d_button_proc(msg, d, c);
 }
 return ret;
}

int d_ex_billbutton_proc(int msg, DIALOG *d, int c)
{
 int ret;
 int (*proc)(void *cbpointer, int d2value);

 if (!(d->flags & D_EXIT))
  d->flags += D_EXIT;

 ret = d_billbutton_proc(msg, d, c);

 if (ret == D_CLOSE) {
  ret = D_O_K;
  scare_mouse();
  acquire_screen();
  SEND_MESSAGE(d, MSG_DRAW, 0);
  release_screen();
  unscare_mouse();
  if (d->dp2) {
   proc = d->dp2;
   (*proc)(d->dp3, d->d2);
   return D_O_K;
  }
 }
 return ret;
}

/*

The following has been commented out, because the play_sample
call seamed to cause trouble on some systems. Please use
d_callback_billbutton instead.

int d_samplex_billbutton_proc(int msg, DIALOG *d, int c)
{
 int ret;
 int (*proc)();

 if (!(d->flags & D_EXIT))
  d->flags += D_EXIT;

 ret = d_billbutton_proc(msg, d, c);

 if (ret == D_CLOSE) {
  ret = D_O_K;
  SEND_MESSAGE(d, MSG_DRAW, 0);
  if (d->dp3) {
   play_sample(d->dp3, 255, 128, 1000, FALSE);
  }
  if (d->dp2) {
   proc = d->dp2;
   (*proc)();
   return D_O_K;
  }
 }
 return ret;
}
*/

int d_callback_billbutton_proc(int msg, DIALOG *d, int c)
{
 int ret;
 int (*proc)();

 if (!(d->flags & D_EXIT))
  d->flags += D_EXIT;

 ret = d_billbutton_proc(msg, d, c);

 if (ret == D_CLOSE) {
  ret = D_O_K;
  SEND_MESSAGE(d, MSG_DRAW, 0);
  if (d->dp2) {
   proc = d->dp2;
   (*proc)();
   return D_O_K;
  }
 }
 return ret;
}

int d_billhline_proc(int msg, DIALOG *d, int c)
{
   if (msg == MSG_DRAW) {
    scare_mouse();
    tdhline(screen, d->x, d->y, d->x+d->w);
    unscare_mouse();
   }
   return D_O_K;
}

int d_billvline_proc(int msg, DIALOG *d, int c)
{
   if (msg == MSG_DRAW) {
    scare_mouse();
    tdvline(screen, d->x, d->y, d->y+d->h);
    unscare_mouse();
   }
   return D_O_K;
}

int d_billgroupbox_proc(int msg,DIALOG *d,int c)
{
  int w = 0;
  int y = 0;

  if(d->dp)
   y = d->y + text_height(font)/2;
  else
   y = d->y;

  if(msg == MSG_DRAW)
  {
    scare_mouse();
    rect(screen, d->x+1, y+1, d->x+d->w-1, y+d->h-1, get_bill_color(bill_hlight));
    rect(screen,   d->x,   y, d->x+d->w-2, y+d->h-2, get_bill_color(bill_shadow));
    if(d->dp)
    {
     w = text_length(font,d->dp);
     rectfill(screen,d->x+5, y, d->x+15+w, y+1, get_bill_color(bill_face));
     text_mode(get_bill_color(bill_face));
     textout(screen, font, d->dp, d->x+10, d->y, get_bill_color(bill_text));
    }
    unscare_mouse();
  }

  return(D_O_K);
}


/* This array is used interanlly to store the position of the list when
   it is "dropped down" */
int _dd_pos[3];

/****************************************************************************/
/* close list if click is ouside of it **************************************/
/****************************************************************************/
int d_ddlist_closer_proc(int msg,DIALOG *d,int c)
{
  if(msg == MSG_CLICK)
  {
    while(gui_mouse_b());
    if(!(is_in(gui_mouse_x(), gui_mouse_y(), _dd_pos[0], _dd_pos[1], _dd_pos[2], _dd_pos[3])));
     return(D_CLOSE);
  }

  return(D_O_K);
}

/****************************************************************************/
/* runs the list and closes it when it is clicked ***************************/
/****************************************************************************/
int d_ddlist_list_proc(int msg, DIALOG *d, int c)
{
  int d2 = d->d2;
  int ret = d_billlist_proc(msg,d,c);

  if(msg == MSG_CLICK)
  {
    /* Scroll bar ? */
    if(d->d2 == d2)   /* If they clicked but didn't scroll, close it */ 
     return(D_CLOSE);
  }

  return(ret);
}

/****************************************************************************/
/* open and Show the list ***************************************************/
/****************************************************************************/
int _open_list_combobox(DIALOG *d)
{
  DIALOG dlg[3] = {{NULL}};
  BITMAP *bmp;


  /* Show the list */
  memset(dlg,0,sizeof(dlg));
  dlg[0].proc = d_ddlist_closer_proc;
  dlg[0].w = SCREEN_W;  /* This object will be called on all mouse clicks,
                           (except, of course, if there is an object on top
                            of it, but I chose not to take for granted that
                            do_dialog wont call it anyways), since it is
                            the w and h or the whole screen.. It is passed
                            the dd_list_procs position w/ the dd_pos array,
                            so that it knows
                            where the drop down list is, and wont close if
                            the click is within the list..
                         */
  dlg[0].h = SCREEN_H;


  dlg[1].proc = d_ddlist_list_proc;    /* Drop Down List set up */
  dlg[1].x = d->x;
  dlg[1].y = d->y+d->h+2;
  dlg[1].w = d->w-1;
  dlg[1].h = text_height(font)*4+4;
  dlg[1].dp = d->dp;
  dlg[1].d1 = d->d1;
  dlg[1].dp2 = d->dp2;
  dlg[1].flags = D_EXIT;


  _dd_pos[0] = dlg[1].x;  /* store position in dd_pos */
  _dd_pos[1] = dlg[1].y;
  _dd_pos[2] = dlg[1].w;
  _dd_pos[3] = dlg[1].h;


  scare_mouse();
  bmp = create_bitmap(dlg[1].w+1,dlg[1].h+1);
  if(bmp)
   blit(screen,bmp,dlg[1].x,dlg[1].y,0,0,dlg[1].w+1,dlg[1].h+1);

  unscare_mouse();

  do_dialog(dlg,1);

  scare_mouse();
  if(bmp)
   blit(bmp,screen,0,0,dlg[1].x,dlg[1].y,dlg[1].w+1,dlg[1].h+1);
  else
   broadcast_dialog_message(MSG_DRAW, 0);

  if(bmp)
   destroy_bitmap(bmp);
  /*JW: author said someone said adding the following line fixed the drop boxes*/
  bill_init_radio_buttons();

  d->d1 = dlg[1].d1;
  SEND_MESSAGE(d,MSG_DRAW,0);
  unscare_mouse();
  return D_O_K;
}


/****************************************************************************/
/* Create a drop down list ********************************************************/
/****************************************************************************/
/* Field : Same as a d_list_proc ou d_billlist_proc *************************/
/****************************************************************************/
int d_billddlist_proc(int msg,DIALOG *d,int c)
{
 int ret = D_O_K;
  DIALOG dlg[3] = {{NULL}};
 char *text;

 /* Set up the push button */
 dlg[0].proc = d_billbutton_proc;

 dlg[0].w = d->h-4;
 dlg[0].h = d->h-4;

 dlg[0].y  = d->y+2;
 dlg[0].x  = d->x+d->w-2-dlg[0].w;

 dlg[0].dp = "+";
 dlg[0].flags = D_EXIT;

 switch(msg)
 {
  case MSG_WANTFOCUS :
  {
   return(D_GOTFOCUS);
   break;
  }
  case MSG_CLICK:
  {
   /* Click on the button ? */
   if(is_in(gui_mouse_x(), gui_mouse_y(), dlg[0].x, dlg[0].y, dlg[0].w, dlg[0].h))
   {
    ret = SEND_MESSAGE(&dlg[0], msg, 0);

    if(ret == D_CLOSE)
    {
     ret = D_O_K;
     SEND_MESSAGE(&dlg[0], MSG_DRAW, 0);

     _open_list_combobox(d);
    }
   }
   /* Or the text field */
   else
   {
    while(gui_mouse_b());
    _open_list_combobox(d);
   }
   while(gui_mouse_b());  /* for button only, has no effect for text field
                       (mouse is already up b/c of a previous loop */

  }
  break;

  case MSG_DRAW :
  {
   scare_mouse();
   if(d->flags & D_DISABLED)
    tdbox(screen, d->x, d->y, d->w, d->h, STL_IN + STL_FACE);
   else
    if(d->flags & D_GOTFOCUS)
    {
     tdbox(screen, d->x, d->y, d->w, d->h, STL_IN + STL_FRAME + STL_TITLEBAR);
     rect(screen, d->x+2, d->y+2, dlg[0].x-1, d->h-2+d->y, get_bill_color(bill_framebg));
    }
    else
     tdbox(screen, d->x, d->y, d->w, d->h, STL_IN + STL_FRAME + STL_FRAMEFILLED);

   SEND_MESSAGE(&dlg[0], msg, 0);

   /* Show the text */
  /* scare_mouse(); */
   text = (*(getfuncptr)d->dp)(d->d1,NULL);

   if(d->flags & D_GOTFOCUS)
    textout(screen, font, text, d->x+8, d->y+(d->h-text_height(font))/2, get_bill_color(bill_titletext));
   else if(d->flags & D_DISABLED)
    textout(screen, font, text, d->x+8, d->y+(d->h-text_height(font))/2, get_bill_color(bill_disabledtext));
   else
    textout(screen,font,text,d->x+8,d->y+(d->h-text_height(font))/2, get_bill_color(bill_text));
   unscare_mouse();
  }
  break;
 }

  return(ret);
}

