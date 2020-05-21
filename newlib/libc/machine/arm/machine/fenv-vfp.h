#ifndef __thumb__
#define	vmrs_fpscr(__r)	__asm __volatile("vmrs %0, fpscr" : "=&r"(__r))
#define	vmsr_fpscr(__r)	__asm __volatile("vmsr fpscr, %0" : : "r"(__r))
#endif 

#define _FPU_MASK_SHIFT	8

__fenv_static inline int feclearexcept(int excepts)
{
	fexcept_t __fpsr;

	vmrs_fpscr(__fpsr);
	__fpsr &= ~excepts;
	vmsr_fpscr(__fpsr);
	return (0);
}

__fenv_static inline int
fegetexceptflag(fexcept_t *flagp, int excepts)
{
	fexcept_t __fpsr;

	vmrs_fpscr(__fpsr);
	*flagp = __fpsr & excepts;
	return (0);
}

__fenv_static inline int
fesetexceptflag(const fexcept_t *flagp, int excepts)
{
	fexcept_t __fpsr;

	vmrs_fpscr(__fpsr);
	__fpsr &= ~excepts;
	__fpsr |= *flagp & excepts;
	vmsr_fpscr(__fpsr);
	return (0);
}

__fenv_static inline int
feraiseexcept(int excepts)
{
	fexcept_t __ex = excepts;

	fesetexceptflag(&__ex, excepts);	/* XXX */
	return (0);
}

__fenv_static inline int
fetestexcept(int excepts)
{
	fexcept_t __fpsr;

	vmrs_fpscr(__fpsr);
	return (__fpsr & excepts);
}

__fenv_static inline int
fegetround(void)
{
	fenv_t __fpsr;

	vmrs_fpscr(__fpsr);
	return (__fpsr & _ROUND_MASK);
}

__fenv_static inline int
fesetround(int round)
{
	fenv_t __fpsr;

	vmrs_fpscr(__fpsr);
	__fpsr &= ~(_ROUND_MASK);
	__fpsr |= round;
	vmsr_fpscr(__fpsr);
	return (0);
}

__fenv_static inline int
fegetenv(fenv_t *envp)
{

	vmrs_fpscr(*envp);
	return (0);
}

__fenv_static inline int
feholdexcept(fenv_t *envp)
{
	fenv_t __env;

	vmrs_fpscr(__env);
	*envp = __env;
	__env &= ~(FE_ALL_EXCEPT);
	vmsr_fpscr(__env);
	return (0);
}

__fenv_static inline int
fesetenv(const fenv_t *envp)
{

	vmsr_fpscr(*envp);
	return (0);
}

__fenv_static inline int
feupdateenv(const fenv_t *envp)
{
	fexcept_t __fpsr;

	vmrs_fpscr(__fpsr);
	vmsr_fpscr(*envp);
	feraiseexcept(__fpsr & FE_ALL_EXCEPT);
	return (0);
}

#if __BSD_VISIBLE

/* We currently provide no external definitions of the functions below. */

__fenv_static inline int
feenableexcept(int __mask)
{
	fenv_t __old_fpsr, __new_fpsr;

	vmrs_fpscr(__old_fpsr);
	__new_fpsr = __old_fpsr |
	    ((__mask & FE_ALL_EXCEPT) << _FPU_MASK_SHIFT);
	vmsr_fpscr(__new_fpsr);
	return ((__old_fpsr >> _FPU_MASK_SHIFT) & FE_ALL_EXCEPT);
}

__fenv_static inline int
fedisableexcept(int __mask)
{
	fenv_t __old_fpsr, __new_fpsr;

	vmrs_fpscr(__old_fpsr);
	__new_fpsr = __old_fpsr &
	    ~((__mask & FE_ALL_EXCEPT) << _FPU_MASK_SHIFT);
	vmsr_fpscr(__new_fpsr);
	return ((__old_fpsr >> _FPU_MASK_SHIFT) & FE_ALL_EXCEPT);
}

__fenv_static inline int
fegetexcept(void)
{
	fenv_t __fpsr;

	vmrs_fpscr(__fpsr);
	return (__fpsr & FE_ALL_EXCEPT);
}

#endif /* __BSD_VISIBLE */

