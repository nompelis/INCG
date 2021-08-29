#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "incg_mesh.h"

//
// Function that takes a pointer to a mesh object and fills its internals
// with the data forming a single triangle. (This function is meant mostly
// for testing.)
//

int incg_MakeMesh_OneTriangle( mesh_t* m )
{
   vertex_t *v;
   edge_t *e;
   triangle_t *t;
   int nt,ne,nv,i;


   if( m == NULL ) return 1;

   // allocate essential structures for a triangle
   nt = 1, nv = 3, ne = 3;
   v = (vertex_t *) malloc( ((size_t) nv) * sizeof( vertex_t ) );
   e = (edge_t *) malloc( ((size_t) ne) * sizeof( edge_t ) );
   t = (triangle_t*) malloc( ((size_t) nt) * sizeof( triangle_t ) );
   if( t == NULL || e == NULL || v == NULL ) {
      if( t != NULL ) free( t );
      if( e != NULL ) free( e );
      if( v != NULL ) free( v );
      return -1;
   }

   // make coefficients assignments for vertices
   v[0].x  = 0.0; v[0].y  = 0.0; v[0].z  = 0.0;  v[0].id = 0;
   v[1].x  = 1.0; v[1].y  = 0.0; v[1].z  = 0.0;  v[1].id = 1;
   v[2].x  = 0.0; v[2].y  = 1.0; v[2].z  = 0.0;  v[2].id = 2;

   // make vertex assignments for edges
   for(i=0;i<ne;++i) { e[i].tl = NULL;  e[i].tr = NULL; e[i].id = i; }
   e[0].va = &( v[0] ); e[0].vb = &( v[1] );
   e[1].va = &( v[1] ); e[1].vb = &( v[2] );
   e[2].va = &( v[2] ); e[2].vb = &( v[0] );

   // triangles
   for(i=0;i<nt;++i) t[i].id = i;
   i = 0;
   t[i].e1 = &( e[0] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[1] ); t[i].d2 = 0;
   t[i].e2->tl = &( t[i] );
   t[i].e3 = &( e[2] ); t[i].d3 = 0;
   t[i].e3->tl = &( t[i] );

   m->nv = nv;
   m->ne = ne;
   m->nt = nt;
   m->v = v;
   m->e = e;
   m->t = t;
#ifdef _DEBUG_
{  FILE *fp = fopen( "TRIANGLE.dat", "w");
   fprintf( fp, "TITLE = \"Single triangle\" \n");
   fprintf( fp, "VARIABLES = x y z \n");
   fprintf( fp, "ZONE NODES=%ld, ELEMENTS=%ld \n", m->nv, m->nt );
   fprintf( fp, "     ZONETYPE=FETRIANGLE, DATAPACKING=POINT \n");
   for(i=0;i<(int) m->nv;++i) {
      fprintf( fp, " %16.9e %16.9e %16.9e \n",
                   m->v[i].x, m->v[i].y, m->v[i].z );
   }
   for(i=0;i<(int) m->nt;++i) {
      if( t[i].d1 == 0 ) {
         fprintf( fp, " %ld", t[i].e1->va->id +1 );
      } else { fprintf( fp, " %ld", t[i].e1->vb->id +1 ); }
      if( t[i].d2 == 0 ) {
         fprintf( fp, " %ld", t[i].e2->va->id +1 );
      } else { fprintf( fp, " %ld", t[i].e2->vb->id +1 ); }
      if( t[i].d3 == 0 ) {
         fprintf( fp, " %ld", t[i].e3->va->id +1 );
      } else { fprintf( fp, " %ld", t[i].e3->vb->id +1 ); }
      fprintf( fp, "\n");
   }
   fclose( fp ); }
#endif
   return 0;
}


//
// Function that takes a pointer to a mesh object and fills its internals
// with the data forming two triangles that share two nodes. (This function
// is meant mostly for testing.)
//

