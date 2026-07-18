/* ARM64 compatibility exports that have no implementation in newlib's
   generic AArch64 configuration. */

#include <stdint.h>

#define FE_ALL_EXCEPT 0x1f
#define FPCR_ENABLE_SHIFT 8

/* Kept for ABI compatibility with pre-locale Cygwin clients. */
const char _ctype_[384] = { 0 };

/* ARM64 has a single floating-point format and no x87-style precision mode. */
static const uint64_t arm64_fe_nomask_env = FE_ALL_EXCEPT << FPCR_ENABLE_SHIFT;
const uint64_t *_fe_nomask_env = &arm64_fe_nomask_env;

int
fedisableexcept (int mask)
{
  uint64_t old_fpcr;
  uint64_t new_fpcr;

  __asm__ volatile ("mrs %0, fpcr" : "=r" (old_fpcr));
  new_fpcr = old_fpcr & ~((uint64_t) (mask & FE_ALL_EXCEPT)
                         << FPCR_ENABLE_SHIFT);
  __asm__ volatile ("msr fpcr, %0" : : "r" (new_fpcr));
  return (old_fpcr >> FPCR_ENABLE_SHIFT) & FE_ALL_EXCEPT;
}

int
fegetexcept (void)
{
  uint64_t fpcr;

  __asm__ volatile ("mrs %0, fpcr" : "=r" (fpcr));
  return (fpcr >> FPCR_ENABLE_SHIFT) & FE_ALL_EXCEPT;
}

double
scalbl (double x, long exponent)
{
  return __builtin_scalbln (x, exponent);
}
