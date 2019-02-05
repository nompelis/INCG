
#ifndef _INCG_TET_H_
#define _INCG_TET_H_

int incg_Tet_PointInside(
   const double x1[3],
   const double x2[3],
   const double x3[3],
   const double x4[3],
   const double xp[3] );

double incg_Tet_CalcVolume(
   const double x1[3],
   const double x2[3],
   const double x3[3],
   const double x4[3] );

#endif