int incg_MakeMesh_TwoTriangles( mesh_t* m )
{
   vertex_t *v;
   edge_t *e;
   triangle_t *t;
   int nt,ne,nv,i;


   if( m == NULL ) return 1;

   // allocate essential structures for two triangles
   nt = 2, nv = 4, ne = 5;
   v = (vertex_t *) malloc( ((size_t) nv) * sizeof( vertex_t ) );
   e = (edge_t *) malloc( ((size_t) ne) * sizeof( edge_t ) );
   t = (triangle_t*) malloc( ((size_t) nt) * sizeof( triangle_t ) );
   if( t == NULL || e == NULL || v == NULL ) {
      if( t != NULL ) free( t );
      if( e != NULL ) free( e );
      if( v != NULL ) free( v );
      return -1;
   }

   // make coefficients assignments for vertices
   v[0].x  = 0.0; v[0].y  = 0.0; v[0].z  = 0.0;  v[0].id = 0;
   v[1].x  = 1.0; v[1].y  = 0.0; v[1].z  = 0.0;  v[1].id = 1;
   v[2].x  = 0.0; v[2].y  = 1.0; v[2].z  = 0.0;  v[2].id = 2;
   v[3].x  = 1.0; v[3].y  = 1.0; v[3].z  = 0.0;  v[3].id = 3;

   // make vertex assignments for edges
   for(i=0;i<ne;++i) { e[i].tl = NULL;  e[i].tr = NULL; e[i].id = i; }
   e[0].va = &( v[0] ); e[0].vb = &( v[1] );
   e[1].va = &( v[1] ); e[1].vb = &( v[2] );
   e[2].va = &( v[2] ); e[2].vb = &( v[0] );
   e[3].va = &( v[1] ); e[3].vb = &( v[3] );
   e[4].va = &( v[3] ); e[4].vb = &( v[2] );

   // triangles
   for(i=0;i<nt;++i) t[i].id = i;
   i = 0;
   t[i].e1 = &( e[0] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[1] ); t[i].d2 = 0;
   t[i].e2->tl = &( t[i] );
   t[i].e3 = &( e[2] ); t[i].d3 = 0;
   t[i].e3->tl = &( t[i] );
   i = 1;
   t[i].e1 = &( e[1] ); t[i].d1 = 1;
   t[i].e1->tr = &( t[i] );
   t[i].e2 = &( e[3] ); t[i].d2 = 0;
   t[i].e2->tl = &( t[i] );
   t[i].e3 = &( e[4] ); t[i].d3 = 0;
   t[i].e3->tl = &( t[i] );

   m->nv = nv;
   m->ne = ne;
   m->nt = nt;
   m->v = v;
   m->e = e;
   m->t = t;
#ifdef _DEBUG_
{  FILE *fp = fopen( "TRIANGLES.dat", "w");
   fprintf( fp, "TITLE = \"Two triangles\" \n");
   fprintf( fp, "VARIABLES = x y z \n");
   fprintf( fp, "ZONE NODES=%ld, ELEMENTS=%ld \n", m->nv, m->nt );
   fprintf( fp, "     ZONETYPE=FETRIANGLE, DATAPACKING=POINT \n");
   for(i=0;i<(int) m->nv;++i) {
      fprintf( fp, " %16.9e %16.9e %16.9e \n",
                   m->v[i].x, m->v[i].y, m->v[i].z );
   }
   for(i=0;i<(int) m->nt;++i) {
      if( t[i].d1 == 0 ) {
         fprintf( fp, " %ld", t[i].e1->va->id +1 );
      } else { fprintf( fp, " %ld", t[i].e1->vb->id +1 ); }
      if( t[i].d2 == 0 ) {
         fprintf( fp, " %ld", t[i].e2->va->id +1 );
      } else { fprintf( fp, " %ld", t[i].e2->vb->id +1 ); }
      if( t[i].d3 == 0 ) {
         fprintf( fp, " %ld", t[i].e3->va->id +1 );
      } else { fprintf( fp, " %ld", t[i].e3->vb->id +1 ); }
      fprintf( fp, "\n");
   }
   fclose( fp ); }
#endif
   return 0;
}


//
// Function that takes a pointer to a mesh object and fills its internals
// with the data forming a unit cube made of triangles that share two nodes.
//

