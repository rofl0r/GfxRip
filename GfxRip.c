// #define ALLEGRO_STATICLINK

#include <allegro.h>
#include "winalleg.h"
#include "tdgui.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

volatile int target_game_time = 0;

int fsize;
FILE * file;

unsigned char * mem = 0;

#define scrW 800
#define scrH 600

#define hole 2
#define legend 9

int offset = 0;
//int offset = 1322704;
int blXSize = 320/8;
int blYSize = 200;
//int blYSize = 112;

int contin_save = 1;

int bits = 1;
//int bits = 8;

int skip = 0;
int skipmode = 0; // 0 = skip bytes after picture , 1 = skip bytes after each bitplane
char skipmoder;

bool reverse = false;
int mode = 0;
int palsearchmode = 2;

char pmoder[4] = { 'S', 'X', 'E', 'A' };

BITMAP * bitmap;

RGB pal[256];

int numX;
int numY;

bool zoom = false;

char goto_num[255];

int ttt=0;
int palfound = 0;

int bplorder[8] = {0,1,2,3,4,5,6,7};

char gotottt[256] = "Go To BYTE:";

DIALOG goto_dialog[] =
{
	/* (dialog proc)         (x)   (y)   (w)  (h)   (fg)(bg) (key)    (flags)  (d1)  (d2)    (dp) */
	{ d_billwin_proc,         0,    0,  180,  60,   255,  0,    0,         0,
																 BW_BUFFEREDUPDATE,    0,    gotottt },
		
	{ d_billbutton_proc,	100,   28,   70,  15,   255,  0,   27,    D_EXIT,    0,    0,    "DONE"},

	{ d_billedit_proc,		 10,   30,   70,  12,     0,  0,    0,         0,    8,    0,    goto_num },
	
	{ NULL,                   0,    0,    0,   0,     0,  0,    0,         0,    0,    0,    NULL }
};

char save_name[255] = "p.bmp";
char save_x[255] = "1";
char save_y[255] = "1";
char save_border[255] = "0";
char save_bcolor[255] = "253";

DIALOG save_dialog[] =
{
	/* (dialog proc)         (x)   (y)   (w)  (h)   (fg)(bg) (key)    (flags)  (d1)  (d2)    (dp) */
	{ d_billwin_proc,         0,    0,  320, 130,   255,  0,    0,         0,
																 BW_BUFFEREDUPDATE,    0,    "Save Picture(s)" },
		
	{ d_billbutton_proc,	210,  105,   70,  15,   255,  0,   27,    D_EXIT,    0,    0,    "EXIT"},
	
	{ d_billtext_proc,       10,   23,    0,   0,     0,  0,    0,         0,    0,    0,    "Filename:" },
	{ d_billedit_proc,		 90,   21,  220,  12,     0,  0,    0,         0,  200,    0,    save_name },

	{ d_billtext_proc,       10,   50,    0,   0,     0,  0,    0,         0,    0,    0,    "Pictures in X:" },
	{ d_billedit_proc,		130,   48,   40,  12,     0,  0,    0,         0,    8,    0,    save_x },
	
	{ d_billtext_proc,       10,   70,    0,   0,     0,  0,    0,         0,    0,    0,    "Pictures in Y:" },
	{ d_billedit_proc,		130,   68,   40,  12,     0,  0,    0,         0,    8,    0,    save_y },

	{ d_billtext_proc,      190,   40,    0,   0,     0,  0,    0,         0,    0,    0,    "Save Direction:" },
	{ d_billradio_proc,		190,   55,   60,  12,     0,  0,    0,D_SELECTED,    0,    0,    "Left -> Right" },
	{ d_billradio_proc,		190,   70,   60,  12,     0,  0,    0,         0,    0,    0,    "Up -> Down" },

	{ d_billbutton_proc,	 40,  105,   70,  15,   255,  0,   27,    D_EXIT,    0,    0,    "SAVE"},

	{ d_billtext_proc,       10,   90,    0,   0,     0,  0,    0,         0,    0,    0,    "Border Around:" },
	{ d_billedit_proc,		130,   88,   40,  12,     0,  0,    0,         0,    8,    0,    save_border },
	
	{ d_billtext_proc,      185,   90,    0,   0,     0,  0,    0,         0,    0,    0,    "Colour:" },
	{ d_billedit_proc,		250,   88,   40,  12,     0,  0,    0,         0,    8,    0,    save_bcolor },
	
	{ NULL,                   0,    0,    0,   0,     0,  0,    0,         0,    0,    0,    NULL }
};

