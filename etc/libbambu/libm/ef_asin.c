/**
 * Porting of the libm library to the PandA framework
 * starting from the original FDLIBM 5.3 (Freely Distributable LIBM) developed by SUN
 * plus the newlib version 1.19 from RedHat and plus uClibc version 0.9.32.1 developed by Erik Andersen.
 * The author of this port is Fabrizio Ferrandi from Politecnico di Milano.
 * The porting fall under the LGPL v2.1, see the files COPYING.LIB and COPYING.LIBM_PANDA in this directory.
 * Date: September, 11 2013.
 */
/* ef_asin.c -- float version of e_asin.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#include "math_privatef.h"

static const float one = 1.0000000000e+00f, /* 0x3F800000 */
    huge = 1.000e+30f, pio2_hi = 1.57079637050628662109375f, pio2_lo = -4.37113900018624283e-8f,
                   pio4_hi = 0.785398185253143310546875f,
                   /* coefficient for R(x^2) */
    pS0 = 1.6666667163e-01f,  /* 0x3e2aaaab */
    pS1 = -3.2556581497e-01f, /* 0xbea6b090 */
    pS2 = 2.0121252537e-01f,  /* 0x3e4e0aa8 */
    pS3 = -4.0055535734e-02f, /* 0xbd241146 */
    pS4 = 7.9153501429e-04f,  /* 0x3a4f7f04 */
    pS5 = 3.4793309169e-05f,  /* 0x3811ef08 */
    qS1 = -2.4033949375e+00f, /* 0xc019d139 */
    qS2 = 2.0209457874e+00f,  /* 0x4001572d */
    qS3 = -6.8828397989e-01f, /* 0xbf303361 */
    qS4 = 7.7038154006e-02f;  /* 0x3d9dc62e */

float __hide_ieee754_asinf(float x)
{
   float t, w, p, q, c, r, s;
   int hx, ix;
   GET_FLOAT_WORD(hx, x);
   ix = hx & 0x7fffffff;
   if(ix == 0x3f800000)
   {
      /* asin(1)=+-pi/2 with inexact */
      return x * pio2_hi + x * pio2_lo;
   }
   else if(ix > 0x3f800000)
   {                             /* |x|>= 1 */
      return __builtin_nanf(""); /* asin(|x|>1) is NaN */
   }
   else if(ix < 0x3f000000)
   { /* |x|<0.5 */
      if(ix < 0x32000000)
      { /* if |x| < 2**-27 */
         if(huge + x > one)
            return x; /* return x with inexact if x!=0*/
      }
      else
      {
         t = x * x;
         p = t * (pS0 + t * (pS1 + t * (pS2 + t * (pS3 + t * (pS4 + t * pS5)))));
         q = one + t * (qS1 + t * (qS2 + t * (qS3 + t * qS4)));
         w = p / q;
         return x + x * w;
      }
   }
   /* 1> |x|>= 0.5 */
   w = one - fabsf(x);
   t = w * (float)0.5;
   p = t * (pS0 + t * (pS1 + t * (pS2 + t * (pS3 + t * (pS4 + t * pS5)))));
   q = one + t * (qS1 + t * (qS2 + t * (qS3 + t * qS4)));
   s = __hide_ieee754_sqrtf(t);
   if(ix >= 0x3F79999A)
   { /* if |x| > 0.975 */
      w = p / q;
      t = pio2_hi - ((float)2.0 * (s + s * w) - pio2_lo);
   }
   else
   {
      int iw;
      w = s;
      GET_FLOAT_WORD(iw, w);
      SET_FLOAT_WORD(w, iw & 0xfffff000);
      c = (t - w * w) / (s + w);
      r = p / q;
      p = (float)2.0 * s * r - (pio2_lo - (float)2.0 * c);
      q = pio4_hi - (float)2.0 * w;
      t = pio4_hi - (p - q);
   }
   if(hx > 0)
      return t;
   else
      return -t;
}
