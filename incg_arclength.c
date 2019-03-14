#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Function that evaluates the integral of the arc-length of "sqrt(x*a)".
 * The arc-length is (in Mathematica notation):
 *  "Integrate[Sqrt[ 1 + D[Sqrt[x*a], x]^2 ], x]"
 *       l = 0.5 * x * sqrt(a/x + 4.0) +
 *           0.125 * a * log( 4.0*x*( sqrt(a/x + 4.0) + 2.0 ) + a );
 */

__inline double incg_Arclength_Sqrtax( double x, double a )
{
   double l,l0;
   const double eps = 1.0e-10;

   if( x <= 0.0 ) return(0.0);

   // upper limit is calculated
   l = 0.5 * x * sqrt(a/x + 4.0) +
       0.125 * a * log( 4.0*x*( sqrt(a/x + 4.0) + 2.0 ) + a );

   // lower limit does not behave well
   x = eps;
   l0= 0.5 * x * sqrt(a/x + 4.0) +
       0.125 * a * log( 4.0*x*( sqrt(a/x + 4.0) + 2.0 ) + a );

   return( l - l0 );
}

/*
 * Function that evaluates the integral of the arc-length of "b*sqrt(x*a)".
 * The arc-length is (in Mathematica notation):
 *  "Integrate[Sqrt[ 1 + D[b*Sqrt[a*x], x]^2 ], x]"
 *     l = 0.5 * x * sqrt(a*b*b/x + 4.0) +
 *         0.125 * a*b*b * log( 4.0 * x * sqrt(a*b*b/x + 4.0) + a*b*b + 8.0*x );
 */

__inline double incg_Arclength_bSqrtax( double x, double a, double b )
{
   double l,t1,t2,l0;
   const double eps = 1.0e-10;

   if( x <= 0.0 ) return(0.0);

   // upper limit is calculated
   t1 = a*b*b;
   t2 = x*sqrt( t1/x + 4.0 );
   l = 0.5*t2 + 0.125*t1*log( 4.0*t2 + t1 + 8.0*x );

   // lower limit does not behave well
   x = eps;
   t2 = x*sqrt( t1/x + 4.0 );
   l0= 0.5*t2 + 0.125*t1*log( 4.0*t2 + t1 + 8.0*x );

   return( l - l0 );
}

#ifdef __cplusplus
}
#endif
