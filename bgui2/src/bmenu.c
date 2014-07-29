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
#include "bmenu.h"

/* The following routines came straight out of Allegro, names and all.
   I hope this doesn't cause any problems, but I wouldn't know, as I
   am realatively new to C programming.  
*/
static void fill_menu_info(MENU_INFO *m, MENU *menu, MENU_INFO *parent, int bar, int x, int y, int minw, int minh)
{
   int c, i;
   int extra = 0;
   char buf[80], *tok;

   m->menu = menu;
   m->parent = parent;
   m->bar = bar;
   m->x = x;
   m->y = y;
   m->w = 2;
   m->h = (m->bar) ? (text_height(font)+6) : 3;
   m->proc = NULL;
   m->sel = -1;

   /* calculate size of the menu */
   for (m->size=0; m->menu[m->size].text; m->size++) {
      for (i=0; (m->menu[m->size].text[i]) && (m->menu[m->size].text[i] != '\t'); i++)
	 buf[i] = m->menu[m->size].text[i];
      buf[i] = 0;

      c = gui_strlen(buf);
                    
      if (m->bar) {
	 m->w += c+16;
      }
      else {
	 m->h += text_height(font)+4;
	 m->w = MAX(m->w, c+16);
      }

      if (m->menu[m->size].text[i] == '\t') {
	 tok = m->menu[m->size].text+i+1;
	 c = gui_strlen(tok);
	 extra = MAX(extra, c);
      }
   }

   if (extra)
      m->w += extra+16;

   m->w = MAX(m->w, minw);
   m->h = MAX(m->h, minh);
}

static void get_menu_pos(MENU_INFO *m, int c, int *x, int *y, int *w)
{
   int c2;

   if (m->bar) {
      *x = m->x+1;

      for (c2=0; c2<c; c2++)
	 *x += gui_strlen(m->menu[c2].text) + 16;

      *y = m->y+1;
      *w = gui_strlen(m->menu[c].text) + 16;
   }
   else {
      *x = m->x+2;
      *y = m->y+c*(text_height(font)+4)+2;
      *w = m->w-4;
   }
}

/* menu_mouse_object:
 *  Returns the index of the object the mouse is currently on top of.
 */
static int menu_mouse_object(MENU_INFO *m)
{
   int c;
   int x, y, w;

   for (c=0; c<m->size; c++) {
      get_menu_pos(m, c, &x, &y, &w);

      if ((gui_mouse_x() >= x) && (gui_mouse_x() < x+w) &&
          (gui_mouse_y() >= y) && (gui_mouse_y() < y+(text_height(font)+4)))
	 return (m->menu[c].text[0]) ? c : -1;
   }

   return -1;
}

/* mouse_in_parent_menu:
 *  Recursively checks if the mouse is inside a menu or any of its parents.
 */
static int mouse_in_parent_menu(MENU_INFO *m) 
{
   int c;

   if (!m)
      return FALSE;

   c = menu_mouse_object(m);
   if ((c >= 0) && (c != m->sel))
      return TRUE;

   return mouse_in_parent_menu(m->parent);
}

int billmenu_alt_key(int k, MENU *m)
// JW: because of the comment above, I hoped that this new Allegro function
//  will work in place of BGUI's function, because it stated it was copied
//  right out of Allegro.  I really don't know why he did this though...
{
   static unsigned char alt_table[] =
   {
      KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, 
      KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, 
      KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z
   };

   char *s;
   int c, d;

   if (k & 0xFF)
      return 0;

   k >>= 8;

   c = scancode_to_ascii(k);
   if (c) {
      k = c;
   } else {
      for (c=0; c<(int)sizeof(alt_table); c++) {
	 if (k == alt_table[c]) {
	    k = c + 'a';
	    break;
	 }
      }

      if (c >= (int)sizeof(alt_table))
	 return 0;
   }

   for (c=0; m[c].text; c++) {
      s = m[c].text;
      while ((d = ugetx(&s)) != 0) {
	 if (d == '&') {
	    d = ugetc(s);
	    if ((d != '&') && (utolower(d) == utolower(k)))
	       return k;
	 }
      }
   }

   return 0;
/*  This is the old BGUI code:
   char *s;
   int c;

   if (k & 0xff)
      return 0;

   k = key_ascii_table[k>>8];

   for (c=0; m[c].text; c++) {
      s = m[c].text;
      while (*s) {
	 if (*s == '&') {
	    s++;
	    if ((*s != '&') && (tolower(*s) == tolower(k)))
	       return k;
	 }
	 s++;
      }
   }

   return 0;
*/
}

