
#ifndef _INCG_UTILS_H_
#define _INCG_UTILS_H_

__inline void incg_Vec_CrossProduct(
   const double x[3],
   const double y[3],
   double z[3] );

__inline double incg_Vec_DotProduct(
   const double x[3],
   const double y[3] );

__inline void incg_Vec_Normalize3( double x[2] );

__inline void incg_Vec_Normalize2( double x[2] );

__inline void incg_Vec_PlaneProject( const double p[4], double x[3] );

__inline void incg_Vec_PlaneProjectPoint( const double p[4], double x[3] );

__inline void incg_Vec_PlaneEquation(
   const double p1[3],
   const double p2[3],
   const double p3[3],
   double pl[4] );

#endif

