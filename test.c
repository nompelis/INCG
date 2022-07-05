#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "incg_utils.h"
#include "incg_tet.h"
#include "incg_tri.h"
#include "incg_mesh.h"

//
// a function to generate a random point inside a triangle
//
void test_point_random_in_triangle()
{
   double x1[3] = { 1.0, 0.0, 0.0 };
   double x2[3] = { 1.0, 1.0, 0.0 };
   double x3[3] = { 1.0, 1.0, 1.0 };
   double xp[3];

   incg_Tri_MakeRandomPoint( x1, x2, x3, xp );
   printf("Point in triangle: %lf %lf %lf (z-y<0 ? %lf) \n",
           xp[0], xp[1], xp[2], xp[2]-xp[1] );
   incg_Tri_MakeRandomPoint( x1, x2, x3, xp );
   printf("Point in triangle: %lf %lf %lf (z-y<0 ? %lf) \n",
           xp[0], xp[1], xp[2], xp[2]-xp[1] );
   incg_Tri_MakeRandomPoint( x1, x2, x3, xp );
   printf("Point in triangle: %lf %lf %lf (z-y<0 ? %lf) \n",
           xp[0], xp[1], xp[2], xp[2]-xp[1] );
}

//
// a function to test whether a point falls within a triangle
//
void test_point_point_in_triangle()
{
   double x1[3] = { 1.0, 0.0, 0.0 };
   double x2[3] = { 1.0, 1.0, 0.0 };
   double x3[3] = { 1.0, 0.5, 1.0 };
   double xp[3];
   int iret;


   xp[0] = 1.0;
   xp[1] = 1.5;
   xp[2] = 0.5;
   iret = incg_Tri_PointInside( x1, x2, x3, xp );
   printf("Testing point in triangle: %d \n", iret );
}

//
// a function to write the fundamental Cartesian basis in a (tecplot) file
//
void test_write_basis( FILE *fp )
{
   double x1[3] = { 0.0, 0.0, 0.0 };
   double x2[3] = { 1.0, 0.0, 0.0 };
   double x3[3] = { 0.0, 1.0, 0.0 };
   double x4[3] = { 0.0, 0.0, 1.0 };

   fprintf(fp,"variables = x y z u v w \n");
   fprintf(fp,"zone T=\"plane as quad\", N=4,E=3,F=FEPOINT,ET=TRIANGLE\n");
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", x1[0],x1[1],x1[2] );
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", x2[0],x2[1],x2[2] );
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", x3[0],x3[1],x3[2] );
   fprintf(fp,"%lf %lf %lf 0 0 0 \n", x4[0],x4[1],x4[2] );
   fprintf(fp," 1 2 1 \n");
   fprintf(fp," 1 3 1 \n");
   fprintf(fp," 1 4 1 \n");
}

//
// a function to perform a point projection on a plane
//
void test_point_projection_on_plane()
{
   FILE *fp;
   double x1[3] = { 1.9, 0.0, 0.0 };
   double x2[3] = { 1.9, 1.0, 0.0 };
   double x3[3] = { 1.0, 1.0, 1.0 };
   double x4[3] = { 1.0, 0.0, 1.0 };
   double xp[3];
   double pl[4];


   printf("Projecting a point on a plane\n");

   fp = fopen("data.dat","w");
   test_write_basis( fp );

   fprintf(fp,"variables = x y z u v w \n");
   fprintf(fp,"zone T=\"plane as quad\", N=4,E=1,F=FEPOINT,ET=QUADRILATERAL\n");
   fprintf(fp,"%lf %lf %lf  0 0 0\n", x1[0],x1[1],x1[2] );
   fprintf(fp,"%lf %lf %lf  0 0 0\n", x2[0],x2[1],x2[2] );
   fprintf(fp,"%lf %lf %lf  0 0 0\n", x3[0],x3[1],x3[2] );
   fprintf(fp,"%lf %lf %lf  0 0 0\n", x4[0],x4[1],x4[2] );
   fprintf(fp,"1 2 3 4 \n");

   xp[0] = 0.25;
   xp[1] = 0.7;
   xp[2] = 0.5;
   fprintf(fp,"zone T=\"arbitrary point \", N=1,E=1,F=FEPOINT,ET=TRIANGLE\n");
   fprintf(fp,"%lf %lf %lf  0 0 0\n", xp[0],xp[1],xp[2] );
   fprintf(fp,"1 1 1 \n");

   // equation of the plane
   incg_Vec_PlaneEquation( x1, x2, x3, pl );

   incg_Vec_PlaneProjectPoint( pl, xp );
   fprintf(fp,"zone T=\"projected point\", N=1,E=1,F=FEPOINT,ET=TRIANGLE\n");
   fprintf(fp,"%lf %lf %lf  0 0 0\n", xp[0],xp[1],xp[2] );
   fprintf(fp,"1 1 1 \n");

   fclose(fp);
}

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

   mesh_t mesh;

   printf("--------\n");
   printf("Volume of tetrahedron: %lf \n", incg_Tet_CalcVolume( x1,x2,x3,x4 ) );

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
   incg_Vec_PlaneProject( pl, pr );
   printf("Projection on plane: %lf %lf %lf \n", pr[0],pr[1],pr[2] );
   printf("--------\n");

   // test point projection on a plane
   (void) test_point_projection_on_plane();
   printf("--------\n");

   // test whether point falls within a triangle
   test_point_point_in_triangle();
   printf("--------\n");

   // test for creatng a random point inside a triangle
   test_point_random_in_triangle();
   printf("--------\n");

   // test creating a cube mesh and refining it uniformly
   printf("Testing creating and uniformly refining a mesh \n");
   (void) incg_MakeMesh_Cube( &mesh );
   (void) incg_RefineMesh_Uniform( &mesh );
   printf("--------\n");

   return(0);
}

