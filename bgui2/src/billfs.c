/*
 * BGui version 2.0 unoffical repackage and rerelease by Jason Winnebeck.
 * Original author: Dan Huizenga (Skis)
 *
 */

/*
 * Most of this code is taken from Allegro.  This file contains the BGUI
 * file selector.
 *
 */


#include "allegro.h"
#include "tdgui.h"

#ifdef ALLEGRO_DOS
   #include <dos.h>
#endif


#if (DEVICE_SEPARATOR != 0) && (DEVICE_SEPARATOR != '\0')
   #define HAVE_DIR_LIST
#endif


static int fs_edit_proc(int, DIALOG *, int);
static int fs_flist_proc(int, DIALOG *, int);
static char *fs_flist_getter(int, int *);

#ifdef HAVE_DIR_LIST
   static int fs_dlist_proc(int, DIALOG *, int);
   static char *fs_dlist_getter(int, int *);
#endif


#define FLIST_SIZE      2048

typedef struct FLIST
{
   char dir[512];
   int size;
   char *name[FLIST_SIZE];
} FLIST;

static FLIST *flist = NULL;

static char *fext = NULL;

static DIALOG file_selector[] =
{
   #ifdef HAVE_DIR_LIST

      /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp)              (dp2) (dp3) */
      { d_billwin_proc,     0,    0,   304,  160,    0,    0,    0,    0,       0,    0,    NULL,             NULL, NULL  },
      { d_billbutton_proc,208,  107,    80,   16,    0,    0,    0,    D_EXIT,  0,    0,    NULL,             NULL, NULL  },
      { d_billbutton_proc,208,  129,    80,   16,    0,    0,   27,    D_EXIT,  0,    0,    NULL,             NULL, NULL  },
      { fs_edit_proc,      16,   28,   272,   12,    0,    0,    0,    0,       79,   0,    NULL,             NULL, NULL  },
      { fs_flist_proc,     16,   46,   176,   99,    0,    0,    0,    D_EXIT,  0,    0,    fs_flist_getter,  NULL, NULL  },
      { fs_dlist_proc,     208,  46,    80,   51,    0,    0,    0,    D_EXIT,  0,    0,    fs_dlist_getter,  NULL, NULL  },

   #else

      /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key) (flags)  (d1)  (d2)  (dp)              (dp2) (dp3) */
      { d_billwin_proc, 0,    0,    304,  188,  0,    0,    0,    0,       0,    0,    NULL,             NULL, NULL  },
      { d_billbutton_proc,     64,   160,  80,   16,   0,    0,    0,    D_EXIT,  0,    0,    NULL,             NULL, NULL  },
      { d_billbutton_proc,     160,  160,  80,   16,   0,    0,    27,   D_EXIT,  0,    0,    NULL,             NULL, NULL  },
      { fs_edit_proc,      16,   28,   272,  12,    0,    0,    0,    0,       79,   0,    NULL,             NULL, NULL  },
      { fs_flist_proc,     16,   46,   272,  99,   0,    0,    0,    D_EXIT,  0,    0,    fs_flist_getter,  NULL, NULL  },

   #endif

   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};

#define FS_MESSAGE      0
#define FS_OK           1
#define FS_CANCEL       2
#define FS_EDIT         3
#define FS_FILES        4

#ifdef HAVE_DIR_LIST       /* not all platforms need a directory list */

#define FS_DISKS        6



/* drive_exists:
 *  Checks whether the specified drive is valid.
 */
static int drive_exists(int x)
{
   #ifdef ALLEGRO_DOS

      /* DOS implementation */
      unsigned int old, ret, tmp;

      x++;

      _dos_getdrive(&old);
      _dos_setdrive(x, &tmp);
      _dos_getdrive(&ret);
      _dos_setdrive(old, &tmp); 

      return (ret == (unsigned)x);

   #else

      /* unknown platform */
      return TRUE;

   #endif
}



/* count_disks:
 *  Counts the number of valid drives.
 */
static int count_disks()
{
   int c, i;

   c = 0;

   for (i=0; i<26; i++)
      if (drive_exists(i))
	 c++;

   return c;
}



/* get_x_drive:
 *  Returns the drive letter matching the specified list index.
 */
static int get_x_drive(int index)
{
   int c, i;

   c = 0;

   for (i=0; i<26; i++) {
      if (drive_exists(i)) {
	 c++;
	 if (c==index)
	    return i;
      }
   }

   return -1;
}



/* fs_dlist_getter:
 *  Listbox data getter routine for the file selector disk list.
 */
