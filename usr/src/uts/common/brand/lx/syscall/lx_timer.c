/*
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 */

/*
 * Copyright 2015 Joyent, Inc.
 */

/*
 * The illumos kernel provides two clock backends: CLOCK_REALTIME, the
 * adjustable system wall clock; and CLOCK_HIGHRES, the monotonically
 * increasing time source that is not subject to drift or adjustment.  By
 * contrast, the Linux kernel is furnished with an overabundance of narrowly
 * differentiated clock types.
 *
 * Fortunately, most of the commonly used Linux clock types are either similar
 * enough to the native clock backends that they can be directly mapped, or
 * represent queries to the per-process and per-LWP microstate counters.
 */

#include <sys/time.h>
#include <sys/systm.h>
#include <sys/cmn_err.h>
#include <sys/lx_impl.h>

/*
 * From "uts/common/os/timer.c":
 */
extern int clock_settime(clockid_t, timespec_t *);
extern int clock_gettime(clockid_t, timespec_t *);
extern int clock_getres(clockid_t, timespec_t *);


static int lx_emul_clock_getres(clockid_t, timespec_t *);
static int lx_emul_clock_gettime(clockid_t, timespec_t *);
static int lx_emul_clock_settime(clockid_t, timespec_t *);

typedef struct lx_clock_backend {
	clockid_t lclk_ntv_id;
	int (*lclk_clock_getres)(clockid_t, timespec_t *);
	int (*lclk_clock_gettime)(clockid_t, timespec_t *);
	int (*lclk_clock_settime)(clockid_t, timespec_t *);
} lx_clock_backend_t;

/*
 * Use the native clock_* system call implementation, but with a translated
 * clock identifier:
 */
#define	NATIVE(ntv_id)							\
	{ ntv_id, clock_getres, clock_gettime, clock_settime }

/*
 * This backend is not supported, so we provide an emulation handler:
 */
#define	EMUL(ntv_id)							\
	{ ntv_id, lx_emul_clock_getres, lx_emul_clock_gettime,		\
	    lx_emul_clock_settime }

static lx_clock_backend_t lx_clock_backends[] = {
	NATIVE(CLOCK_REALTIME),		/* LX_CLOCK_REALTIME */
	NATIVE(CLOCK_HIGHRES),		/* LX_CLOCK_MONOTONIC */
	EMUL(CLOCK_PROCESS_CPUTIME_ID),	/* LX_CLOCK_PROCESS_CPUTIME_ID */
	EMUL(CLOCK_THREAD_CPUTIME_ID),	/* LX_CLOCK_THREAD_CPUTIME_ID */
	NATIVE(CLOCK_HIGHRES),		/* LX_CLOCK_MONOTONIC_RAW */
	NATIVE(CLOCK_REALTIME),		/* LX_CLOCK_REALTIME_COARSE */
	NATIVE(CLOCK_HIGHRES)		/* LX_CLOCK_MONOTONIC_COARSE */
};

#define	LX_CLOCK_MAX \
	(sizeof (lx_clock_backends) / sizeof (lx_clock_backends[0]))
#define	LX_CLOCK_BACKEND(clk) \
	((clk) < LX_CLOCK_MAX && (clk) >= 0 ? &lx_clock_backends[(clk)] : NULL)

static int
lx_emul_clock_settime(clockid_t clock, timespec_t *tp)
{
	return (set_errno(EINVAL));
}