int getpixelcol(int pos, int x, int y)
{
	bool bit[8];
	int p;
	int byte1;
	int bit1;
	int nn;
	int bitn;
	int i;

	for (i=0; i < 8; i++)
	{
		bit[i]=false;
	}

	for (i=0; i < bits; i++)
	{
		switch (mode)
		{
		case 0:
			// Amiga type bitplanes
			p = pos+(blXSize*blYSize)*(i);
			nn = x+(y*(blXSize*8));
			break;
		case 1:
			// ST Type bitplanes
			p = pos+blXSize*(i);
			nn = x+(bits*y*(blXSize*8));
			break;
		case 2:
			// mode == 2	// Amiga Sprite !!!
			if (i < 2)
			{
				p = pos+blXSize*(i);
				nn = x+((2*y)*(blXSize*8));
			}
			else
			{
				p = pos+blXSize*(i-2)+(blYSize*blXSize)*2;
				nn = x+((2*y)*(blXSize*8));
			}
			break;
		case 3:
			// mode == 3 // CPC gfx for Bat-Man / HoH
			p = pos+(x/8)+i;
			nn = x+((2*y)*(blXSize*8));
			break;
		case 4:
			//  mode ==4 // CPC gfx for Ultimate games
			p = pos+(x/4)+(blXSize*y*2);
			nn = (i*4)+x%4;
			break;
		}
		if (skipmode == 1)
		{
			p+=skip*i;
		}
		byte1 = nn/8;
		bit1 = 7-(nn%8);
		if (reverse)
		{
			bitn = (bits-i)-1;
		}
		else
		{
			bitn = i;
		}
		if (p+byte1 < fsize)
		{
			if (mem[p+byte1] & (1 << bit1))
			{
				bit[bitn] = true;
			}
			else
			{
				bit[bitn] = false;
			}
		}
		else
		{
			bit[bitn] = false;
		}
	}
	int col = 0;
	for (i=0; i < bits; i++)
	{
		if (bit[bplorder[i]])
		{
			col+=1<<i;
		}
	}
	return col;
}

void drawbitmap(BITMAP * bmp, int pos, int xx, int yy)
{
	for (int x = 0; x < blXSize*8; x ++)
	{
		for (int y = 0; y < blYSize; y++)
		{
			putpixel(bmp, xx+x, yy+y, getpixelcol(pos, x, y));
		}
	}
}

void drawstuff()
{
	int xx, yy;

	int pos = offset;

	for (int y=0; y < numY; y++)
	{
		for (int x=0; x < numX; x++)
		{
			xx = x*(blXSize*8+hole);
			yy = legend+y*(blYSize+hole);

			drawbitmap(bitmap, pos, xx, yy);
//			pos+=((blXSize*blYSize)*bits)+skip;
			if (skipmode == 0)
			{
				pos+=((blXSize*blYSize)*bits)+skip;
			}
			else
			{
				pos+=((blXSize*blYSize+skip)*bits);
			}
		}
	}
}

void draw_zoom()
{
	int xx, yy;
	
	xx = scrW-(blXSize*8*2+4);
	yy = scrH-(blYSize*2+4);

	if (xx < scrW/2)
	{
		xx = scrW/2;
	}
	if (yy < scrH/2)
	{
		yy = scrH/2;
	}
	stretch_blit(bitmap, bitmap, 0, legend, blXSize*8, blYSize, xx+2, yy+2, blXSize*8*2, blYSize*2);
	rect(bitmap,xx,yy,scrW-1,scrH-1,makecol(0,0,255));
	rect(bitmap,xx+1,yy+1,scrW-2,scrH-2,253);
}