static char *fs_dlist_getter(int index, int *list_size)
{
   static char d[8];
   int pos, c;

   if (index < 0) {
      if (list_size)
	 *list_size = count_disks();
      return NULL;
   }

   c = 'A' + get_x_drive(index+1);
   if ((c < 'A') || (c > 'Z'))
      c = 'C';

   pos = usetc(d, c);
   pos += usetc(d+pos, DEVICE_SEPARATOR);
   pos += usetc(d+pos, OTHER_PATH_SEPARATOR);
   usetc(d+pos, 0);

   return d;
}



/* fs_dlist_proc:
 *  Dialog procedure for the file selector disk list.
 */
static int fs_dlist_proc(int msg, DIALOG *d, int c)
{
   char *s = file_selector[FS_EDIT].dp;
   int ret, i, temp;

   if (msg == MSG_START) {
      d->d1 = d->d2 = 0;
      temp = utoupper(ugetc(s));
      if (((temp >= 'A') && (temp <= 'Z')) && (ugetat(s, 1) == DEVICE_SEPARATOR)) {
	 temp -= 'A';
	 for (i=0; i<temp; i++)
	    if (drive_exists(i))
	       d->d1++;
      }
   }

   ret = d_billlist_proc(msg, d, c);

   if (ret == D_CLOSE) {
      temp = 'A' + get_x_drive(d->d1+1);
      if ((temp < 'A') || (temp > 'Z'))
	 temp = 'C';

      s += usetc(s, temp);
      s += usetc(s, DEVICE_SEPARATOR);
      s += usetc(s, OTHER_PATH_SEPARATOR);
      usetc(s, 0);

      scare_mouse();
      SEND_MESSAGE(file_selector+FS_FILES, MSG_START, 0);
      SEND_MESSAGE(file_selector+FS_FILES, MSG_DRAW, 0);
      SEND_MESSAGE(file_selector+FS_EDIT, MSG_START, 0);
      SEND_MESSAGE(file_selector+FS_EDIT, MSG_DRAW, 0);
      unscare_mouse();

      return D_O_K;
   }

   return ret;
}



#endif      /* HAVE_DIR_LIST */



/* fs_edit_proc:
 *  Dialog procedure for the file selector editable string.
 */
static int fs_edit_proc(int msg, DIALOG *d, int c)
{
   char *s = d->dp;
   char b[512];
   int ch, attr;

   if (msg == MSG_START) {
      fix_filename_path(b, s, sizeof(b));
      ustrcpy(s, b);
   }

   if (msg == MSG_KEY) {
      if ((!ugetc(s)) || (ugetat(s, -1) == DEVICE_SEPARATOR))
	 ustrcat(s, uconvert_ascii("./", NULL));

      fix_filename_path(b, s, sizeof(b));
      ustrcpy(s, b);

      ch = ugetat(s, -1);
      if ((ch != '/') && (ch != OTHER_PATH_SEPARATOR)) {
	 if (file_exists(s, FA_RDONLY | FA_HIDDEN | FA_DIREC, &attr)) {
	    if (attr & FA_DIREC)
	       put_backslash(s);
	    else
	       return D_CLOSE;
	 }
	 else
	    return D_CLOSE;
      }

      scare_mouse();
      SEND_MESSAGE(file_selector+FS_FILES, MSG_START, 0);
      SEND_MESSAGE(file_selector+FS_FILES, MSG_DRAW, 0);
      SEND_MESSAGE(d, MSG_START, 0);
      SEND_MESSAGE(d, MSG_DRAW, 0);
      unscare_mouse();

      return D_O_K;
   }

   if (msg == MSG_UCHAR) {
      if ((c >= 'a') && (c <= 'z')) {
	 if (!ALLEGRO_LFN)
	    c = utoupper(c);
      }
      else if (c == '/') {
	 c = OTHER_PATH_SEPARATOR;
      }
      else if (ALLEGRO_LFN) {
	 if ((c > 127) || (c < 32))
	    return D_O_K;
      }
      else {
	 if ((c != OTHER_PATH_SEPARATOR) && (c != '_') &&
	     (c != DEVICE_SEPARATOR) && (c != '.') &&
	     ((c < 'A') || (c > 'Z')) && ((c < '0') || (c > '9')))
	    return D_O_K;
      }
   }

   return d_billedit_proc(msg, d, c); 
}



/* ustrfilecmp:
 *  ustricmp for filenames: makes sure that eg "foo.bar" comes before
 *  "foo-1.bar", and also that "foo9.bar" comes before "foo10.bar".
 */
