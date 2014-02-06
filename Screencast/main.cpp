/*
** main.c<2> for SCRENNSHOT in /home/wilmot_p/SCREENCASTER/VALA
** 
** Made by WILMOT Pierre
** Login   <pierre.wilmot@gmail.com>
** 
** Started on  Wed Oct  9 18:50:45 2013 WILMOT Pierre
// Last update Sat Jan 18 16:54:04 2014 WILMOT Pierre
*/

#include	<sstream>
#include	<glib-object.h>
#include	<stdlib.h>
#include	<string.h>
#include	<gtk/gtk.h>
#include	<gdk/gdk.h>
#include	<gdk-pixbuf/gdk-pixbuf.h>
#include	<iostream>
#include	<opencv/highgui.h>
#include	<gdk/gdkx.h>

#include	"x11.h"

int		main(int ac, char **av)
{
  int		width;
  int		height;

  GdkPixbuf	*screenshot;

  char		*pixelData;

  gtk_init(&ac, &av);

  GdkWindow *SHWin = gdk_x11_window_foreign_new_for_display(gdk_display_get_default(), getSuperHexagonWindow());
  width = gdk_window_get_width(SHWin);
  height = gdk_window_get_height(SHWin);

  std::cout << "Screen resolution : " << std::endl;
  std::cout << width << std::endl;
  std::cout << height << std::endl;

  GError	*error = NULL;
  int		i = 0;
  while (1 && i < 500)
    {
      std::string handle = "img/Screen_";
      handle += std::to_string(i);
      handle += ".jpeg";
      screenshot = gdk_pixbuf_get_from_window (SHWin, 0, 0, width, height);
      //gdk_pixbuf_save(screenshot, handle.c_str(), "jpeg", &error, NULL);
      ++i;
      pixelData = (char *)gdk_pixbuf_get_pixels (screenshot);
      write(1, pixelData, width * height * 3);
      g_object_unref(G_OBJECT(screenshot));
      usleep(1000/60);
    }
  return 0;
}
