#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>


//
// Function to calculate the cross-product of 3D vectors
//
__inline void incg_Vec_CrossProduct(
   const double x[3],
   const double y[3],
   double z[3] )
{
   z[0] = + x[1]*y[2] - y[1]*x[2];
   z[1] = - x[0]*y[2] + y[0]*x[2];
   z[2] = + x[0]*y[1] - y[0]*x[1];
}


//
// Function to calculate the dot-product of 3D vectors
//
__inline double incg_Vec_DotProduct(
   const double x[3],
   const double y[3] )
{
   return( x[0]*y[0] + x[1]*y[1] + x[2]*y[2] );
}


//
// Function to normalize a 3D vector
//
__inline void incg_Vec_Normalize3( double x[3] )
{
   double t;

   t = x[0]*x[0] + x[1]*x[1] + x[2]*x[2];
   t = 1.0/sqrt(t);

   x[0] = x[0]*t;
   x[1] = x[1]*t;
   x[2] = x[2]*t;
}

//
// Function to normalize a 2D vector
//
__inline void incg_Vec_Normalize2( double x[2] )
{
   double t;

   t = x[0]*x[0] + x[1]*x[1];
   t = 1.0/sqrt(t);

   x[0] = x[0]*t;
   x[1] = x[1]*t;
}

