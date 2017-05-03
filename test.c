#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "incg_utils.h"
#include "incg_tet.h"


int main(int argc, char **argv)
{
   int iret;

   double x1[3] = { 0.0, 0.0, 0.0 };
   double x2[3] = { 1.0, 0.0, 0.0 };
   double x3[3] = { 0.0, 1.0, 0.0 };
   double x4[3] = { 0.0, 0.0, 1.0 };
   double xo[3] = { 0.1, 0.1, 0.1 };
   double pr[3] = { 0.0, 0.0, 1.0 };
   double pl[4] = {-1.0,-1.0, 1.0, 0.0 };  // plane equation un-normalized

   printf("--------\n");

   iret = incg_Tet_PointInside( x1,x2,x3,x4, xo );
   if( iret == 1 ) printf("Point falls within \n");
   printf("--------\n");

   incg_Vec_Normalize3( xo );
   printf("Normalized xo: %lf %lf %lf \n", xo[0],xo[1],xo[2] );
   xo[0] = 1.0;
   xo[1] = 1.0;
   incg_Vec_Normalize2( xo );
   printf("Normalized xo: %lf %lf \n", xo[0],xo[1] );
   printf("--------\n");

   printf("Plane: %lf %lf %lf \n", pl[0],pl[1],pl[2] );
   incg_Vec_Normalize3( pl );
   printf("Plane (normalized vector): %lf %lf %lf \n", pl[0],pl[1],pl[2] );
   incg_Vec_PlaneProject( pr, pl );
   printf("Projection on plane: %lf %lf %lf \n", pr[0],pr[1],pr[2] );
   printf("--------\n");

   return(0);
}

