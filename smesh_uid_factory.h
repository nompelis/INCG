#include <stdio.h>
#include <stdlib.h>

#ifndef _SMESH_UID_FACTORY_H_
#define _SMESH_UID_FACTORY_H_


#include <vector>

#ifdef __cplusplus
extern "C" {
#endif


//
// Forward declaration of surface-mesh classes
//

class sMesh_Node;
class sMesh_Edge;
class sMesh_Quad;
class sMesh_Tri;


//
// A class that serializes and keeps track of objects it creates
//

class sMesh_uid_factory {
public:
   sMesh_uid_factory();
   ~sMesh_uid_factory();

   long getLastNodeUID() const;
   long getLastEdgeUID() const;
   long getLastQuadUID() const;
   long getLastTriUID() const;

   long getNewNodeUID( sMesh_Node *p );
   long getNewEdgeUID( sMesh_Edge *p );
   long getNewQuadUID( sMesh_Quad *p );
   long getNewTriUID( sMesh_Tri *p );

   long sizeNodes() const;
   long sizeEdges() const;
   long sizeQuads() const;
   long sizeTris() const;

   sMesh_Node * getNodePtr( long index_ ) const;
   sMesh_Edge * getEdgePtr( long index_ ) const;
   sMesh_Quad * getQuadPtr( long index_ ) const;
   sMesh_Tri * getTriPtr( long index_ ) const;

   std::vector< sMesh_Node * > nodes;
   std::vector< sMesh_Edge * > edges;
   std::vector< sMesh_Quad * > quads;
   std::vector< sMesh_Tri * > tris;

protected:

private:
   long node_uid, edge_uid, tri_uid, quad_uid;

};



// --------------------------------------------------------------------
// Supporting surface-mesh UID factory function prototypes
// --------------------------------------------------------------------

int dump_factory_data( sMesh_uid_factory *p, char *filename );


#ifdef __cplusplus
}
#endif
#endif