int incg_MakeMesh_Cube( mesh_t* m )
{
   vertex_t *v;
   edge_t *e;
   triangle_t *t;
   int nt,ne,nv,i;


   if( m == NULL ) return 1;

   // allocate essential structures for a cube
   nt = 12, nv = 8, ne = 18;
   v = (vertex_t *) malloc( ((size_t) nv) * sizeof( vertex_t ) );
   e = (edge_t *) malloc( ((size_t) ne) * sizeof( edge_t ) );
   t = (triangle_t*) malloc( ((size_t) nt) * sizeof( triangle_t ) );
   if( t == NULL || e == NULL || v == NULL ) {
      if( t != NULL ) free( t );
      if( e != NULL ) free( e );
      if( v != NULL ) free( v );
      return -1;
   }

   // make coefficients assignments for vertices (unroll Cartesian block)
   v[0].x  = 0.0; v[0].y  = 0.0; v[0].z  = 0.0;  v[0].id = 0;
   v[1].x  = 1.0; v[1].y  = 0.0; v[1].z  = 0.0;  v[1].id = 1;
   v[2].x  = 0.0; v[2].y  = 1.0; v[2].z  = 0.0;  v[2].id = 2;
   v[3].x  = 1.0; v[3].y  = 1.0; v[3].z  = 0.0;  v[3].id = 3;
   v[4].x  = 0.0; v[4].y  = 0.0; v[4].z  = 1.0;  v[4].id = 4;
   v[5].x  = 1.0; v[5].y  = 0.0; v[5].z  = 1.0;  v[5].id = 5;
   v[6].x  = 0.0; v[6].y  = 1.0; v[6].z  = 1.0;  v[6].id = 6;
   v[7].x  = 1.0; v[7].y  = 1.0; v[7].z  = 1.0;  v[7].id = 7;

   // make vertex assignments for edges (unroll by i,j,k _directions_)
   for(i=0;i<ne;++i) { e[i].tl = NULL;  e[i].tr = NULL; e[i].id = i; }
   // "i-direction"
   e[0].va = &( v[0] ); e[0].vb = &( v[1] );
   e[1].va = &( v[2] ); e[1].vb = &( v[3] );
   e[2].va = &( v[4] ); e[2].vb = &( v[5] );
   e[3].va = &( v[6] ); e[3].vb = &( v[7] );

   // "j-direction"
   e[4].va = &( v[0] ); e[4].vb = &( v[2] );
   e[5].va = &( v[1] ); e[5].vb = &( v[3] );
   e[6].va = &( v[4] ); e[6].vb = &( v[6] );
   e[7].va = &( v[5] ); e[7].vb = &( v[7] );

   // "k-direction"
   e[8].va = &( v[0] );  e[8].vb = &( v[4] );
   e[9].va = &( v[1] );  e[9].vb = &( v[5] );
   e[10].va = &( v[2] ); e[10].vb = &( v[6] );
   e[11].va = &( v[3] ); e[11].vb = &( v[7] );

   // "diagonals"
   e[12].va = &( v[0] );  e[12].vb = &( v[6] );
   e[13].va = &( v[1] );  e[13].vb = &( v[7] );
   e[14].va = &( v[0] );  e[14].vb = &( v[5] );
   e[15].va = &( v[2] );  e[15].vb = &( v[7] );
   e[16].va = &( v[0] );  e[16].vb = &( v[3] );
   e[17].va = &( v[4] );  e[17].vb = &( v[7] );

   // triangles
   for(i=0;i<nt;++i) t[i].id = i;
   i = 0;
   t[i].e1 = &( e[ 4] ); t[i].d1 = 1;
   t[i].e1->tr = &( t[i] );
   t[i].e2 = &( e[12] ); t[i].d2 = 0;
   t[i].e2->tl = &( t[i] );
   t[i].e3 = &( e[10] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );
   i = 1;
   t[i].e1 = &( e[ 8] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[ 6] ); t[i].d2 = 0;
   t[i].e2->tl = &( t[i] );
   t[i].e3 = &( e[12] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );
   i = 2;
   t[i].e1 = &( e[ 5] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[11] ); t[i].d2 = 0;
   t[i].e2->tl = &( t[i] );
   t[i].e3 = &( e[13] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );
   i = 3;
   t[i].e1 = &( e[13] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[ 7] ); t[i].d2 = 1;
   t[i].e2->tr = &( t[i] );
   t[i].e3 = &( e[ 9] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );
   i = 4;
   t[i].e1 = &( e[ 0] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[ 9] ); t[i].d2 = 0;
   t[i].e2->tl = &( t[i] );
   t[i].e3 = &( e[14] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );
   i = 5;
   t[i].e1 = &( e[14] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[ 2] ); t[i].d2 = 1;
   t[i].e2->tr = &( t[i] );
   t[i].e3 = &( e[ 8] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );
   i = 6;
   t[i].e1 = &( e[ 1] ); t[i].d1 = 1;
   t[i].e1->tr = &( t[i] );
   t[i].e2 = &( e[15] ); t[i].d2 = 0;
   t[i].e2->tl = &( t[i] );
   t[i].e3 = &( e[11] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );
   i = 7;
   t[i].e1 = &( e[10] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[ 3] ); t[i].d2 = 0;
   t[i].e2->tl = &( t[i] );
   t[i].e3 = &( e[15] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );
   i = 8;
   t[i].e1 = &( e[ 4] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[ 1] ); t[i].d2 = 0;
   t[i].e2->tl = &( t[i] );
   t[i].e3 = &( e[16] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );
   i = 9;
   t[i].e1 = &( e[16] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[ 5] ); t[i].d2 = 1;
   t[i].e2->tr = &( t[i] );
   t[i].e3 = &( e[ 0] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );
   i = 10;
   t[i].e1 = &( e[ 2] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[ 7] ); t[i].d2 = 0;
   t[i].e2->tl = &( t[i] );
   t[i].e3 = &( e[17] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );
   i = 11;
   t[i].e1 = &( e[17] ); t[i].d1 = 0;
   t[i].e1->tl = &( t[i] );
   t[i].e2 = &( e[ 3] ); t[i].d2 = 1;
   t[i].e2->tr = &( t[i] );
   t[i].e3 = &( e[ 6] ); t[i].d3 = 1;
   t[i].e3->tr = &( t[i] );

   m->nv = nv;
   m->ne = ne;
   m->nt = nt;
   m->v = v;
   m->e = e;
   m->t = t;
#ifdef _DEBUG_
{  FILE *fp = fopen( "CUBE.dat", "w");
   fprintf( fp, "TITLE = \"CUBE made of triangles\" \n");
   fprintf( fp, "VARIABLES = x y z \n");
   fprintf( fp, "ZONE NODES=%ld, ELEMENTS=%ld \n", m->nv, m->nt );
   fprintf( fp, "     ZONETYPE=FETRIANGLE, DATAPACKING=POINT \n");
   for(i=0;i<(int) m->nv;++i) {
      fprintf( fp, " %16.9e %16.9e %16.9e \n",
                   m->v[i].x, m->v[i].y, m->v[i].z );
   }
   for(i=0;i<(int) m->nt;++i) {
      if( t[i].d1 == 0 ) {
         fprintf( fp, " %ld", t[i].e1->va->id +1 );
      } else { fprintf( fp, " %ld", t[i].e1->vb->id +1 ); }
      if( t[i].d2 == 0 ) {
         fprintf( fp, " %ld", t[i].e2->va->id +1 );
      } else { fprintf( fp, " %ld", t[i].e2->vb->id +1 ); }
      if( t[i].d3 == 0 ) {
         fprintf( fp, " %ld", t[i].e3->va->id +1 );
      } else { fprintf( fp, " %ld", t[i].e3->vb->id +1 ); }
      fprintf( fp, "\n");
   }

   fclose( fp ); }
#endif
   return 0;
}


