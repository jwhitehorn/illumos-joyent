/***********************************************************************
*                                                                      *
*               This software is part of the ast package               *
*          Copyright (c) 1985-2010 AT&T Intellectual Property          *
*                      and is licensed under the                       *
*                  Common Public License, Version 1.0                  *
*                    by AT&T Intellectual Property                     *
*                                                                      *
*                A copy of the License is available at                 *
*            http://www.opensource.org/licenses/cpl1.0.txt             *
*         (with md5 checksum 059e8cd6165cb4c31e351f2b69388fd9)         *
*                                                                      *
*              Information and Software Systems Research               *
*                            AT&T Research                             *
*                           Florham Park NJ                            *
*                                                                      *
*                 Glenn Fowler <gsf@research.att.com>                  *
*                  David Korn <dgk@research.att.com>                   *
*                   Phong Vo <kpv@research.att.com>                    *
*                                                                      *
***********************************************************************/
#pragma prototyped

#include "stdhdr.h"

wchar_t*
fgetws(wchar_t* s, int n, Sfio_t* f)
{
	register wchar_t*	p = s;
	register wchar_t*	e = s + n - 1;
	register wint_t		c;

	STDIO_PTR(f, "fgets", wchar_t*, (wchar_t*, int, Sfio_t*), (s, n, f))

	FWIDE(f, 0);
	while (p < e && (c = fgetwc(f)) != WEOF && (*p++ = c) != '\n');
	*p = 0;
	return s;
}

wchar_t*
getws(wchar_t* s)
{
	register wchar_t*	p = s;
	register wchar_t*	e = s + BUFSIZ - 1;
	register wint_t		c;

	FWIDE(sfstdin, 0);
	while (p < e && (c = fgetwc(sfstdin)) != WEOF && (*p++ = c) != '\n');
	*p = 0;
	return s;
}
