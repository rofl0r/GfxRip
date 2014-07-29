#include <stdlib.h>
#include <stdio.h>
#include "allegro.h"
#include "tdgui.h"
#include <string.h>

char the_string[32] = "Change Me!";
char lbl_string[256] = "VScroll Value: 0";

void msbox();
void fbshow();
void ss_saver();
void movemode_setter();
extern DIALOG the_dialog[];
RGB white = {63, 63, 63};
RGB black = {0, 0, 0};

#define MOVEMODE_LISTBOX 15

/* callback function to specify the contents of the listbox */
char *listbox_getter(int index, int *list_size)
{
   static char *strings[] =
   {
      "Zero",  "One",   "Two",   "Three", "Four",  "Five", 
      "Six",   "Seven", "Eight", "Nine",  "Ten"
   };

   if (index < 0) {
      *list_size = 11;
      return NULL;
   }
   else
      return strings[index]; 
}

char *windowmode_listbox_getter(int index, int *list_size)
{
 static char *strings[] =
 {
  "Updated Unbuffered Moving",
  "Updated Buffered Moving",
  "Buffered Moving"
 };
 if (index < 0)
 {
  *list_size = 3;
  return NULL;
 }
 else
  return strings[index];
}

void msbox()
{
 billalert("BGui Movable Alert Box",
           "This is a BGui Alert Box.",
           "Grab the title and move it.",
           "Cool!", "Who cares?", 13, 0);
}

void fbshow()
{
 char buf[200] = "\0";
 billfile_select("File Select", buf, "*.*");
}

/* JW: added void keyword because this is C, not C++, and quit() is
   different from quit(void) essentially, and this causes an annoying warning
   in the menu struct*/
int quit(void)
{
   if (billalert("Quit?", "Really want to quit?", NULL, "Yes", "Cancel", 'y', 27) == 1)
      return D_CLOSE;
   else
      return D_O_K;
}

void vchangelbl(int i)
{
 sprintf (lbl_string, "VScroll Value: %d ", i);
 the_dialog[3].d2 = i;
 the_dialog[4].d2 = i;
 SEND_MESSAGE((the_dialog + 4), MSG_DRAW, 0);
 SEND_MESSAGE((the_dialog + 3), MSG_DRAW, 0);
 SEND_MESSAGE((the_dialog + 1), MSG_DRAW, 0);
}

void hchangelbl(int i)
{
 sprintf (lbl_string, "HScroll Value: %d ", i);
 the_dialog[2].d2 = i;
 SEND_MESSAGE((the_dialog + 2), MSG_DRAW, 0);
 SEND_MESSAGE((the_dialog + 1), MSG_DRAW, 0);
}

MENU my_menu[] =
{
   { "&Load\t(ctrl+L)",             NULL,           NULL },
   { "&Save\t(ctrl+S)",             NULL,            NULL },
   { "",                            NULL,             NULL },
   { "&Quit\t(ctrl+Q)",             quit,          NULL },
   { NULL,                          NULL,             NULL }
};

MENU menu[] =
{ 
   { "&File",                       NULL,             my_menu },
   { NULL,                          NULL,             NULL }
};

