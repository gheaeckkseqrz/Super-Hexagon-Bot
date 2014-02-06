/*
** main.c for X11WindowList in /home/wilmot_p/PROG/C++/SuperHexagon
** 
** Made by WILMOT Pierre
** Login   <pierre.wilmot@gmail.com>
** 
** Started on  Sat Dec 21 22:59:48 2013 WILMOT Pierre
** Last update Mon Dec 23 10:18:01 2013 WILMOT Pierre
*/

#include	<X11/Xlib.h>
#include	<stdio.h>
#include	<errno.h>
#include	<X11/Xatom.h>

char		*winame(Display *disp, Window win);
void		wingeo(Display *disp, Window win, int *x, int *y,
			unsigned int *w, unsigned int *h);

int    		main()
{
  Display *display = XOpenDisplay(NULL);
  int screen = DefaultScreen(display);
  Window rootwindow = RootWindow(display, screen);

  Window root_return;
  Window parent_return;
  Window *children_return;
  unsigned int nchildren_return;

  XQueryTree(display, rootwindow, &root_return, &parent_return, &children_return, &nchildren_return);
  printf("XQueryTree returned %d children\n", nchildren_return);

  unsigned int i = 0;
  char *winName;
  int x, y;
  unsigned int w, h;
  for (i = 0 ; i < nchildren_return ; ++i)
    {
      winName = winame(display, children_return[i]);
      wingeo(display, children_return[i], &x, &y, &w, &h);
      if (winName)
      	printf ("[%d] - %s - %d - (%d, %d, %d, %d)\n", i, winName, (unsigned int)children_return[i], x, y, w, h);
      else
      	printf ("[%d] - %s - %d - (%d, %d, %d, %d)\n", i, "NULL", (unsigned int)children_return[i], x, y, w, h);
    }

  return 0;
}

char		*winame (Display *disp, Window win)
{
  Atom prop = XInternAtom(disp,"WM_NAME",False), type;
  int form;
  unsigned long remain, len;
  unsigned char *list;

  errno = 0;
  if ( XGetWindowProperty(disp, win, prop, 0, 1024, False, XA_STRING, &type, &form, &len, &remain, &list) != Success)
    {
      perror("winname() -- GetWinProp");
      return NULL;
    }
  return (char*)list;
}

void		 wingeo(Display *disp, Window win, int *x, int *y,
			unsigned int *w, unsigned int *h)
{
  Window root_return;
  unsigned int border, depth;

  if (XGetGeometry(disp, win, &root_return, x, y, w, h, &border, &depth) != Success)
    {
      //      perror("wingeo() -- XGetGeometry");
      return;
    }
}
