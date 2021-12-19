/* _udivsi3 for Lattice Mico32.
   Contributed by Jon Beniston <jon@beniston.com>

   Copyright (C) 2009-2019 Free Software Foundation, Inc.

   This file is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any
   later version.

   This file is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>. */

// #include "libgcc_lm32.h"
#include <stdint.h>

extern uint32_t __udivmodsi4 (uint32_t num, uint32_t den, int modwanted);

/* Unsigned integer division.  */

uint32_t
__udivsi3 (uint32_t a, uint32_t b)
{
  return __udivmodsi4 (a, b, 0);
}