DIALOG the_dialog[] =
{
   /* (dialog proc)       (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp) */
   /* Menu bar & Hline */
   { d_billwin_proc,        70,   70,  390,  215,   255,  0,    0,    0,       BW_UPDATE,    0,    "Moveable BillWin" },

   { d_billtext_proc,      272,   89,    0,    0,     0,  0,    0,    0,       0,    0,    lbl_string },
   { d_billprogress_proc,   75,  270,  380,   10,     0,  0,    0,    0,       20,   0,    NULL},
   { d_billprogress_proc,  445,   90,   10,  175,     0,  0,    0,    0,       20,   0,    NULL},  
   { d_billprogress_proc,  253,  160,   10,   10,     0,  0,    0,    0,       20,   0,    NULL},  

   { d_billmenu_proc,       72,   89,    0,    0,     0,  0,    0,    0,       0,    0,    menu },
   { d_billradio_proc,     150,   87,   20,   12,     0,  0,    0, D_SELECTED, 0,    0,    "A" },
   { d_billradio_proc,     180,   87,   20,   12,     0,  0,    0,    0,       0,    1,    "B" },
   { d_billradio_proc,     210,   87,   20,   12,     0,  0,    0, D_DISABLED, 0,    0,    "C" },
   { d_billbutton_proc,    431,   74,   10,   10,   255,  0,    0,    D_EXIT,  0,    0,    "x" },
   { d_billhline_proc,      72,  101,  371,    0,     0,  0,    0,    0,       0,    0,    NULL},

   /* First Group */
   { d_billgroupbox_proc,   80,  105,  170,  150,   255,  0,    0,    0,       0,    0,    "Group One" },                              
   { d_billedit_proc,       85,  115,  160,   14,   255,  0,    0,    0,      30,    0,    the_string },
   { d_billbutton_proc,     85,  135,   80,   16,   255,  0,    0,    0,       0,    0,    "&Toggle Me" },
   {d_ex_billbutton_proc,  175,  135,   70,   16,   255,  0,    0,    0,       0,    0,    "Click Me"},
   { d_billlist_proc,       85,  155,  160,   30,   255,  0,    0,    0,       0,    0,    windowmode_listbox_getter },
{d_callback_billbutton_proc,85,  190,  160,   16,   255,  0,    0,    0,       0,    0,    "Set Move Mode", movemode_setter},
   { d_billddlist_proc,     85,  210,  160,   14,   255,  0,    0,    0,       0,    0,    listbox_getter },
   { d_billbutton_proc,     85,  235,   60,   16,   255,  0,    0,    D_EXIT,  0,    0,    "OK" },
{d_callback_billbutton_proc,153, 235,   92,   16,   255,  0,    0,        0,  0,    0,    "Screen Shot", ss_saver },

   /* Second Group */
   { d_billgroupbox_proc,  265,  110,  175,  152,   255,  0,    0,    0,       0,    0,    NULL },                              
   { d_hbillscrollbar_proc,270,  245,  145,   12,   255,  0,    0,    0,      20,    0,    hchangelbl},
   { d_vbillscrollbar_proc,420,  115,   12,  142,   255,  0,    0,    0,      20,    0,    vchangelbl},
   { d_billslider_proc,    270,  225,  145,   16,   255,  0,    0,    0,     200,    0,   NULL },
{d_callback_billbutton_proc,270,  204,  70,   16,   255,  0,    0,    0,       0,    0,    "Msg Box", msbox},
   { d_billtextbox_proc,   270,  115,  120,   65,   255,  0,    0,    0,       0,    0,    "This is a BillTextbox. It is a non editable text box with word wrapping and scroll bars. You can put a string of any length in a BillTextbox." },
{d_callback_billbutton_proc,270,  185,  70,   16,   255,  0,    0,    0,       0,    0,    "File Box", fbshow},
   {d_billcheck_proc,      342,  188,   55,   10,     0,  0,    0,    0,       0,    0,    "Check"},
   {d_billcheck_proc,      342,  207,   55,   10,     0,  0,    0, D_DISABLED, 1,    0,    "Check"},
   { d_billslider_proc,    400,  115,   16,   80,   255,  0,    0,    0,     200,    0,   NULL },
   { NULL,                   0,    0,    0,    0,     0,  0,    0,    0,       0,    0,    NULL }
};

void ss_saver()
{
 PALETTE pal;
 get_palette(pal);
 acquire_screen();
 save_pcx("ss.pcx", screen, pal);
 release_screen();
}

void movemode_setter()
{
 if(the_dialog[MOVEMODE_LISTBOX].d1 == 1)
  the_dialog[0].d1 = BW_BUFFEREDUPDATE;
 else if(the_dialog[MOVEMODE_LISTBOX].d1 == 2)
  the_dialog[0].d1 = BW_BUFFERED;
 else
  the_dialog[0].d1 = BW_UPDATE;
}

/* JW: made main take proper parameters*/
int main(int argc, char* argv[])
{
 int gcard;
 int gwid;
 int ghgt;

 allegro_init();
 install_keyboard(); 
 install_mouse();
 install_timer();

 set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0);

 bill_init();
 bill_init_very_smallpal(17);
 set_color(16, &white);
 set_color(255, &black);

 set_palette(desktop_palette);
 clear_to_color(screen, get_bill_color(bill_face));
 if (!gfx_mode_select(&gcard, &gwid, &ghgt)) {
  allegro_exit();
  exit(1);
 }
 set_gfx_mode(gcard, gwid, ghgt, 0, 0);

 bill_init_smallpal(17);
 set_color(16, &white);
 set_color(255, &black);
 clear_to_color(screen, get_bill_color(bill_face));
 set_gui_mode(1);
 init_moveable(the_dialog);
 do_dialog (the_dialog, 0);
 shutdown_moveable(the_dialog);
 return 0;
}
END_OF_MAIN();
