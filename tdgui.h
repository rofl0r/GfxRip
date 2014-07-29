/*
Several constants are re-defined under different names.  This is simply
because they are used for different purposes.  They do exactly the same thing,
it is just to make the code easier to read / understand

Please note the 90% of the code in here was taken straight out of Allegro,
I only made a few small changes to the drawing routines.
*/

#ifndef _TDGUI_H_
#define _TDGUI_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define BW_UPDATE 0
#define BW_BUFFEREDUPDATE 1
#define BW_BUFFERED 2

#define MODE_EXACTWIN    TRUE
#define MODE_ALLEGCOMPAT FALSE

#define STL_IN  1            //In style. looks like start button.
                             //(this is different from a standard button)   
#define STL_DOWN  STL_IN     //Same as above.
#define STL_OUT  2           //Opposite of STL_IN
#define STL_UP  STL_OUT      //Same as above.
#define STL_FLAT 4           //Makes faces of objects grey.
#define STL_FILLED STL_FLAT  //Same as above.
#define STL_FACE STL_FLAT    //Same as above.
#define STL_FRAME 8          //Used in conjunction with STL_IN to draw a frame.
                             //Frames are drawn differently from other objects.
#define STL_DITHERED 16      //Used for the background of scroll bars.
#define STL_FRAMEFILLED 32   //Fills the backgroud with white for the frames.
#define STL_BUTTONDOWN 64    //Used when you have called set_gui_mode(1).
                             //Buttons are drawn differently than other
                             //"in" objects.
#define STL_TITLEBAR 128     //Window titles and menus.

#define SCL_UPBUTTON_DOWN 1  //Flags used with the half-finished scrollbars.
#define SCL_DOWNBUTTON_DOWN 2//(Note: the scrollbars on the ListBox are done.
                             // It's the stand-alone ones that aren't)
#define SCL_UPARROW 4
#define SCL_DOWNARROW 8
#define SCL_LEFTARROW 16
#define SCL_RIGHTARROW 32

#define DIR_HORIZ 1
#define DIR_VERT 2

#define BILL_CLEAR      -1    /*Set this as BG on bill_text and bill_ctext,
                                If you want. It is negative so that if you
                                are converting an old Allegro dialog, you
                                won't accidentally make clear text by not
                                changing the old value.
                              */
#define BILL_TITLE      -2 


#define GFX_CANCEL         3  //Stuff taken out of allgro.
#define GFX_DRIVER_LIST    4
#define GFX_MODE_LIST      5
#define GFX_DEPTH_LIST     6

#define A_S1  0
#define A_S2  1
#define A_S3  2
#define A_B1  3
#define A_B2  4
#define A_B3  5

#define BILL_SMALLPAL_SIZE 10 //This is just the number of colors required
                              //for the bill_init_smallpal  
#define BILL_VERY_SMALLPAL_SIZE 7  //This is just the number of colors required
                              //for the bill_init_very_smallpal  

extern int bill_appearexact;  //Turned on by set_gui_mode(1)

extern float bg_ver;
extern char bg_ver_str[];

extern RGB bill_light;        //The colors used by Bgui
extern RGB bill_hlight;
extern RGB bill_face;
extern RGB bill_shadow;
extern RGB bill_dkshadow;
extern RGB bill_text;
extern RGB bill_activetitle;
extern RGB bill_titletext;
extern RGB bill_framebg;
extern RGB bill_disabledtext;
extern PALETTE bill_pal;

void memverify_str(void *pointer, char *str);
void memverify(void *pointer); /* This is used to verify malloc'd memory
                                 before use.  Just checks for a NULL pointer
                                 and exits if it's found.  I encourage you
                                 to use this as well, since it's here */

void set_gui_mode(int mode);  //Right now this only makes the buttons look
                              //more like Win95. Later I hope to add arrows
                              //to the scrollbars on the listbox.  Leaving
                              //this off will (after BGui is done) make the
                              //controls fit the same dimentions as their
                              //Allegro counterparts better.

int get_gui_mode();           /* tells you what mode the GUI is in. */

int get_bill_color(RGB billcolor);       //Just returns the closest index of an RGB.
void bill_init_pal();                    //Gray palette w/ blue for the title bar.
void bill_init_smallpal(int start);      // use 9 colors of pallete for BGui.
void bill_init_very_smallpal(int start); /*Same as smallpal, except it skips
                                           repeat colors, and takes the required
                                           color count down to 7. DONT USE THIS
                                           IF YOU ARE GOING TO CHANGE THE COLOR
                                           CONSTANTS
                                         */
