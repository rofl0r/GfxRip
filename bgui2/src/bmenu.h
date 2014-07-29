typedef struct MENU_INFO            /* information about a popup menu */
{
   MENU *menu;                      /* the menu itself */
   struct MENU_INFO *parent;        /* the parent menu, or NULL for root */
   int bar;                         /* set if it is a top level menu bar */
   int size;                        /* number of items in the menu */
   int sel;                         /* selected item */
   int x, y, w, h;                  /* screen position of the menu */
   int (*proc)();                   /* callback function */
   BITMAP *saved;                   /* saved what was underneath it */
} MENU_INFO;

static void fill_menu_info(MENU_INFO *m, MENU *menu, MENU_INFO *parent, int bar, int x, int y, int minw, int minh);
static void draw_billmenu_item(MENU_INFO *m, int c);
static void draw_billmenu(MENU_INFO *m);
int _do_billmenu(MENU *menu, MENU_INFO *parent, int bar, int x, int y, int repos, int *dret, int minw, int minh);
int billmenu_alt_key(int k, MENU *m);
