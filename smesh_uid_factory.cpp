#include <stdio.h>
#include <stdlib.h>

#include "smesh_uid_factory.h"
#include "smesh.h"

#ifdef _USE_HDF_
#include <hdf5.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif



sMesh_uid_factory::sMesh_uid_factory()
{
   node_uid = -1;
   edge_uid = -1;
   quad_uid = -1;
   tri_uid = -1;
}

sMesh_uid_factory::~sMesh_uid_factory()
{
   nodes.clear();
   edges.clear();
   quads.clear();
   tris.clear();
}

long sMesh_uid_factory::getLastNodeUID() const
{
   return( node_uid );
}

long sMesh_uid_factory::getLastEdgeUID() const
{
   return( edge_uid );
}

long sMesh_uid_factory::getLastQuadUID() const
{
   return( quad_uid );
}

long sMesh_uid_factory::getLastTriUID() const
{
   return( tri_uid );
}

long sMesh_uid_factory::getNewNodeUID( sMesh_Node *p )
{
   nodes.push_back( p );
   ++node_uid;
   return( node_uid );
}

long sMesh_uid_factory::getNewEdgeUID( sMesh_Edge *p )
{
   edges.push_back( p );
   ++edge_uid;
   return( edge_uid );
}

long sMesh_uid_factory::getNewQuadUID( sMesh_Quad *p )
{
   quads.push_back( p );
   ++quad_uid;
   return( quad_uid );
}

long sMesh_uid_factory::getNewTriUID( sMesh_Tri *p )
{
   tris.push_back( p );
   ++tri_uid;
   return( tri_uid );
}

sMesh_Node * sMesh_uid_factory::getNodePtr( long index_ ) const
{
   sMesh_Node *p = nodes[ index_ ];

   return( p );
}

sMesh_Edge * sMesh_uid_factory::getEdgePtr( long index_ ) const
{
   sMesh_Edge *p = edges[ index_ ];

   return( p );
}

sMesh_Quad * sMesh_uid_factory::getQuadPtr( long index_ ) const
{
   sMesh_Quad *p = quads[ index_ ];

   return( p );
}

sMesh_Tri * sMesh_uid_factory::getTriPtr( long index_ ) const
{
   sMesh_Tri *p = tris[ index_ ];

   return( p );
}

long sMesh_uid_factory::sizeNodes() const
{
   long isize = (long) nodes.size();

   return( isize );
}

long sMesh_uid_factory::sizeEdges() const
{
   long isize = (long) edges.size();
   
   return( isize );
}

long sMesh_uid_factory::sizeQuads() const
{
   long isize = (long) quads.size();
   
   return( isize );
}

long sMesh_uid_factory::sizeTris() const
{
   long isize = (long) tris.size();
   
   return( isize );
}


// --------------------------------------------------------------------
// Supporting octree UID factory functions
// --------------------------------------------------------------------

//
// Function to dump the contents
//
int dump_factory_data( sMesh_uid_factory *p, char *filename )
{
   if( p == NULL ) {
      printf( "A null uid_factory pointer was provided \n" );
      return 1;
   }

   if( filename == NULL ) {
      printf( "A null filename was provided \n" );
      return 2;
   }

   FILE *fp = fopen( filename,"w" );
   if( fp == NULL ) {
      printf( "Could not open file \"%s\"\n", filename );
      return 3;
   }

   long num_nodes = (long) p->nodes.size();
   long num_edges = (long) p->edges.size();
   long num_quads = (long) p->quads.size();
   long num_tris  = (long) p->tris.size();
   fprintf( stdout, "Nodes %ld Edges %ld Quads %ld Tris %ld \n",
            num_nodes, num_edges, num_quads, num_tris );

   fclose( fp );

   fp = fopen( filename, "a" );
   if( fp == NULL ) {
      printf( "Could not re-open file \"%s\"\n", filename );
      return 4;
   }

   fprintf(fp,"zone T=\"face_lines\", N=%ld, E=%ld, F=FEPOINT,ET=LINESEG\n",
               2*num_edges, num_edges );
   for(long n=0;n<num_edges;++n) {
   }

   fclose( fp );

   return 0;
}


#ifdef __cplusplus
}
#endif

