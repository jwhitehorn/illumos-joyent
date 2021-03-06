/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright (c) 1992, 1993, 1994, 2000 by Sun Microsystems, Inc.
 * All rights reserved.
 */

#ifndef	_META_SET_COM_H
#define	_META_SET_COM_H

#pragma ident	"%Z%%M%	%I%	%E% SMI"

#include <meta.h>
#include <ctype.h>
#include <sys/mnttab.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* meta_lib_prv.c */
extern	FILE		*open_mnttab(void);
extern	int		close_mnttab(void);

#ifdef	__cplusplus
}
#endif

#endif	/* _META_SET_COM_H */
