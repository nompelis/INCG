#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

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

//
// Function to project a 3D vector on a plane given the plane equation
//
__inline void incg_Vec_PlaneProject( const double p[4], double x[3] )
{
   double t;

   t = x[0]*p[0] + x[1]*p[1] + x[2]*p[2];

   x[0] -= t*p[0];
   x[1] -= t*p[1];
   x[2] -= t*p[2];
}

//
// Function to project a point in 3D space on a plane given the plane equation
//
__inline void incg_Vec_PlaneProjectPoint( const double p[4], double x[3] )
{
   double t;

   t = x[0]*p[0] + x[1]*p[1] + x[2]*p[2];

   t = -(p[3] + t);
   x[0] += t*p[0];
   x[1] += t*p[1];
   x[2] += t*p[2];
}

//
// Function to return the equation for a plane given 3 3D points
// The plane equation is: "\vec{x} \dot \vec{n} + \alpha = 0"
//
__inline void incg_Vec_PlaneEquation(
   const double p1[3],
   const double p2[3],
   const double p3[3],
   double pl[4] )
{
   double dx1[3],dx2[3];

   dx1[0] = p2[0] - p1[0];
   dx1[1] = p2[1] - p1[1];
   dx1[2] = p2[2] - p1[2];

   dx2[0] = p3[0] - p1[0];
   dx2[1] = p3[1] - p1[1];
   dx2[2] = p3[2] - p1[2];

   incg_Vec_CrossProduct( dx1, dx2, pl );
   incg_Vec_Normalize3( pl );
   pl[3] = - incg_Vec_DotProduct( p1, pl );
}

#ifdef __cplusplus
}
#endif