static BITMAPINFO *get_bitmap_info(BITMAP * bitmap, PALETTE pal)
{
   BITMAPINFO *bi;
   int x;
   int bpp;

   bi = (BITMAPINFO *)malloc(sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 256);

   bpp = 8;

   ZeroMemory(&bi->bmiHeader, sizeof(BITMAPINFOHEADER));

   bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bi->bmiHeader.biBitCount = 8;
   bi->bmiHeader.biPlanes = 1;
   bi->bmiHeader.biWidth = bitmap->w;
   bi->bmiHeader.biHeight = -bitmap->h;
   bi->bmiHeader.biClrUsed = 256;
   bi->bmiHeader.biCompression = BI_RGB;

   if (pal) {
      for (x = 0; x < 256; x++) {
	 bi->bmiColors[x].rgbRed = _rgb_scale_6[pal[x].r];
	 bi->bmiColors[x].rgbGreen = _rgb_scale_6[pal[x].g];
	 bi->bmiColors[x].rgbBlue = _rgb_scale_6[pal[x].b];
	 bi->bmiColors[x].rgbReserved = 0;
      }
   }

   return bi;
}


static BYTE *get_dib(BITMAP * bitmap)
{
   int bpp;
   int y;
   int pitch;
   BYTE *pixels;

   bpp = 8;
   pitch = bitmap->w;
   pitch = (pitch + 3) & ~3;	/* align on dword */

   pixels = (BYTE *) malloc(bitmap->h * pitch);
   if (!pixels)
      return NULL;

   /* this code will probably need optimalization. if anybody can
    * optimalize it, do so!
    */
	 for (y = 0; y < bitmap->h; y++) {
	    memcpy(pixels + y * pitch, bitmap->line[y], bitmap->w);
	 }
   return pixels;
}

#if 0
HBITMAP convert_bitmap_to_hbitmap2(BITMAP * bitmap)
{
   HDC hdc;
   HBITMAP hbmp;
   BITMAPINFO *bi;
   HPALETTE hpal, holdpal;
   BYTE *pixels;

   pixels = get_dib(bitmap);

   /* when we have DIB, we can convert it to DDB */
   hdc = GetDC(NULL);

   bi = get_bitmap_info(bitmap, _current_palette);

   hpal = convert_palette_to_hpalette(_current_palette);
   holdpal = SelectPalette(hdc, hpal, TRUE);
   RealizePalette(hdc);
   hbmp = CreateDIBitmap(hdc, &bi->bmiHeader, CBM_INIT, pixels, bi, DIB_RGB_COLORS);
   ReleaseDC(NULL, hdc);

   SelectPalette(hdc, holdpal, TRUE);
   DeleteObject(hpal);

   free(pixels);
   free(bi);

   return hbmp;
}
#endif

void copyBitmapToClipboard(BITMAP *bmp) {
#if 0
    if (OpenClipboard(win_get_window())) {
        if (EmptyClipboard()) {
            HBITMAP hbmp = convert_bitmap_to_hbitmap2(bmp);
            if (hbmp) {
                if (!SetClipboardData(CF_BITMAP, hbmp)) {
                    DeleteObject(hbmp);
                }                
                CloseClipboard();
            }
        } 
    }
#endif
}

void copy_to_clip()
{
	BITMAP * bm = create_bitmap(blXSize*8, blYSize);

	blit(bitmap, bm, 0, legend, 0, 0, blXSize*8, blYSize);

	copyBitmapToClipboard(bm);

	destroy_bitmap(bm);

}

void refresh()
{
	clear_to_color(bitmap, 253);

	text_mode(-1);

	numX = scrW/(blXSize*8+hole);
	numY = (scrH-legend)/(blYSize+hole);
	
	if (numX == 0)
	{
		numX = 1;
	}
	if (numY == 0)
	{
		numY= 1;
	}
	char rev;
	if (reverse)
	{
		rev = 'R';
	}
	else
	{
		rev = 'N';
	}

	char moder[3];
	switch (mode)
	{
	case 0:
		strcpy(moder, "AM");
		break;
	case 1:
		strcpy(moder, "ST");
		break;
	case 2:
		strcpy(moder, "SP");
		break;
	case 3:
		strcpy(moder, "C+");
		break;
	case 4:
		strcpy(moder, "C-");
		break;
	}
	if (skipmode == 0)
	{
		skipmoder = 'P';
	}
	else
	{
		skipmoder = 'B';
	}
	textprintf(bitmap, font, 0, 0, 254, "Off:%7i Siz: X:%4i Y:%4i Bit:%i Vis.X:%2i Y:%2i Pal%c:%7i Mode:%s Skip-%c:%4i Order:%c %i%i%i%i%i%i%i%i", offset,blXSize*8, blYSize, bits, numX, numY, pmoder[palsearchmode], palfound, moder, skipmoder, skip, rev,
		bplorder[7], bplorder[6], bplorder[5], bplorder[4], bplorder[3], bplorder[2], bplorder[1], bplorder[0] );
	
	drawstuff();

	if (zoom)
	{
		draw_zoom();
	}

	blit(bitmap, screen, 0, 0, 0, 0, scrW, scrH);		
}