static int ustrfilecmp(char *s1, char *s2)
{
   int c1, c2;
   int x1, x2;
   char *t1, *t2;

   for (;;) {
      c1 = utolower(ugetx(&s1));
      c2 = utolower(ugetx(&s2));

      if ((c1 >= '0') && (c1 <= '9') && (c2 >= '0') && (c2 <= '9')) {
	 x1 = ustrtol(s1 - ucwidth(c1), &t1, 10);
	 x2 = ustrtol(s2 - ucwidth(c2), &t2, 10);
	 if (x1 != x2)
	    return x1 - x2;
	 else if (t1 - s1 != t2 - s2)
	    return (t2 - s2) - (t1 - s1);
	 s1 = t1;
	 s2 = t2;
      }
      else if (c1 != c2) {
	 if (!c1)
	    return -1;
	 else if (!c2)
	    return 1;
	 else if (c1 == '.')
	    return -1;
	 else if (c2 == '.')
	    return 1;
	 return c1 - c2;
      }

      if (!c1)
	 return 0;
   }
}



/* fs_flist_putter:
 *  Callback routine for for_each_file() to fill the file selector listbox.
 */
static void fs_flist_putter(char *str, int attrib, int param)
{
   char ext_tokens[32];
   char *s, *ext, *tok, *name;
   char tmp[512], tmp2[32];
   int c, c2, i, k, sign;

   /* attribute flags (rhsda order)
    * 0 = not required, 1 = must be set, -1 = must be unset
    */
   int attr_flag[5+5] = {
      0, -1, -1, 0, 0,
      FA_RDONLY, FA_HIDDEN, FA_SYSTEM, FA_DIREC, FA_ARCH
   };

   c = usetc(ext_tokens, ' ');
   c += usetc(ext_tokens+c, ',');
   c += usetc(ext_tokens+c, ';');
   usetc(ext_tokens+c, 0);

   s = get_filename(str);
   fix_filename_case(s);

   if (fext) {
      ustrcpy(tmp, fext);
      ustrtok(tmp, ext_tokens);
      c = (ustrtok(NULL, ext_tokens) ? 1 : 0);
      if (!c) {
	 if (!ustrchr(fext, '/'))
	    c = 1;
      }

      if (c && (!(attrib & FA_DIREC))) {
	 ustrcpy(tmp, fext);
	 ext = get_extension(s);
	 tok = ustrtok(tmp, ext_tokens);

	 while (tok) {
	    if (ustricmp(ext, tok) == 0)
	       break;

	    tok = ustrtok(NULL, ext_tokens);
	 }

	 if (!tok)
	    return;
      }

      c = usetc(ext_tokens, ' ');
      c += usetc(ext_tokens+c, ',');
      c += usetc(ext_tokens+c, ';');
      c += usetc(ext_tokens+c, '/');
      usetc(ext_tokens+c, 0);

      ustrcpy(tmp, fext);
      tok = ustrchr(tmp, '/');

      if (tok)
	 tok = ustrtok(tok, ext_tokens);

      if (tok) {
	 sign = 1;
	 c = usetc(tmp2, 'r');
	 c += usetc(tmp2+c, 'h');
	 c += usetc(tmp2+c, 's');
	 c += usetc(tmp2+c, 'd');
	 c += usetc(tmp2+c, 'a');
	 c += usetc(tmp2+c, '+');
	 c += usetc(tmp2+c, '-');
	 usetc(tmp2+c, 0);

	 /* scan the string */
	 i = 0;
	 while ((c = utolower(ugetat(tok, i)))) {
	    k = 0;
	    while ((c2 = ugetat(tmp2, k))) {
	       if (c == c2) {
		  if (k<5) {
		     attr_flag[k] = sign;
		     break;
		  }
		  else
		     sign = (k==5) ? 1 : -1;
	       }
	       k++;
	    }
	    i++;
	 }
      }
   }

   /* check if file attributes match */
   if (!(attr_flag[3+5] & attrib)) {
      /* if not a directory, we check all attributes except FA_DIREC */
      for (c=0; c<5; c++) {
	 if (c == 3)
	    continue;
	 if ((attr_flag[c] == 1) && (!(attrib & attr_flag[c+5])))
	    return;
	 if ((attr_flag[c] == -1) && (attrib & attr_flag[c+5]))
	    return;
      }
   }
   else {
      /* if a directory, we check only FA_DIREC */
      if (attr_flag[3] == -1)
	 return;
   }

   if ((flist->size < FLIST_SIZE) && ((ugetc(s) != '.') || (ugetat(s, 1)))) {
      name = malloc(ustrsizez(s) + ((attrib & FA_DIREC) ? ucwidth(OTHER_PATH_SEPARATOR) : 0));
      if (!name)
	 return;

      for (c=0; c<flist->size; c++) {
	 if (ugetat(flist->name[c], -1) == OTHER_PATH_SEPARATOR) {
	    if (attrib & FA_DIREC)
	       if (ustrfilecmp(s, flist->name[c]) < 0)
		  break;
	 }
	 else {
	    if (attrib & FA_DIREC)
	       break;
	    if (ustrfilecmp(s, flist->name[c]) < 0)
	       break;
	 }
      }

      for (c2=flist->size; c2>c; c2--)
	 flist->name[c2] = flist->name[c2-1];

      flist->name[c] = name;
      ustrcpy(flist->name[c], s);

      if (attrib & FA_DIREC)
	 put_backslash(flist->name[c]);

      flist->size++;
   }
}



