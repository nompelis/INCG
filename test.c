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


   iret = incg_Tet_PointInside( x1,x2,x3,x4, xo );
   if( iret == 1 ) printf("Point falls within \n");

   incg_Vec_Normalize3( xo );
   printf("Normalized xo: %lf %lf %lf \n", xo[0],xo[1],xo[2] );
   xo[0] = 1.0;
   xo[1] = 1.0;
   incg_Vec_Normalize2( xo );
   printf("Normalized xo: %lf %lf \n", xo[0],xo[1] );

   return(0);
}