/* menu_key_shortcut:
 *  Returns true if c is indicated as a keyboard shortcut by a '&' character
 *  in the specified string.
 */
static int menu_key_shortcut(int c, char *s)
{
   while (*s) {
      if (*s == '&') {
	 s++;
	 if ((*s != '&') && (tolower(*s) == tolower(c & 0xff)))
	    return TRUE;
      }
      s++;
   }

   return FALSE;
}

static void draw_billmenu_item(MENU_INFO *m, int c)
{
 int fg, bg;
 int i, x, y, w;
 char buf[80], *tok;
 int stl;

 if (m->menu[c].flags & D_DISABLED) {
  fg = get_bill_color(bill_disabledtext);
  bg = get_bill_color(bill_face);
  stl = STL_FACE;
 }
 else {
  if (c == m->sel) {
   fg = get_bill_color(bill_titletext);;
   bg = get_bill_color(bill_activetitle);
   stl = STL_TITLEBAR;
  }
  else {
   fg = get_bill_color(bill_text);
   bg = get_bill_color(bill_face);
   stl = STL_FACE;
  }
 }

 get_menu_pos(m, c, &x, &y, &w);

 tdbox(screen, x, y, w, text_height(font)+3, stl);

 text_mode(bg);

 if (m->menu[c].text[0]) {
  for (i=0; (m->menu[c].text[i]) && (m->menu[c].text[i] != '\t'); i++)
   buf[i] = m->menu[c].text[i];
  buf[i] = 0;

  gui_textout(screen, buf, x+8, y+1, fg, FALSE);

  if (m->menu[c].text[i] == '\t') {
   tok = m->menu[c].text+i+1;
   gui_textout(screen, tok, x+w-gui_strlen(tok)-8, y+1, fg, FALSE);
  }
 }
 else
  tdhline(screen, x, y+text_height(font)/2+2, x+w);

 if (m->menu[c].flags & D_SELECTED) {
  line(screen, x+1, y+text_height(font)/2+1, x+3, y+text_height(font)+1, fg);
  line(screen, x+3, y+text_height(font)+1, x+6, y+2, fg);
 }
}

static void draw_billmenu(MENU_INFO *m)
{
 int c;

 for (c=0; m->menu[c].text; c++)
  draw_billmenu_item(m, c);

 if(!m->bar) tdbox(screen, m->x, m->y, m->w, m->h, STL_OUT);
}