/* fs_flist_getter:
 *  Listbox data getter routine for the file selector list.
 */
static char *fs_flist_getter(int index, int *list_size)
{
   if (index < 0) {
      if (list_size)
	 *list_size = flist->size;
      return NULL;
   }

   return flist->name[index];
}



/* fs_flist_proc:
 *  Dialog procedure for the file selector list.
 */
static int fs_flist_proc(int msg, DIALOG *d, int c)
{
   static int recurse_flag = 0;
   char *s = file_selector[FS_EDIT].dp;
   char tmp[32];
   int sel = d->d1;
   int i, ret;

   if (msg == MSG_START) {
      if (!flist) {
	 flist = malloc(sizeof(FLIST));

	 if (!flist) {
	    *allegro_errno = ENOMEM;
	    return D_CLOSE; 
	 }
      }
      else {
	 for (i=0; i<flist->size; i++)
	    if (flist->name[i])
	       free(flist->name[i]);
      }

      flist->size = 0;

      replace_filename(flist->dir, s, uconvert_ascii("*.*", tmp), sizeof(flist->dir));

      for_each_file(flist->dir, FA_RDONLY | FA_DIREC | FA_ARCH | FA_HIDDEN | FA_SYSTEM, fs_flist_putter, 0);

      if (*allegro_errno)
	 billalert(NULL, get_config_text("Disk error"), NULL, get_config_text("OK"), NULL, 13, 0);

      usetc(get_filename(flist->dir), 0);
      d->d1 = d->d2 = 0;
      sel = 0;
   }

   if (msg == MSG_END) {
      if (flist) {
	 for (i=0; i<flist->size; i++)
	    if (flist->name[i])
	       free(flist->name[i]);
	 free(flist);
	 flist = NULL;
      }
   }

   recurse_flag++;
   ret = d_billlist_proc(msg, d, c);     /* call the parent procedure */
   recurse_flag--;

   if (((sel != d->d1) || (ret == D_CLOSE)) && (recurse_flag == 0)) {
      replace_filename(s, flist->dir, flist->name[d->d1], 512);
      scare_mouse();
      SEND_MESSAGE(file_selector+FS_EDIT, MSG_START, 0);
      SEND_MESSAGE(file_selector+FS_EDIT, MSG_DRAW, 0);
      unscare_mouse();

      if (ret == D_CLOSE)
	 return SEND_MESSAGE(file_selector+FS_EDIT, MSG_KEY, 0);
   }

   return ret;
}



/* file_select:
 *  Displays the Allegro file selector, with the message as caption. 
 *  Allows the user to select a file, and stores the selection in path 
 *  (which should have room for at least 80 characters). The files are
 *  filtered according to the file extensions in ext. Passing NULL
 *  includes all files, "PCX;BMP" includes only files with .PCX or .BMP
 *  extensions. Returns zero if it was closed with the Cancel button, 
 *  non-zero if it was OK'd.
 */
int billfile_select(char *message, char *path, char *ext)
{
   char buf[512];
   int ret;
   char *p;

   file_selector[FS_MESSAGE].dp = message;
   file_selector[FS_EDIT].dp = path;
   file_selector[FS_OK].dp = get_config_text("OK");
   file_selector[FS_CANCEL].dp = get_config_text("Cancel");
   fext = ext;

   if (!ugetc(path)) {
      getcwd(buf, 80);
      do_uconvert(buf, U_ASCII, path, U_CURRENT, -1);
      fix_filename_case(path);
      fix_filename_slashes(path);
      put_backslash(path);
   }

   clear_keybuf();

   do {
   } while (gui_mouse_b());

   centre_dialog(file_selector);
   set_dialog_color(file_selector, gui_fg_color, gui_bg_color);
   centre_dialog(file_selector);
   init_moveable(file_selector);
   ret = do_dialog(file_selector, FS_EDIT);
   shutdown_moveable(file_selector);

   if ((ret == FS_CANCEL) || (!ugetc(get_filename(path))))
      return FALSE;

   p = get_extension(path);
   if ((!ugetc(p)) && (ext) && (!ustrpbrk(ext, uconvert_ascii(" ,;", NULL)))) {
      p += usetc(p, '.');
      ustrcpy(p, ext);
   }

   return TRUE; 
}

