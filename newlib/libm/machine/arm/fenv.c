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

#include <fenv.h>

#include <machine/acle-compat.h>

#if __ARM_ARCH >= 6
#define FENV_ARMv6
#endif

/* When SOFTFP_ABI is defined we are using the softfp ABI. */
#if defined(__VFP_FP__) && !defined(__ARM_PCS_VFP)
#define SOFTFP_ABI
#endif

fenv_t __fe_dfl_env = { 0 };

const fenv_t *_fe_dfl_env = &__fe_dfl_env;

#ifndef SOFTFP_ABI
#define	vmrs_fpscr(__r)	__asm __volatile("vmrs %0, fpscr" : "=&r"(__r))
#define	vmsr_fpscr(__r)	__asm __volatile("vmsr fpscr, %0" : : "r"(__r))
#endif

#define _FPU_MASK_SHIFT	8

int feclearexcept(int excepts)
{

#ifndef SOFTFP_ABI
	fexcept_t __fpsr;
	vmrs_fpscr(__fpsr);
	__fpsr &= ~excepts;
	vmsr_fpscr(__fpsr);
#endif

	return (0);
}

int fegetexceptflag(fexcept_t *flagp, int excepts)
{

#ifndef SOFTFP_ABI
	fexcept_t __fpsr;
	vmrs_fpscr(__fpsr);
	__fpsr &= ~excepts;
	__fpsr |= *flagp & excepts;
	vmsr_fpscr(__fpsr);
#endif

	return (0);
}

int fesetexceptflag(const fexcept_t *flagp, int excepts)
{

#ifndef SOFTFP_ABI
	fexcept_t __fpsr;
	vmrs_fpscr(__fpsr);
	__fpsr &= ~excepts;
	__fpsr |= *flagp & excepts;
	vmsr_fpscr(__fpsr);
#endif

	return (0);
}

int feraiseexcept(int excepts)
{

#ifndef SOFTFP_ABI
	fexcept_t __ex = excepts;
	fesetexceptflag(&__ex, excepts);
#endif

	return (0);
}

int fetestexcept(int excepts)
{
#ifndef SOFTFP_ABI
	fexcept_t __fpsr;
	vmrs_fpscr(__fpsr);
	return (__fpsr & excepts);
#endif

	return (0);
}

int fegetround(void)
{

#ifndef SOFTFP_ABI
	fenv_t __fpsr;
	vmrs_fpscr(__fpsr);
	return (__fpsr & _ROUND_MASK);
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
	fenv_t __fpsr;
	vmrs_fpscr(__fpsr);
	__fpsr &= ~(_ROUND_MASK);
	__fpsr |= round;
	vmsr_fpscr(__fpsr);
	return (0);
#endif

	return (0);
}

int fegetenv(fenv_t *envp)
{
	
#ifndef SOFTFP_ABI
		vmrs_fpscr(*envp);
		return 0;
#endif

	*envp |= 0;

	return (0);
}

int feholdexcept(fenv_t *envp)
{

#ifndef SOFTFP_ABI
	fenv_t __env;
	vmrs_fpscr(__env);
	*envp = __env;
	__env &= ~(FE_ALL_EXCEPT);
	vmsr_fpscr(__env);
	return (0);

#endif
	*envp |= 0;

	return (0);
}

int fesetenv(const fenv_t *envp)
{

#ifndef SOFTFP_ABI
		vmsr_fpscr(*envp);
#endif
	return (0);
}

int feupdateenv(const fenv_t *envp)
{

#ifndef SOFTFP_ABI
	fexcept_t __fpsr;
	vmrs_fpscr(__fpsr);
	vmsr_fpscr(*envp);
	feraiseexcept(__fpsr & FE_ALL_EXCEPT);

	return 0;
#else

#if defined FE_NOMASK_ENV && FE_ALL_EXCEPT != 0

  if (envp == FE_NOMASK_ENV)
      return 1;

#endif

  return 0;

#endif

}

int feenableexcept(int __mask)
{
	
#ifndef SOFTFP_ABI
	fenv_t __old_fpsr, __new_fpsr;
	vmrs_fpscr(__old_fpsr);
	__new_fpsr = __old_fpsr |
	    ((__mask & FE_ALL_EXCEPT) << _FPU_MASK_SHIFT);
	vmsr_fpscr(__new_fpsr);
	return ((__old_fpsr >> _FPU_MASK_SHIFT) & FE_ALL_EXCEPT);
#endif

	return (0);
}

int fedisableexcept(int __mask)
{

#ifndef SOFTFP_ABI
	fenv_t __old_fpsr, __new_fpsr;

	vmrs_fpscr(__old_fpsr);
	__new_fpsr = __old_fpsr &
	    ~((__mask & FE_ALL_EXCEPT) << _FPU_MASK_SHIFT);
	vmsr_fpscr(__new_fpsr);
	return ((__old_fpsr >> _FPU_MASK_SHIFT) & FE_ALL_EXCEPT);
#endif

	return (0);
}

int fegetexcept(void)
{

#ifndef SOFTFP_ABI
	fenv_t __fpsr;

	vmrs_fpscr(__fpsr);
	return (__fpsr & FE_ALL_EXCEPT);
#endif

	return (0);
}