static int
lx_emul_clock_gettime(clockid_t clock, timespec_t *tp)
{
	timespec_t t;

	switch (clock) {
	case CLOCK_PROCESS_CPUTIME_ID: {
		proc_t *p = ttoproc(curthread);
		hrtime_t snsecs, unsecs;

		/*
		 * Based on getrusage() in "rusagesys.c":
		 */
		mutex_enter(&p->p_lock);
		unsecs = mstate_aggr_state(p, LMS_USER);
		snsecs = mstate_aggr_state(p, LMS_SYSTEM);
		mutex_exit(&p->p_lock);

		hrt2ts(unsecs + snsecs, &t);
		break;
	}

	case CLOCK_THREAD_CPUTIME_ID: {
		klwp_t *lwp = ttolwp(curthread);
		struct mstate *ms = &lwp->lwp_mstate;
		hrtime_t snsecs, unsecs;

		/*
		 * Based on getrusage_lwp() in "rusagesys.c":
		 */
		unsecs = ms->ms_acct[LMS_USER];
		snsecs = ms->ms_acct[LMS_SYSTEM] + ms->ms_acct[LMS_TRAP];

		scalehrtime(&unsecs);
		scalehrtime(&snsecs);

		hrt2ts(unsecs + snsecs, &t);
		break;
	}

	default:
		return (set_errno(EINVAL));
	}

#if defined(_SYSCALL32_IMPL)
	if (get_udatamodel() != DATAMODEL_NATIVE) {
		timespec32_t t32;

		if (TIMESPEC_OVERFLOW(&t)) {
			return (set_errno(EOVERFLOW));
		}
		TIMESPEC_TO_TIMESPEC32(&t32, &t);

		if (copyout(&t32, tp, sizeof (t32)) != 0) {
			return (set_errno(EFAULT));
		}

		return (0);
	}
#endif

	if (copyout(&t, tp, sizeof (t)) != 0) {
		return (set_errno(EFAULT));
	}

	return (0);
}

static int
lx_emul_clock_getres(clockid_t clock, timespec_t *tp)
{
	timespec_t t;

	if (tp == NULL) {
		return (0);
	}

	switch (clock) {
	case CLOCK_PROCESS_CPUTIME_ID:
	case CLOCK_THREAD_CPUTIME_ID:
		/*
		 * These clock backends return microstate accounting values for
		 * the LWP or the entire process.  The Linux kernel claims they
		 * have nanosecond resolution; so will we.
		 */
		t.tv_sec = 0;
		t.tv_nsec = 1;
		break;

	default:
		return (set_errno(EINVAL));
	}

#if defined(_SYSCALL32_IMPL)
	if (get_udatamodel() != DATAMODEL_NATIVE) {
		timespec32_t t32;

		if (TIMESPEC_OVERFLOW(&t)) {
			return (set_errno(EOVERFLOW));
		}
		TIMESPEC_TO_TIMESPEC32(&t32, &t);

		if (copyout(&t32, tp, sizeof (t32)) != 0) {
			return (set_errno(EFAULT));
		}

		return (0);
	}
#endif

	if (copyout(&t, tp, sizeof (t)) != 0) {
		return (set_errno(EFAULT));
	}

	return (0);
}

static void
lx_clock_unsupported(int clock)
{
	char buf[100];

	(void) snprintf(buf, sizeof (buf), "unsupported clock: %d", clock);
	lx_unsupported(buf);
}

long
lx_clock_settime(int clock, timespec_t *tp)
{
	lx_clock_backend_t *backend;

	if ((backend = LX_CLOCK_BACKEND(clock)) == NULL) {
		lx_clock_unsupported(clock);
		return (set_errno(EINVAL));
	}

	return (backend->lclk_clock_settime(backend->lclk_ntv_id, tp));
}

long
lx_clock_gettime(int clock, timespec_t *tp)
{
	lx_clock_backend_t *backend;

	if ((backend = LX_CLOCK_BACKEND(clock)) == NULL) {
		lx_clock_unsupported(clock);
		return (set_errno(EINVAL));
	}

	return (backend->lclk_clock_gettime(backend->lclk_ntv_id, tp));
}

long
lx_clock_getres(int clock, timespec_t *tp)
{
	lx_clock_backend_t *backend;

	if (tp == NULL) {
		return (0);
	}

	if ((backend = LX_CLOCK_BACKEND(clock)) == NULL) {
		lx_clock_unsupported(clock);
		return (set_errno(EINVAL));
	}

	return (backend->lclk_clock_getres(backend->lclk_ntv_id, tp));
}
