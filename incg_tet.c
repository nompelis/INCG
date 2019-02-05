#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "incg_tet.h"
#include "incg_utils.h"


//
// Function to test whether a point falls within a tetrahedron
// Expects the tetrahedron to be given with its nodes ordered in a conventional
// 1,2,3 counter-clockwise base and 4 being the opposite vertex.
//
int incg_Tet_PointInside(
   const double x1[3],
   const double x2[3],
   const double x3[3],
   const double x4[3],
   const double xp[3] )
{
   double dx1[3], dx2[3], da[3], dy[3];
   int k;
   double ret;


   // test of normal of face 1-2-3
   for(k=0;k<3;++k) dx1[k] = x2[k] - x1[k];
   for(k=0;k<3;++k) dx2[k] = x3[k] - x1[k];
   incg_Vec_CrossProduct( dx1, dx2, da );
   for(k=0;k<3;++k) dy[k] = xp[k] - x1[k];
   ret = incg_Vec_DotProduct( da, dy );
//printf("Face 1: %lf \n", ret );
   if( ret < 0.0 ) return(0);

   // test of normal of face 1-2-4
   for(k=0;k<3;++k) dx2[k] = x4[k] - x1[k];
   incg_Vec_CrossProduct( dx2, dx1, da );
   ret = incg_Vec_DotProduct( da, dy );
//printf("Face 1: %lf \n", ret );
   if( ret < 0.0 ) return(0);

   // test of normal of face 1-3-4
   for(k=0;k<3;++k) dx1[k] = x3[k] - x1[k];
   incg_Vec_CrossProduct( dx1, dx2, da );
   ret = incg_Vec_DotProduct( da, dy );
//printf("Face 1: %lf \n", ret );
   if( ret < 0.0 ) return(0);

   // test of normal of face 2-3-4
   for(k=0;k<3;++k) dx1[k] = x4[k] - x2[k];
   for(k=0;k<3;++k) dx2[k] = x3[k] - x2[k];
   incg_Vec_CrossProduct( dx1, dx2, da );
   for(k=0;k<3;++k) dy[k] = xp[k] - x2[k];
   ret = incg_Vec_DotProduct( da, dy );
//printf("Face 1: %lf \n", ret );
   if( ret < 0.0 ) return(0);

   return(1);
}


//
// Function to calculate the volume of the tetrahedron
// Expects the tetrahedron to be given with its nodes ordered in a conventional
// 1,2,3 counter-clockwise base and 4 being the opposite vertex.
//
double incg_Tet_CalcVolume(
   const double x1[3],
   const double x2[3],
   const double x3[3],
   const double x4[3] )
{
   double vol;
   double dx1[3],dx2[3],dx3[3];


   dx1[0] = x2[0] - x1[0];
   dx1[1] = x2[1] - x1[1];
   dx1[2] = x2[2] - x1[2];

   dx2[0] = x3[0] - x1[0];
   dx2[1] = x3[1] - x1[1];
   dx2[2] = x3[2] - x1[2];

   dx3[0] = x4[0] - x1[0];
   dx3[1] = x4[1] - x1[1];
   dx3[2] = x4[2] - x1[2];

   vol = ( dx1[1]*dx2[2] - dx2[1]*dx1[2])*dx3[0] +
         (-dx1[0]*dx2[2] + dx2[0]*dx1[2])*dx3[1] +
         ( dx1[0]*dx2[1] - dx2[0]*dx1[1])*dx3[2];
   vol = vol / 6.0;

   return( vol );
}

#ifdef __cplusplus
}
#endif
