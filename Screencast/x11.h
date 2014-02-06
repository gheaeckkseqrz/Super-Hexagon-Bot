/*
** x11.h for SuperHexagonBot in /home/wilmot_p/PROG/C++/SuperHexagon/Screencast
** 
** Made by WILMOT Pierre
** Login   <pierre.wilmot@gmail.com>
** 
** Started on  Mon Dec 23 02:01:15 2013 WILMOT Pierre
** Last update Mon Dec 23 02:13:25 2013 WILMOT Pierre
*/

#ifndef	MX11_H
#define	MX11_H

#include	<X11/Xlib.h>

char		*winame(Display *disp, Window win);
void		wingeo(Display *disp, Window win, int *x, int *y,
			unsigned int *w, unsigned int *h);
Window		getSuperHexagonWindow();

#endif