void genpal()
{
	srand(1);
	for (int i=0; i < 256; i++)
	{
		pal[i].r = rand()%64;
		pal[i].g = rand()%64;
		pal[i].b = rand()%64;
	}
	pal[1].r = pal[1].g = pal[1].b = 0;
	pal[0].r = pal[0].g = pal[0].b = 63;
	pal[254].r = pal[254].g = pal[254].b = 0;
	pal[253].r = pal[253].g = pal[253].b = 50;
	pal[253].b = 53;
}
int parse_number(char * str)
{
	int t;

	if (str[0] == 0)
	{
		return 0;
	}
	sscanf(str, "%d", &t);
	return t;
}

void goto_bye()
{
	sprintf(gotottt, "Go To BYTE:");

	sprintf(goto_num, "%i", offset);
	unscare_mouse();
	
	init_moveable(goto_dialog);

	show_mouse(screen);

	do_dialog (goto_dialog, 0);
	
	shutdown_moveable(goto_dialog);

	scare_mouse();

	offset = parse_number(goto_num);
}

void skip_bye()
{
	sprintf(gotottt, "Skip Value:");
	
	sprintf(goto_num, "%i", offset);
	unscare_mouse();
	
	init_moveable(goto_dialog);
	
	show_mouse(screen);
	
	do_dialog (goto_dialog, 0);
	
	shutdown_moveable(goto_dialog);
	
	scare_mouse();
	
	skip = parse_number(goto_num);
}

void prepare_save_name()
{
	char temp[256];
	char temp2[256];
	int t;
	bool ok;

	strcpy(temp, save_name);
	t = strlen(temp);

	while (t >= 0 && temp[t] != '.')
	{
		t--;
	}
	temp[t] = 0;
	ok = false;
	if (t > 3)
	{
		if (temp[t-1] >= '0' && temp[t-1] <= '9' &&
			temp[t-2] >= '0' && temp[t-2] <= '9' &&
			temp[t-3] >= '0' && temp[t-3] <= '9')
		{
			ok = true;
		}
	}
	if (!ok)
	{
		strcat(temp, "000.bmp");
		contin_save = 1;
	}
	else
	{
		sprintf(temp2, "%03i.bmp", contin_save);
		temp[t-3] = 0;
		strcat(temp, temp2);
		contin_save++;
	}
	strcpy(save_name,temp);
}

void save(bool cont)
{
	int t;

	if (cont)
	{
		prepare_save_name();
	}

	unscare_mouse();
	
	init_moveable(save_dialog);
	
	show_mouse(screen);
	
	t = do_dialog (save_dialog, 0);
	
	shutdown_moveable(save_dialog);
	
	scare_mouse();

	if (t != 1)
	{
		int numX = parse_number(save_x);
		int numY = parse_number(save_y);
		int border = parse_number(save_border);
		int bcolor = parse_number(save_bcolor);
		if (numX < 1)
		{
			numX = 1;
		}
		if (numY < 1)
		{
			numY = 1;
		}
		bool direction = false;
		if (save_dialog[9].flags & D_SELECTED)
		{
			direction = true;
		}
		int xs = (blXSize*8+border)*numX;
		int ys = (blYSize+border)*numY;

		BITMAP * temp = create_bitmap(xs, ys);

		clear_to_color(temp, bcolor);

		int xx, yy;		
		int pos = offset;

		if (!direction)
		{
			for (int x=0; x < numX; x++)
			{
				for (int y=0; y < numY; y++)
				{
					xx = x*(blXSize*8+border);
					yy = y*(blYSize+border);
					
					drawbitmap(temp, pos, xx, yy);
					pos+=(blXSize*blYSize)*bits+skip;
				}
			}
		}
		else
		{
			for (int y=0; y < numY; y++)
			{
				for (int x=0; x < numX; x++)
				{
					xx = x*(blXSize*8+border);
					yy = y*(blYSize+border);
					
					drawbitmap(temp, pos, xx, yy);
					pos+=(blXSize*blYSize)*bits+skip;
				}
			}
		}

		if (exists(save_name))
		{
			if ( billalert("Question!",
				"This file already exists !",
				"Do you want to overwrite it ?",
				"NO", "YES", 27, 0) == 2)
			{
				save_bmp(save_name, temp, pal);		
				destroy_bitmap(temp);
			}
			return;
		}
		save_bmp(save_name, temp, pal);		
		destroy_bitmap(temp);
	}
}

