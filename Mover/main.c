#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/extensions/XTest.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

KeySym *getKeySim(char a, Display *d);

int main(int ac, char **av)
{
  (void)ac;
  (void)av;


  int i1, i2, i3, i4;

  Display *d = XOpenDisplay(NULL);
  if (d == NULL)
    {
      printf("Can't open display\n");
      return 1;
    }

  if (!XTestQueryExtension(d, &i1, &i2, &i3, &i4))
    {
      printf("Not supported\n");
      return 0;
    }

  char current;
  int keycode = 0;
  while (1)
    {
      char k = getchar();
      if (k != current)
	{
	  if (keycode != 0)
	    {
	      if (XTestFakeKeyEvent(d, XKeysymToKeycode(d, *getKeySim(keycode, d)), False, CurrentTime) == 0)
		printf("XTestFakeKeyEvent Failled\n");
	    }

	  if (k == 'a')
	    keycode = 38;
	  if (k == 'd')
	    keycode = 40;
	  if (k == ' ')
	    keycode = 50;


	  if (keycode != 0)
	    {
	      if (XTestFakeKeyEvent(d, XKeysymToKeycode(d, *getKeySim(keycode, d)), True, CurrentTime) == 0)
		printf("XTestFakeKeyEvent Failled\n");
	      XFlush(d);
	    }
	  current = k;
	}
    }
}

KeySym *getKeySim(char a, Display *d)
{
  int keysyms_per_keycode_return;
  KeySym *keysym = XGetKeyboardMapping(d, a, 1, &keysyms_per_keycode_return);

  //  printf("KeySim for '%c' : %u || ret : %d\n", a, *keysym, keysyms_per_keycode_return);
  return keysym;
}