//
// Function that takes a pointer to a mesh object and performs uniform
// subdivision of all its triangles by spliting all edges, resulting in four
// times the number of triangles. (The function preserves node sharing of
// triangles that have a common edge.)
//

int incg_RefineMesh_Uniform( mesh_t* m )
{
   vertex_t *v;
   edge_t *e;
   triangle_t *t;
   long int nt,ne,nv,i;
   long int nt2,ne2,nv2;


   if( m == NULL ) return 1;
   if( m->nv == 0 || m->ne == 0 || m->nt == 0 ) return 2;

   nv = m->nv;
   ne = m->ne;
   nt = m->nt;

   // counts of new entities
   nv2 = nv + ne;         // old plus a new vertex at each old edge's midpoint
   ne2 = ne*2 + 3*nt;     // split edges and three edges inside a triangle
   nt2 = 4*nt;            // each triangle is split into four

   // allocate new structures
   v = (vertex_t *)  malloc( ((size_t) nv2) * sizeof( vertex_t ) );
   e = (edge_t *)    malloc( ((size_t) ne2) * sizeof( edge_t ) );
   t = (triangle_t*) malloc( ((size_t) nt2) * sizeof( triangle_t ) );
   if( v == NULL || e == NULL || t == NULL ) {
      if( t != NULL ) free( t );
      if( e != NULL ) free( e );
      if( v != NULL ) free( v );
      return -1;
   }

   // copy the first batch of vertices
   for(i=0;i<nv;++i) {
      memcpy( &( v[i] ), &( m->v[i] ), sizeof( vertex_t ) );
   }
   // In one sweep over edges...
   // 1. create new vertices (by sweeping over edges and splitting)
   // 2. create new edges (by splitting parent edges)
   for(i=0;i<ne;++i) {
      long int iv = nv + m->e[i].id;    // set index by order of parent edges
      long int ie = m->e[i].id * 2;     // index of (first) subdiv. edges
      vertex_t *mpv = &( v[ iv ] );     // new vertex that subdvd. a parent edge
      edge_t *ea = &( e[ie + 0] );      // new edges that subdvd. a parent edge
      edge_t *eb = &( e[ie + 1] );      // new edges that subdvd. a parent edge

      mpv->id = iv;
      mpv->x = ( m->e[i].va->x + m->e[i].vb->x )*0.5;
      mpv->y = ( m->e[i].va->y + m->e[i].vb->y )*0.5;
      mpv->z = ( m->e[i].va->z + m->e[i].vb->z )*0.5;

      ea->id = ie + 0;                   // id derived from parent edge index
      ea->va = &( v[ m->e[i].va->id ] ); // parent edge's vertex A (by index)
      ea->vb = mpv;                      // new vertex
      ea->tl = NULL;                     // edges made from splits need setting
      ea->tr = NULL;                     // edges made from splits need setting

      eb->id = ie + 1;                   // id derived from parent edge index
      eb->va = mpv;                      // new vertex
      eb->vb = &( v[ m->e[i].vb->id ] ); // parent edge's vertex B (by index)
      eb->tl = NULL;                     // edges made from splits need setting
      eb->tr = NULL;                     // edges made from splits need setting
   }
#ifdef _DEBUG_
   for(i=0;i<(int) nt2;++i) {
      t[i].d1 = 3; t[i].d2 = 3; t[i].d3 = 3;
      t[i].e1 = NULL; t[i].e2 = NULL; t[i].e3 = NULL;
   }
#endif

   // create new edges (by sweeping over triangles and using a convention)
   //       /\       First form edges of sub-triangle 1, unless neighbours of
   //      /3 \      parent triangle have lower IDs. Form edges of triangle 2.
   //     / __ \     Edges for triangles 3 & 4 formed as for triangle 1.
   //    /1\4 /2\    Parent triangle egdes are: bottom = 1, right = 2,...
   //   /___\/___\   (bad comments0
   for(i=0;i<nt;++i) {
      triangle_t *tp = &( m->t[i] );     // parent triangle
      edge_t *ea=NULL, *eb=NULL;         // new edges that subdvd. a parent edge
      edge_t *e1, *e2, *e3;              // new interior edges
      triangle_t *tc1, *tc2, *tc3, *tc4; // pointers to children triangles

      tc1 = &( t[ 4*i + 0] );    // new child triangle
      tc1->id = 4*i + 0;
      tc2 = &( t[ 4*i + 1] );    // new child triangle
      tc2->id = 4*i + 1;
      tc3 = &( t[ 4*i + 2] );    // new child triangle
      tc3->id = 4*i + 2;
      tc4 = &( t[ 4*i + 3] );    // new child triangle
      tc4->id = 4*i + 3;

      e1 = &( e[ 2*ne + 3*i + 0] );     // new interior edge object (by index)
      e1->id = 2*ne + 3*i + 0;
      e1->tl = tc4;
      e1->tr = tc2;
      e2 = &( e[ 2*ne + 3*i + 1] );     // new interior edge object (by index)
      e2->id = 2*ne + 3*i + 1;
      e2->tl = tc4;
      e2->tr = tc3;
      e3 = &( e[ 2*ne + 3*i + 2] );     // new interior edge object (by index)
      e3->id = 2*ne + 3*i + 2;
      e3->tl = tc4;
      e3->tr = tc1;

      tc1->e2 = e3;
      tc1->d2 = 1;
      tc4->e3 = e3;
      tc4->d3 = 0;

      tc2->e2 = e1;
      tc2->d2 = 1;
      tc4->e1 = e1;
      tc4->d1 = 0;

      tc3->e2 = e2;
      tc3->d2 = 1;
      tc4->e2 = e2;
      tc4->d2 = 0;

      // subtriangles 1,2,3

      // take the first edge of parent triangle; get edges via creation index
      ea = &( e[ tp->e1->id*2 + 0 ] );    // index of parent edge...
      eb = &( e[ tp->e1->id*2 + 1 ] );
      // take the first edge of parent triangle; boundaries will give null
      if( tp->d1 == 0 ) {     // edge direction in the triangle loop
      // tn1 = tp->e1->tr;    // neighb. is on the right side of the edge
      // if( tn1 != NULL ) {  // not a boundary
      //    if( tn1->id > tp->id ) {    // neighbour of parent not yet visited
      //       tn1 = NULL;    // pretend there is no neightbour for next steps
      //    } else {    // neighbour of parent has undergone edge construction
      //    }
      // } else {
      // }

         ea->tl = tc1;
         tc1->e1 = ea;
         tc1->d1 = 0;

         eb->tl = tc2;
         tc2->e3 = eb;
         tc2->d3 = 0;
      } else {                // edge direction in the triangle loop is oppos.
      // tn1 = tp->e1->tl;    // neighb. is on the left side of the edge
      // if( tn1 != NULL ) {  // not a boundary
      //    if( tn1->id > tp->id ) {    // neighbour of parent not yet visited
      //       tn1 = NULL;    // pretend there is no neightbour for next steps
      //    } else {    // neighbour of parent has undergone edge construction
      //    }
      // } else {
      // }

         ea->tr = tc2;
         tc2->e3 = ea;
         tc2->d3 = 1;

         eb->tr = tc1;
         tc1->e1 = eb;
         tc1->d1 = 1;
      }
      // new internal edges involving the new node on edge 1
      e3->vb = ea->vb;     // "ray" pointing to node
      e1->va = ea->vb;     // "ray" leaving node

      // take the first edge of parent triangle; get edges via creation index
      ea = &( e[ tp->e2->id*2 + 0 ] );    // index of parent edge...
      eb = &( e[ tp->e2->id*2 + 1 ] );
      // take the second edge of parent triangle; boundaries will give null
      if( tp->d2 == 0 ) {     // edge direction in the triangle loop
      // tn2 = tp->e2->tr;    // neighb. is on the right side of the edge
      // if( tn2 != NULL ) {  // not a boundary
      //    if( tn2->id > tp->id ) {    // neighbour of parent not yet visited
      //       tn2 = NULL;    // pretend there is no neightbour for next steps
      //    } else {    // neighbour of parent has undergone edge construction
      //    }
      // } else {
      // }

         ea->tl = tc2;
         tc2->e1 = ea;
         tc2->d1 = 0;

         eb->tl = tc3;
         tc3->e3 = eb;
         tc3->d3 = 0;
      } else {                // edge direction in the triangle loop is oppos.
      // tn2 = tp->e2->tl;    // neighb. is on the right side of the edge
      // if( tn2 != NULL ) {  // not a boundary
      //    if( tn2->id > tp->id ) {    // neighbour of parent not yet visited
      //       tn2 = NULL;    // pretend there is no neightbour for next steps
      //    } else {    // neighbour of parent has undergone edge construction
      //    }
      // } else {
      // }

         ea->tr = tc3;
         tc3->e3 = ea;
         tc3->d3 = 1;

         eb->tr = tc2;
         tc2->e1 = eb;
         tc2->d1 = 1;
      }
      // new internal edges involving the new node on edge 2
      e1->vb = ea->vb;     // "ray" pointing to node
      e2->va = ea->vb;     // "ray" leaving node

      // take the first edge of parent triangle; get edges via creation index
      ea = &( e[ tp->e3->id*2 + 0 ] );    // index of parent edge...
      eb = &( e[ tp->e3->id*2 + 1 ] );
      // take the third edge of parent triangle; boundaries will give null
      if( tp->d3 == 0 ) {     // edge direction in the triangle loop
      // tn3 = tp->e3->tr;    // neighb. is on the right side of the edge
      // if( tn3 != NULL ) {  // not a boundary
      //    if( tn3->id > tp->id ) {    // neighbour of parent not yet visited
      //       tn3 = NULL;    // pretend there is no neightbour for next steps
      //    } else {    // neighbour of parent has undergone edge construction
      //    }
      // } else {
      // }

         ea->tl = tc3;
         tc3->e1 = ea;
         tc3->d1 = 0;

         eb->tl = tc1;
         tc1->e3 = eb;
         tc1->d3 = 0;
      } else {                // edge direction in the triangle loop is oppos.
      // tn3 = tp->e3->tl;    // neighb. is on the right side of the edge
      // if( tn3 != NULL ) {  // not a boundary
      //    if( tn3->id > tp->id ) {    // neighbour of parent not yet visited
      //       tn3 = NULL;    // pretend there is no neightbour for next steps
      //    } else {    // neighbour of parent has undergone edge construction
      //    }
      // } else {
      // }

         ea->tr = tc1;
         tc1->e3 = ea;
         tc1->d3 = 1;

         eb->tr = tc3;
         tc3->e1 = eb;
         tc3->d1 = 1;
      }
      // new internal edges involving the new node on edge 3
      e2->vb = ea->vb;     // "ray" pointing to node
      e3->va = ea->vb;     // "ray" leaving node
   }
#ifdef _DEBUG_
{  FILE *fp = fopen( "NEW_EDGES.dat", "w");
   fprintf( fp, "TITLE = \"Edges of parent triangles after split\" \n");
   fprintf( fp, "VARIABLES = x y z \n");
   fprintf( fp, "ZONE T=\"edges\", NODES=%ld, ELEMENTS=%ld \n", nv2, ne2 );
   fprintf( fp, "     ZONETYPE=FELINESEG, DATAPACKING=POINT \n");
   for(i=0;i<(int) nv2;++i) {
      fprintf( fp, " %16.9e %16.9e %16.9e \n",
                   v[i].x, v[i].y, v[i].z );
   }
   for(i=0;i<(int) ne2;++i) {
      fprintf( fp, " %ld %ld \n", e[i].va->id +1, e[i].vb->id +1 );
   }
   fprintf( fp, "ZONE T=\"triangles\", NODES=%ld, ELEMENTS=%ld \n", nv2, nt2 );
   fprintf( fp, "     ZONETYPE=FETRIANGLE, DATAPACKING=POINT \n");
   for(i=0;i<(int) nv2;++i) {
      fprintf( fp, " %16.9e %16.9e %16.9e \n",
                   v[i].x, v[i].y, v[i].z );
   }
   for(i=0;i<(int) nt2;++i) {
      if( t[i].d1 == 0 ) {
         fprintf( fp, " %ld ", t[i].e1->va->id +1 );
      } else {
         fprintf( fp, " %ld ", t[i].e1->vb->id +1 );
      }
      if( t[i].d2 == 0 ) {
         fprintf( fp, " %ld ", t[i].e2->va->id +1 );
      } else {
         fprintf( fp, " %ld ", t[i].e2->vb->id +1 );
      }
      if( t[i].d3 == 0 ) {
         fprintf( fp, " %ld ", t[i].e3->va->id +1 );
      } else {
         fprintf( fp, " %ld ", t[i].e3->vb->id +1 );
      }
      fprintf( fp, "\n" );
   }
   fclose( fp ); }
#endif

   // release memory from incoming mesh object and re-assign
   free( m->v );
   m->v = v;
   m->nv = nv2;

   free( m->e );
   m->e = e;
   m->ne = ne2;

   free( m->t );
   m->t = t;
   m->nt = nt2;

   return 0;
}


#ifdef __cplusplus
}
#endif