void grab_uae()
{
	// Simpler method to find palettes !
	bool found = false;
	while (!found && ttt < fsize-64)
	{
		if (mem[ttt] == 0x01 && mem[ttt+1] == 0x80 && mem[ttt+4] == 0x01 && mem[ttt+5] == 0x82)
		{
			found = true;
			palfound = ttt;
			int c1, c2;
			int r,g,b;
			for (int i=0; i < 32; i++)
			{
				c1 = mem[ttt+2+i*4];
				c2 = mem[ttt+3+i*4];
				r = (c1&0x0f)<<2;
				g = (c2&0xf0)>>2;
				b = (c2&0x0f)<<2;

				r = r|((r>>2)&3);
				g = g|((g>>2)&3);
				b = b|((b>>2)&3);
				
				pal[i].r = r;
				pal[i].g = g;
				pal[i].b = b;
			}
			set_pallete(pal);			
		}
		ttt++;
	}
	if (!found)
	{
		ttt = 0;
	}
}

void grab_uae2()
{
	// More advanced method to find palettes !
	bool colfound[32];
	bool nomorecols = false;
	int foundcolors = 0;
	
	for (int i=0; i < 32; i++)
	{
		colfound[i] = false;
	}
	bool found = false;
	while (!found && ttt < fsize-64)
	{
		if (mem[ttt+0] == 0x01 && mem[ttt+1] >= 0x80 && mem[ttt+1] <=0x9f &&
			mem[ttt+4] == 0x01 && mem[ttt+5] >= 0x80 && mem[ttt+5] <=0x9f &&
			mem[ttt+8] == 0x01 && mem[ttt+9] >= 0x80 && mem[ttt+9] <=0x9f &&
			mem[ttt+12] == 0x01 && mem[ttt+13] >= 0x80 && mem[ttt+13] <=0x9f)
		{
			found = true;
			palfound = ttt;
			int c1, c2;
			int r,g,b;
			
			int i=0;
			int colind = 0;
			bool ended = false;

			while (ttt+(i*4) < fsize-2 && !ended)
			{
				if (mem[ttt+(i*4)+0] == 0xff && mem[ttt+(i*4)+1] == 0xff)
				{
					ended = true;
				}
				else
				{
					if (mem[ttt+1+(i*4)] >= 0x80 && mem[ttt+1+(i*4)] <= 0xbf)
					{
						colind = (mem[ttt+1+(i*4)]-0x80)>>1;
						if (!colfound[colind])
						{
							colfound[colind] = true;
							c1 = mem[ttt+2+(i*4)];
							c2 = mem[ttt+3+(i*4)];
							r = (c1&0x0f)<<2;
							g = (c2&0xf0)>>2;
							b = (c2&0x0f)<<2;
							
							r = r|((r>>2)&3);
							g = g|((g>>2)&3);
							b = b|((b>>2)&3);

							pal[colind].r = r;
							pal[colind].g = g;
							pal[colind].b = b;
							if (!nomorecols)
							{
								foundcolors+=4;
							}
						}
						else
						{
							nomorecols = true;
						}
					}
					else
					{
						nomorecols = true;
					}
				}
				i++;
			}
			set_pallete(pal);			
		}
		if (foundcolors > 0)
		{
			ttt+=foundcolors;
		}
		else
		{
			ttt++;
		}
	}
	if (!found)
	{
		ttt = 0;
	}
}

int find_chunk(char * name)
{
	int pos = 0;
	int add = 0;
	int len2;
	while (pos < fsize)
	{
		if (!memcmp(name, &mem[pos], 4))
		{
			// found
			return pos;
		}
		add = mem[pos+4]*256*256*256+mem[pos+5]*256*256+mem[pos+6]*256+mem[pos+7]+4;
		pos += add;
		/* alignment */
		len2 = add & 3;
		pos -= len2;
		
	}
	return -1;
}

