#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "incg_tri.h"
#include "incg_utils.h"


//
// Function to test whether a point falls within a triangle
// Assumes that all points lie (approximately) on the same plane.
//
int incg_Tri_PointInside(
   const double p1[3],
   const double p2[3],
   const double p3[3],
   const double xp[3] )
{
   double dx1[3], dx2[3], proj;

#ifdef _DEBUG2_
   FILE *fp=fopen("data.dat","w");
   test_write_basis(fp);
   fprintf(fp,"variables = x y z u v w \n");
   fprintf(fp,"zone T=\"triangle\", N=3,E=1,F=FEPOINT,ET=TRIANGLE\n");
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", p1[0],p1[1],p1[2] );
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", p2[0],p2[1],p2[2] );
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", p3[0],p3[1],p3[2] );
   fprintf(fp,"1 2 3 \n");
   fprintf(fp,"variables = x y z u v w \n");
   fprintf(fp,"zone T=\"point\", N=1,E=1,F=FEPOINT,ET=TRIANGLE\n");
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", xp[0],xp[1],xp[2] );
   fprintf(fp,"1 1 1 \n");
#endif

   // side of edge 1
   dx1[0] = p2[0] - p1[0];
   dx1[1] = p2[1] - p1[1];
   dx1[2] = p2[2] - p1[2];

   dx2[0] = p1[0] - p3[0];
   dx2[1] = p1[1] - p3[1];
   dx2[2] = p1[2] - p3[2];

   incg_Vec_Normalize3( dx1 );
   proj = incg_Vec_DotProduct( dx1, dx2 );
//printf("Projection %lf \n", proj );//HACK
   dx2[0] = -(dx2[0] - proj*dx1[0]);
   dx2[1] = -(dx2[1] - proj*dx1[1]);
   dx2[2] = -(dx2[2] - proj*dx1[2]);
//printf("Vector perpend. to edge: %lf %lf %lf \n", dx2[0], dx2[1], dx2[2] );
#ifdef _DEBUG2_
   fprintf(fp,"variables = x y z u v w \n");
   fprintf(fp,"zone T=\"vector\", N=2,E=1,F=FEPOINT,ET=TRIANGLE\n");
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", 0.0, 0.0, 0.0 );
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", dx2[0],dx2[1],dx2[2] );
   fprintf(fp,"1 2 1 \n");
#endif

  proj = (xp[0] - p1[0])*dx2[0] +
         (xp[1] - p1[1])*dx2[1] +
         (xp[2] - p1[2])*dx2[2];
//printf("Projection two %lf \n", proj );//HACK
  if( proj < 0.0 ) return(0);

   // side of edge 2
   dx1[0] = p3[0] - p2[0];
   dx1[1] = p3[1] - p2[1];
   dx1[2] = p3[2] - p2[2];

   dx2[0] = p2[0] - p1[0];
   dx2[1] = p2[1] - p1[1];
   dx2[2] = p2[2] - p1[2];

   incg_Vec_Normalize3( dx1 );
   proj = incg_Vec_DotProduct( dx1, dx2 );
//printf("Projection %lf \n", proj );//HACK
   dx2[0] = -(dx2[0] - proj*dx1[0]);
   dx2[1] = -(dx2[1] - proj*dx1[1]);
   dx2[2] = -(dx2[2] - proj*dx1[2]);
//printf("Vector perpend. to edge: %lf %lf %lf \n", dx2[0], dx2[1], dx2[2] );
#ifdef _DEBUG2_
   fprintf(fp,"variables = x y z u v w \n");
   fprintf(fp,"zone T=\"vector\", N=2,E=1,F=FEPOINT,ET=TRIANGLE\n");
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", 0.0, 0.0, 0.0 );
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", dx2[0],dx2[1],dx2[2] );
   fprintf(fp,"1 2 1 \n");
#endif

   proj = (xp[0] - p2[0])*dx2[0] +
          (xp[1] - p2[1])*dx2[1] +
          (xp[2] - p2[2])*dx2[2];
//printf("Projection two %lf \n", proj );//HACK
   if( proj < 0.0 ) return(0);

   // side of edge 2
   dx1[0] = p1[0] - p3[0];
   dx1[1] = p1[1] - p3[1];
   dx1[2] = p1[2] - p3[2];

   dx2[0] = p3[0] - p2[0];
   dx2[1] = p3[1] - p2[1];
   dx2[2] = p3[2] - p2[2];

   incg_Vec_Normalize3( dx1 );
   proj = incg_Vec_DotProduct( dx1, dx2 );
//printf("Projection %lf \n", proj );//HACK
   dx2[0] = -(dx2[0] - proj*dx1[0]);
   dx2[1] = -(dx2[1] - proj*dx1[1]);
   dx2[2] = -(dx2[2] - proj*dx1[2]);
//printf("Vector perpend. to edge: %lf %lf %lf \n", dx2[0], dx2[1], dx2[2] );
#ifdef _DEBUG2_
   fprintf(fp,"variables = x y z u v w \n");
   fprintf(fp,"zone T=\"vector\", N=2,E=1,F=FEPOINT,ET=TRIANGLE\n");
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", 0.0, 0.0, 0.0 );
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", dx2[0],dx2[1],dx2[2] );
   fprintf(fp,"1 2 1 \n");
#endif

   proj = (xp[0] - p3[0])*dx2[0] +
          (xp[1] - p3[1])*dx2[1] +
          (xp[2] - p3[2])*dx2[2];
//printf("Projection two %lf \n", proj );//HACK
   if( proj < 0.0 ) return(0);

   return(1);
}

#ifdef __cplusplus
}
#endif

