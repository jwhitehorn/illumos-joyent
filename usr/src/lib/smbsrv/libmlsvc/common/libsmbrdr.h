/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
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
 * Copyright 2011 Nexenta Systems, Inc.  All rights reserved.
 * Copyright 2009 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#ifndef	_LIBSMBRDR_H
#define	_LIBSMBRDR_H

#include <smbsrv/libsmb.h>

#ifdef	__cplusplus
extern "C" {
#endif

void smbrdr_init(void);

struct smb_ctx;
int smbrdr_ctx_new(struct smb_ctx **, char *, char *, char *);
void smbrdr_ctx_free(struct smb_ctx *);

/* Redirector LOGON function */
extern int smbrdr_logon(char *, char *, char *);
extern int smbrdr_get_ssnkey(int, unsigned char *, size_t);

/* Redirector session functions */
extern void smbrdr_disconnect(const char *);

#ifdef	__cplusplus
}
#endif

#endif	/* _LIBSMBRDR_H */