void grab_uae3()
{
	// Grab the palette from the snapshot
	int pos;
	pos = find_chunk("CHIP");
	if (pos != -1)
	{
		palfound = pos+4+8+4+0x0180 - (64 + 12*8);
		for (int i=0; i < 63; i++)
		{
			int c1, c2;
			int r,g,b;

			c1 = mem[palfound+i*2];
			c2 = mem[palfound+i*2+1];
			r = (c1&0x0f)<<2;
			g = (c2&0xf0)>>2;
			b = (c2&0x0f)<<2;
			
			r = r|((r>>2)&3);
			g = g|((g>>2)&3);
			b = b|((b>>2)&3);

			pal[i].r = r;
			pal[i].g = g;
			pal[i].b = b;
		}
		set_pallete(pal);			
	}
}

void grab_uae3aga()
{
	// Grab the AGA palette from the snapshot
	int pos;
	pos = find_chunk("AGAC");
	if (pos != -1)
	{
		palfound = pos+8+4;
		for (int i=0; i < 256; i++)
		{
			int r,g,b;

			r = mem[palfound+i*4+1];
			g = mem[palfound+i*4+2];
			b = mem[palfound+i*4+3];

			pal[i].r = r>>2;
			pal[i].g = g>>2;
			pal[i].b = b>>2;
		}
		set_pallete(pal);			
	}
}


void getpalettefile()
{
	BITMAP * t;

	FILE * f;
	char buf[255] = "";
	int ret = billfile_select("Select BMP file", buf, "bmp");
	if (ret != 0)
	{
		f = fopen(buf, "rt");
		if (f != NULL)
		{
			t = load_bmp(buf, pal);
			if (t != NULL)
			{
				set_pallete(pal);	
				destroy_bitmap(t);				
			}
		}
	}
}