int _do_billmenu(MENU *menu, MENU_INFO *parent, int bar, int x, int y, int repos, int *dret, int minw, int minh)
{
   MENU_INFO m;
   MENU_INFO *i;
   int c, c2;
   int ret = -1;
   int mouse_on = gui_mouse_b();
   int old_sel;
   int mouse_sel;
   int _x, _y;
   int redraw = TRUE;

   show_mouse(NULL);

   fill_menu_info(&m, menu, parent, bar, x, y, minw, minh);

   if (repos) {
      m.x = MID(0, m.x, SCREEN_W-m.w-1);
      m.y = MID(0, m.y, SCREEN_H-m.h-1);
   }

   /* save screen under the menu */
   m.saved = create_bitmap(m.w+1, m.h+1); 

   if (m.saved)
      blit(screen, m.saved, m.x, m.y, 0, 0, m.w+1, m.h+1);
   else
      errno = ENOMEM;

   m.sel = mouse_sel = menu_mouse_object(&m);
   if ((m.sel < 0) && (!mouse_on) && (!bar))
      m.sel = 0;

   show_mouse(screen);

   do {
      old_sel = m.sel;

      c = menu_mouse_object(&m);
      if ((gui_mouse_b()) || (c != mouse_sel))
	 m.sel = mouse_sel = c;

      if (gui_mouse_b()) {                            /* if button pressed */
         if ((gui_mouse_x() < m.x) || (gui_mouse_x() > m.x+m.w) ||
             (gui_mouse_y() < m.y) || (gui_mouse_y() > m.y+m.h)) {
	    if (!mouse_on)                            /* dismiss menu? */
	       break;

	    if (mouse_in_parent_menu(m.parent))       /* back to parent? */
	       break;
	 }

	 if ((m.sel >= 0) && (m.menu[m.sel].child))   /* bring up child? */
	    ret = m.sel;

	 mouse_on = TRUE;
	 clear_keybuf();
      }
      else {                                          /* button not pressed */
	 if (mouse_on)                                /* selected an item? */
	    ret = m.sel;

	 mouse_on = FALSE;

	 if (keypressed()) {                          /* keyboard input */
	    c = readkey();

	    if ((c & 0xff) == 27) {
	       ret = -1;
	       goto getout;
	    }

	    switch (c >> 8) {

	       case KEY_LEFT:
		  if (m.parent) {
		     if (m.parent->bar) {
			simulate_keypress(KEY_LEFT<<8);
			simulate_keypress(KEY_DOWN<<8);
		     }
		     ret = -1;
		     goto getout;
		  }
		  /* fall through */

	       case KEY_UP:
		  if ((((c >> 8) == KEY_LEFT) && (m.bar)) ||
		      (((c >> 8) == KEY_UP) && (!m.bar))) {
		     c = m.sel;
		     do {
			c--;
			if (c < 0)
			   c = m.size - 1;
		     } while ((!(m.menu[c].text[0])) && (c != m.sel));
		     m.sel = c;
		  }
		  break;

	       case KEY_RIGHT:
		  if (((m.sel < 0) || (!m.menu[m.sel].child)) &&
		      (m.parent) && (m.parent->bar)) {
		     simulate_keypress(KEY_RIGHT<<8);
		     simulate_keypress(KEY_DOWN<<8);
		     ret = -1;
		     goto getout;
		  }
		  /* fall through */

	       case KEY_DOWN:
		  if ((m.sel >= 0) && (m.menu[m.sel].child) &&
		      ((((c >> 8) == KEY_RIGHT) && (!m.bar)) ||
		       (((c >> 8) == KEY_DOWN) && (m.bar)))) {
		     ret = m.sel;
		  }
		  else if ((((c >> 8) == KEY_RIGHT) && (m.bar)) ||
			   (((c >> 8) == KEY_DOWN) && (!m.bar))) {
		     c = m.sel;
		     do {
			c++;
			if (c >= m.size)
			   c = 0;
		     } while ((!(m.menu[c].text[0])) && (c != m.sel));
		     m.sel = c;
		  }
		  break;

	       case KEY_SPACE:
	       case KEY_ENTER:
		  if (m.sel >= 0)
		     ret = m.sel;
		  break;

	       default:
		  if ((!m.parent) && ((c & 0xff) == 0))
                     c = billmenu_alt_key(c, m.menu);
		  for (c2=0; m.menu[c2].text; c2++) {
		     if (menu_key_shortcut(c, m.menu[c2].text)) {
			ret = m.sel = c2;
			break;
		     }
		  }
		  if (m.parent) {
		     i = m.parent;
		     for (c2=0; i->parent; c2++)
			i = i->parent;
                     c = billmenu_alt_key(c, i->menu);
		     if (c) {
			while (c2-- > 0)
			   simulate_keypress(27);
			simulate_keypress(c);
			ret = -1;
			goto getout;
		     }
		  }
		  break;
	    }
	 }
      }

      if ((redraw) || (m.sel != old_sel)) {           /* selection changed? */
	 show_mouse(NULL);

	 if (redraw) {
            draw_billmenu(&m);
	    redraw = FALSE;
	 }
	 else {
	    if (old_sel >= 0)
               draw_billmenu_item(&m, old_sel);

	    if (m.sel >= 0)
               draw_billmenu_item(&m, m.sel);
	 }

	 show_mouse(screen);
      }

      if ((ret >= 0) && (m.menu[ret].flags & D_DISABLED))
	 ret = -1;

      if (ret >= 0) {                                 /* child menu? */
	 if (m.menu[ret].child) {
	    if (m.bar) {
	       get_menu_pos(&m, ret, &_x, &_y, &c);
	       _x += 6;
	       _y += text_height(font)+7;
	    }
	    else {
	       _x = m.x+m.w*2/3;
	       _y = m.y + (text_height(font)+4)*ret + text_height(font)/4+2;
	    }
            c = _do_billmenu(m.menu[ret].child, &m, FALSE, _x, _y, TRUE, NULL, 0, 0);
	    if (c < 0) {
	       ret = -1;
	       mouse_on = FALSE;
	       mouse_sel = menu_mouse_object(&m);
	    }
	 }
      }

      if ((m.bar) && (!gui_mouse_b()) && (!keypressed()) &&
          ((gui_mouse_x() < m.x) || (gui_mouse_x() > m.x+m.w) ||
           (gui_mouse_y() < m.y) || (gui_mouse_y() > m.y+m.h)))
	 break;

   } while (ret < 0);

   getout:

   if (dret)
      *dret = 0;

   /* callback function? */
   if ((!m.proc) && (ret >= 0)) {
      active_menu = &m.menu[ret];
      m.proc = active_menu->proc;
   }

   if (ret >= 0) {
      if (parent)
	 parent->proc = m.proc;
      else  {
	 if (m.proc) {
	    c = m.proc();
	    if (dret)
	       *dret = c;
	 }
      }
   }

   /* restore screen */
   if (m.saved) {
      show_mouse(NULL);
      blit(m.saved, screen, 0, 0, m.x, m.y, m.w+1, m.h+2);
      destroy_bitmap(m.saved);
   }

   show_mouse(screen);

   return ret;
}

int do_billmenu(MENU *menu, int x, int y)
{
   int ret = _do_billmenu(menu, NULL, FALSE, x, y, TRUE, NULL, 0, 0);

   do {
   } while (gui_mouse_b());

   return ret;
}


int d_billmenu_proc(int msg, DIALOG *d, int c)
{ 
  MENU_INFO m;
  int x;
  int ret = D_O_K;

  switch (msg) {
   case MSG_DRAW:
    fill_menu_info(&m, d->dp, NULL, TRUE, d->x-1, d->y-1, d->w+2, d->h+2);
    draw_billmenu(&m);
    break;

   case MSG_XCHAR: 
    x = billmenu_alt_key(c, d->dp);
    if (!x)
     break;
    ret |= D_USED_CHAR;
    simulate_keypress(x);
    SEND_MESSAGE(d, MSG_CLICK, 0);
    /*No break, keep right on going*/

   case MSG_CLICK:
   case MSG_GOTMOUSE:
    _do_billmenu(d->dp, NULL, TRUE, d->x-1, d->y-1, FALSE, &x, d->w+2, d->h+2);
    ret |= x;
    do {
    } while (gui_mouse_b());
    break;

   default:
    ret = d_menu_proc(msg, d, c);
    break;
  }
  return ret;
}