void bill_init();                  //As of now, this does absolutely nothing.

void tdbox(BITMAP *b, int x, int y, int w, int h, int stl);
                                   //BGui is based on this routine. You
                                   //optimize it, and you are optimizing all
                                   //of BGui.  If some kind person can make
                                   //it a lot faster for me, that would be
                                   //great.

void tdhline(BITMAP *b, int x, int y, int x2);
void tdvline(BITMAP *b, int x, int y, int y2);

int is_in(int xa, int ya, int xb, int yb, int w, int h);
                                   //Used w/ scrollbar

void _draw_scrollable_billframe(DIALOG *d, int listsize, int offset,
                   int height, int fg_color, int bg);
                                   //Same as the counterpart function in
                                   //Allegro, except it looks like windows
                                   //95. (except there are no arrows on the
                                   //scroll bar).

void _draw_billlistbox(DIALOG *d); 
void _draw_billtextbox(char *thetext, int *listsize, int draw, int offset,
		   int wword, int tabsize, int x, int y, int w, int h,
                   int disabled, int fore, int deselect, int disable);

void _draw_arrow(int x, int y, int basew, int colr, int stl);
                                   //Used w/ scrollbar to draw arrows

void bill_fade_bmp(BITMAP *bmp, int clr);
                                   //You know how Windows fades the screen
                                   //when you hit shutdown? That's what this
                                   //does.  Please note that it's kinda slow.

                                   //A quick note about the billalert
                                   //functions:  If you use these, you may
                                   //want to slightly modify your program,
                                   //instead of having 3 lines of text, these
                                   //have a title bar and 2 lines of text.
int billalert3(char *s1, char *s2, char *s3, char *b1, char *b2,
                   char *b3, int c1, int c2, int c3);
int billalert(char *s1, char *s2, char *s3, char *b1, char *b2,
                   int c1, int c2);

                                   //The following are direct replacements
                                   //for the allegro counterparts.
int d_hbillscrollbar_proc(int msg, DIALOG *d, int c);   //Horizontal ScrollBar
int d_vbillscrollbar_proc(int msg, DIALOG *d, int c);

int d_billprogress_proc(int msg, DIALOG *d, int c); /*progress bar */

int d_billtextbox_proc(int msg, DIALOG *d, int c);
int d_billedit_proc(int msg, DIALOG *d, int c);

int d_billslider_proc(int msg, DIALOG *d, int c);
int d_billwin_proc(int msg, DIALOG *d, int c);         //A window (non-moveable)
                                                      //W/ title bar.  
int d_billlist_proc(int msg, DIALOG *d, int c);
int d_billbutton_proc(int msg, DIALOG *d, int c);
int d_ex_billbutton_proc(int msg, DIALOG *d, int c);  //Button w/ callback.
int d_samplex_billbutton_proc(int msg, DIALOG *d, int c); //Button w/ callback
                                                          //that was SUPPOSED
                                                          //to play a sample
                                                          //(doesn't work)

int d_billtext_proc(int msg, DIALOG *d, int c);
int d_billctext_proc(int msg, DIALOG *d, int c);
int d_billcheck_proc(int msg, DIALOG *d, int c);

int d_billclear_proc(int msg, DIALOG *d, int c);

int billfile_select(char *message, char *path, char *ext);

int d_billmenu_proc(int msg, DIALOG *d, int c);

int d_billhline_proc(int msg, DIALOG *d, int c);
int d_billvline_proc(int msg, DIALOG *d, int c);

int d_billradio_proc(int msg, DIALOG *d, int c);
void bill_init_radio_buttons();

int d_callback_billbutton_proc(int msg, DIALOG *d, int c);

int d_billscrollablebitmap_proc(int msg, DIALOG *d, int c);
int d_billgroupbox_proc(int msg, DIALOG *d, int c);
int d_billddlist_proc(int msg,DIALOG *d,int c);

//Here's some stuff that I threw in as an added bonus :-)
void replace_col(BITMAP *b, int oc, int nc);              //Replaces color
                                                          //oc in bitmap b
                                                          //w/ color nc.
void make_solid_pal(PALETTE pal, int start, int stop, int r, int g, int b);
                                                          //makes palette solid
                                                          //from start to stop
int moveable_do_dialog(DIALOG *dialog, int focus_obj);
int init_moveable(DIALOG *dialog);
int shutdown_moveable(DIALOG *dialog);
int do_billmenu(MENU *menu, int x, int y);

#ifdef __cplusplus
}
#endif
#endif //_TDGUI_H_