int main(int argc, char *argv[])
{

	install_allegro(SYSTEM_AUTODETECT, &errno, atexit);

	
	install_mouse();
	if(install_keyboard())
	{
		allegro_exit();
		allegro_message("Error installing keyboard handler.\nExiting");
		return -1;
	}
	set_color_depth(8);
	if(0 > set_gfx_mode(GFX_SAFE, scrW, scrH, 0, 0))
	{
		allegro_exit();
		allegro_message("Error setting graphics mode.\nExiting");
		return -1;
	}
	genpal();
	
	set_pallete(pal);	
	
	bill_init();
	//		bill_init_smallpal(17);		
	set_gui_mode(1);

	char buf[255] = "";
	
	if (argc < 2)
	{
		billfile_select("Select file", buf, NULL);
	}
	else
	{
		strcpy(buf, argv[1]);
	}
	file = fopen(buf, "rb");
	if (file != NULL)
	{
		fsize = file_size(buf);
		if (fsize !=0)
		{
			mem = (unsigned char *) malloc(fsize);
			if (mem != 0)
			{
				fread((void *) mem, 1, fsize, file);
			}
		}
	}
	
	if (mem != 0)
	{
		bitmap = create_bitmap(scrW, scrH);		
		
		int keyp = 0;
	
		centre_dialog(goto_dialog);
		centre_dialog(save_dialog);
		
		while (keyp != KEY_ESC)
		{
			refresh();
			keyp = readkey();
			keyp>>=8;
			if (keyp >= KEY_0 && keyp <= KEY_8)
			{
				int nn = keyp-KEY_0;
				if (key[KEY_LSHIFT] || key[KEY_RSHIFT])
				{
					bplorder[nn]--;
					if (bplorder[nn] < 0)
					{
						bplorder[nn]=7;
					}
				}
				else
				{
					bplorder[nn]++;
					if (bplorder[nn] > 7)
					{
						bplorder[nn]=0;
					}
				}
			}
			int modeskip = 1;
			if (mode == 1)
			{
				modeskip = bits;
			}
			switch(keyp)
			{
			case KEY_B:
				pal[0].r = pal[0].b = 63;
				pal[0].g = 0;
				set_pallete(pal);	
				break;
			case KEY_MINUS:
				if (bplorder[0]>0)
				{
					bplorder[0]--;
				}
				else
				{
					bplorder[0]=7;
				}
				break;
			case KEY_UP:
				if (key[KEY_LSHIFT] || key[KEY_RSHIFT])
				{
					if (offset >= (blXSize*8*modeskip) )
					{
						offset-=blXSize*8*modeskip;
					}
				}
				else
				{
					if (offset >= blXSize*modeskip )
					{
						offset-=blXSize*modeskip;
					}
				}
				break;
			case KEY_DOWN:
				if (key[KEY_LSHIFT] || key[KEY_RSHIFT])
				{
					if (offset < (fsize-blXSize*8*modeskip))
					{
						offset+=blXSize*8*modeskip;
					}
				}
				else
				{
					if (offset < fsize-(blXSize*modeskip))
					{
						offset+=blXSize*modeskip;
					}
				}
				break;
			case KEY_LEFT:
				if (key[KEY_LSHIFT] || key[KEY_RSHIFT])
				{
					if (offset >= (blXSize*blYSize*bits)+skip )
					{
						offset-=(blXSize*blYSize*bits)+skip;
					}
				}
				else
				{
					if (offset > 0)
					{
						offset--;
					}
				}
				break;
			case KEY_RIGHT:
				if (key[KEY_LSHIFT] || key[KEY_RSHIFT])
				{
					if (offset < fsize-((blXSize*blYSize*bits)+skip))
					{
						offset+=(blXSize*blYSize*bits)+skip;
					}
				}
				else
				{
					if (offset < fsize)
					{
						offset++;
					}
				}
				break;
			case KEY_E:
				if (bits < 8)
				{
					bits++;
				}
				break;
			case KEY_D:
				if (bits > 1)
				{
					bits--;
				}
				break;
			case KEY_W:
				if (key[KEY_LSHIFT] || key[KEY_RSHIFT])
				{
					blYSize+=8;
				}
				else
				{
					blYSize++;
				}
				break;
			case KEY_S:
				if (key[KEY_LSHIFT] || key[KEY_RSHIFT])
				{
					if (blYSize > 9)
					{
						blYSize-=8;
					}
				}
				else
				{
					if (blYSize > 1)
					{
						blYSize--;
					}
				}
				break;
			case KEY_Q:
				blXSize++;
				break;
			case KEY_A:
				if (blXSize > 1)
				{
					blXSize--;
				}
				break;
			case KEY_G:
				goto_bye();
				break;				
			case KEY_H:
				skip_bye();
				break;
				
			case KEY_P:
				if (key[KEY_LSHIFT] || key[KEY_RSHIFT])
				{
					save(true);
				}
				else
				{
					save(false);
				}
				break;
			case KEY_L:
				switch (palsearchmode)
				{
				case 0:
					grab_uae();
					break;
				case 1:
					grab_uae2();
					break;
				case 2:
					grab_uae3();
					break;
				case 3:
					grab_uae3aga();
					break;
				}
				break;
			case KEY_J:
				ttt = palfound;
				switch (palsearchmode)
				{
				case 0:
					grab_uae();
					break;
				case 1:
					grab_uae2();
					break;
				case 2:
					grab_uae3();
					break;
				case 3:
					grab_uae3aga();
					break;
				}
				break;
			case KEY_K:
				genpal();				
				set_pallete(pal);	
				break;
			case KEY_U:
				zoom = !zoom;
				break;
			case KEY_R:
				if (key[KEY_LSHIFT] || key[KEY_RSHIFT])
				{
					skip+=8;
				}
				else
				{
					skip++;
				}
				break;
			case KEY_F:
				if (key[KEY_LSHIFT] || key[KEY_RSHIFT])
				{
					if (skip > 7)
					{
						skip-=8;
					}
				}
				else
				{
					if (skip > 0)
					{
						skip--;
					}
				}
				break;
			case KEY_N:
				reverse = !reverse;
				break;
			case KEY_M:
				mode+=1;
				if (mode == 5)
				{
					mode = 0;
				}
				break;
			case KEY_Z:
				palsearchmode+=1;
				if (palsearchmode == 4)
				{
					palsearchmode = 0;
				}
				break;
			case KEY_C:
				getpalettefile();
				break;
			case KEY_X:
				if (skipmode == 0)
				{
					skipmode = 1;
				}
				else
				{
					skipmode = 0;
				}
				break;
			case KEY_INSERT:
				if (key[KEY_LCONTROL] || key[KEY_RCONTROL])
				{
					copy_to_clip();
				}
				break;
			}
			clear_keybuf();
		}

		destroy_bitmap(bitmap);
		free (mem);
	}
	return 0;
}
END_OF_MAIN()
