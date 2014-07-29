void data2bmp(unsigned char d[], BITMAP *b, int w, int h);
void bill_init_radio_buttons();

//static void bill_dotted_rect(int x1, int y1, int x2, int y2, int fg);
                                   //Same as Allegro's dotted rect, except
                                   //it leaves spaces inbetween the dots alone.

typedef char *(*getfuncptr)(int, int *);

extern BITMAP *radio_unsel;
extern BITMAP *radio_sel;
extern BITMAP *radio_grey;

extern unsigned char radio_unsel_data[144];
extern unsigned char radio_sel_data[144];
extern unsigned char radio_grey_data[144];

typedef struct BILLWIN_INFO_STRUCTURE
{
 BITMAP *backbuffer;
 int mdx;
 int mdy;
} BILLWIN_INFO_STRUCTURE;
