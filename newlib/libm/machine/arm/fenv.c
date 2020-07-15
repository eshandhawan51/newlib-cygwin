/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2004 David Schultz <das@FreeBSD.ORG>
 * Copyright (c) 2013 Andrew Turner <andrew@FreeBSD.ORG>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#define	__fenv_static
#include <fenv.h>

#include <machine/acle-compat.h>

#if __ARM_ARCH >= 6
#define FENV_ARMv6
#endif

/* When SOFTFP_ABI is defined we are using the softfp ABI. */
#if defined(__VFP_FP__) && !defined(__ARM_PCS_VFP)
#define SOFTFP_ABI
#endif

#ifndef FENV_MANGLE
/*
 * Hopefully the system ID byte is immutable, so it's valid to use
 * this as a default environment.
 */
fenv_t __fe_dfl_env = { 0 };

const fenv_t *_fe_dfl_env = &__fe_dfl_env;
#endif


/* If this is a non-mangled softfp version special processing is required */
#if defined(FENV_MANGLE) || !defined(SOFTFP_ABI) || !defined(FENV_ARMv6)
/*
 * The following macros map between the softfloat emulator's flags and
 * the hardware's FPSR.  The hardware this file was written for doesn't
 * have rounding control bits, so we stick those in the system ID byte.
 */
#ifndef SOFTFP_ABI
#include <machine/fenv-vfp.h>
#endif

#ifdef __GNUC_GNU_INLINE__
#error "This file must be compiled with C99 'inline' semantics"
#endif

extern inline int feclearexcept(int excepts);
extern inline int fegetexceptflag(fexcept_t *flagp, int excepts);
extern inline int fesetexceptflag(const fexcept_t *flagp, int excepts);
extern inline int feraiseexcept(int excepts);
extern inline int fetestexcept(int excepts);
extern inline int fegetround(void);
extern inline int fesetround(int round);
extern inline int fegetenv(fenv_t *envp);
extern inline int feholdexcept(fenv_t *envp);
extern inline int fesetenv(const fenv_t *envp);
extern inline int feupdateenv(const fenv_t *envp);
extern inline int feenableexcept(int __mask);
extern inline int fedisableexcept(int __mask);
extern inline int fegetexcept(void);

#else /* !FENV_MANGLE && SOFTFP_ABI */
/* Set by libc when the VFP unit is enabled */

#ifndef SOFTFP_ABI
int __vfp_feclearexcept(int excepts);
int __vfp_fegetexceptflag(fexcept_t *flagp, int excepts);
int __vfp_fesetexceptflag(const fexcept_t *flagp, int excepts);
int __vfp_feraiseexcept(int excepts);
int __vfp_fetestexcept(int excepts);
int __vfp_fegetround(void);
int __vfp_fesetround(int round);
int __vfp_fegetenv(fenv_t *envp);
int __vfp_feholdexcept(fenv_t *envp);
int __vfp_fesetenv(const fenv_t *envp);
int __vfp_feupdateenv(const fenv_t *envp);
int __vfp_feenableexcept(int __mask);
int __vfp_fedisableexcept(int __mask);
int __vfp_fegetexcept(void);
#endif

static int
__softfp_round_to_vfp(int round)
{

	switch (round) {
	case FE_TONEAREST:
	default:
		return VFP_FE_TONEAREST;
	case FE_TOWARDZERO:
		return VFP_FE_TOWARDZERO;
	case FE_UPWARD:
		return VFP_FE_UPWARD;
	case FE_DOWNWARD:
		return VFP_FE_DOWNWARD;
	}
}

static int
__softfp_round_from_vfp(int round)
{

	switch (round) {
	case VFP_FE_TONEAREST:
	default:
		return FE_TONEAREST;
	case VFP_FE_TOWARDZERO:
		return FE_TOWARDZERO;
	case VFP_FE_UPWARD:
		return FE_UPWARD;
	case VFP_FE_DOWNWARD:
		return FE_DOWNWARD;
	}
}

int feclearexcept(int excepts)
{

#ifndef SOFTFP_ABI
		__vfp_feclearexcept(excepts);
#endif
	

	return (0);
}

int fegetexceptflag(fexcept_t *flagp, int excepts)
{
	fexcept_t __vfp_flagp;

	__vfp_flagp = 0;
#ifndef SOFTFP_ABI
		__vfp_fegetexceptflag(&__vfp_flagp, excepts);
#endif
	
	*flagp |= __vfp_flagp;

	return (0);
}

int fesetexceptflag(const fexcept_t *flagp, int excepts)
{

#ifndef SOFTFP_ABI
		__vfp_fesetexceptflag(flagp, excepts);
#endif

	return (0);
}

int feraiseexcept(int excepts)
{

#ifndef SOFTFP_ABI
		__vfp_feraiseexcept(excepts);
#endif

	return (0);
}

int fetestexcept(int excepts)
{
	int __got_excepts;

	__got_excepts = 0;
#ifndef SOFTFP_ABI
		__got_excepts = __vfp_fetestexcept(excepts);
#endif

	return (__got_excepts);
}

int fegetround(void)
{

#ifndef SOFTFP_ABI
		return __softfp_round_from_vfp(__vfp_fegetround());
#endif

#ifdef SOFTFP_ABI
/* For soft float */

#ifdef FE_TONEAREST
		return FE_TONEAREST;
#else 
		return 0;
#endif

#endif
}

int fesetround(int round)
{

#ifndef SOFTFP_ABI
		__vfp_fesetround(__softfp_round_to_vfp(round));
#endif

	return (0);
}

int fegetenv(fenv_t *envp)
{
	fenv_t __vfp_envp;

	__vfp_envp = 0;
#ifndef SOFTFP_ABI
		__vfp_fegetenv(&__vfp_envp);
#endif

	*envp |= __vfp_envp;

	return (0);
}

int feholdexcept(fenv_t *envp)
{
	fenv_t __vfp_envp;

	__vfp_envp = 0;
#ifndef SOFTFP_ABI
		__vfp_feholdexcept(&__vfp_envp);
#endif
	*envp |= __vfp_envp;

	return (0);
}

int fesetenv(const fenv_t *envp)
{

#ifndef SOFTFP_ABI
		__vfp_fesetenv(envp);
#endif


	return (0);
}

int feupdateenv(const fenv_t *envp)
{

#ifndef SOFTFP_ABI
		__vfp_feupdateenv(envp);

		return 0;
#endif

#ifdef SOFTFP_ABI

#if defined FE_NOMASK_ENV && FE_ALL_EXCEPT != 0

  if (envp == FE_NOMASK_ENV)
      return 1;

#endif

  return 0;

#endif

}

int feenableexcept(int __mask)
{
	int __unmasked;

	__unmasked = 0;
#ifndef SOFTFP_ABI
		__unmasked = __vfp_feenableexcept(__mask);
#endif

	return (__unmasked);
}

int fedisableexcept(int __mask)
{
	int __unmasked;

	__unmasked = 0;
#ifndef SOFTFP_ABI
		__unmasked = __vfp_fedisableexcept(__mask);
#endif

	return (__unmasked);
}

int fegetexcept(void)
{
	int __unmasked;

	__unmasked = 0;
#ifndef SOFTFP_ABI
		__unmasked = __vfp_fegetexcept();
#endif

	return (__unmasked);
}

#endif

