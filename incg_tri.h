
#ifndef _INCG_TRI_H_
#define _INCG_TRI_H_

int incg_Tri_PointInside(
   const double p1[3],
   const double p2[3],
   const double p3[3],
   const double xp[3] );

void incg_Tri_MakeRandomPoint(
   const double p1[3],
   const double p2[3],
   const double p3[3],
   double xp[3] );

#endif

