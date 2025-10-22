#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "smesh.h"
#include "smesh_uid_factory.h"

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------

//
// Function to set the UID of an edge given node indices
//

int set_edge_uid( edgeid_t & euid,
                  unsigned long i, unsigned long j )
{
   if( i < j ) {
      euid.ids.i = i;
      euid.ids.j = j;
      return 1;
   } else {
      euid.ids.i = j;
      euid.ids.j = i;
      return 2;
   }
}


//----------------------------------------------------------------------------

//
// Methods for the node object
//

sMesh_Node::sMesh_Node()
{
   uid = global_UID_factory->getNewNodeUID( this );

   flags = 0x00;
   x = -9.9e+99;
   y = -9.9e+99;
   z = -9.9e+99;
}

sMesh_Node::~sMesh_Node()
{

}

long sMesh_Node::getUID() const
{
   return uid;
}

int sMesh_Node::addRef( void *p )
{
   refs.push_back( p );
   return 0;
}

int sMesh_Node::removeRef( void *p )
{
   refs.remove( p );   // ERROR TRAP
   return 0;
}

unsigned int sMesh_Node::getNumRefs() const
{
   return( (unsigned int) refs.size() );
}


//
// Methods for the edge object
//

sMesh_Edge::sMesh_Edge( sMesh_Node * np1_, sMesh_Node * np2_ )
{
   uid = global_UID_factory->getNewEdgeUID( this );

   flags = 0x00;
   if( np1_->getUID() < np2_->getUID() ) {
      np1 = np1_;
      np2 = np2_;
   } else {
      np1 = np2_;
      np2 = np1_;
   }
   np1->addRef( (void*) this );
   np2->addRef( (void*) this );
}

sMesh_Edge::~sMesh_Edge()
{
   np1->removeRef( (void*) this );
   np2->removeRef( (void*) this );
}

long sMesh_Edge::getUID() const
{
   return uid;
}

int sMesh_Edge::addRef( void *p )
{
   refs.push_back( p );
   return 0;
}

int sMesh_Edge::removeRef( void *p )
{
   refs.remove( p );   // ERROR TRAP
   return 0;
}

unsigned int sMesh_Edge::getNumRefs() const
{
   return( (unsigned int) refs.size() );
}

sMesh_Node* sMesh_Edge::getNodePtr( int which_one ) const
{
   if( which_one == 1 ) return np1;
   else if( which_one == 2 ) return np2;
   else return NULL;
}

int sMesh_Edge::isBoundary() const
{
   // Bitwise-AND of [____ __X_] and [____ ___X] is 1 when both elements exist.
   unsigned int ui =   (flags & 0x01)
                   & ( (flags & 0x02) >> 1);
   // Subtracting from 1 gives 0 if it is an interior edge or 1 if it is trace.
   return 1 - ui;
}

int sMesh_Edge::computeLength()
{
   // REQUIRES ERROR-TRAPPING in theory
   double d = np1->x - np2->x;
   length = d*d;
          d = np1->y - np2->y;
   length += d*d;
          d = np1->z - np2->z;
   length += d*d;
   length = sqrt( length );

   return 0;
}

double sMesh_Edge::getLength() const
{
   return length;
}

sMesh_Edge* sMesh_Edge::getChildPtr( int which_one ) const
{
   if( which_one == 1 ) return cp1;
   else if( which_one == 2 ) return cp2;
   else return NULL;
}

int sMesh_Edge::isSplit() const
{
   if( cp1 == NULL ) return 0;
   return 1;
}

int sMesh_Edge::split( uidmap_t* node_uid, ptrmap_t* node_ptr,
                       uidmap_t* edge_uid, ptrmap_t* edge_ptr )
{
   if( isSplit() ) return 1;
   sMesh_Node* mnp = new sMesh_Node();
   if( mnp == NULL ) {
      FPRINTF( stdout, " [Error]  Could not create node object \n" );
      return -1;
   }
   if( node_uid != NULL ) (*node_uid)[ mnp->getUID() ] = mnp;
   if( node_ptr != NULL ) (*node_ptr)[ mnp ] = mnp->getUID();
   mnp->flags |= (0x01 << 7);

   // (The node order should be with the new node's UID being largest.)
   if( np1->getUID() > mnp->getUID() || np1->getUID() > mnp->getUID() ) {
      FPRINTF( stdout, " [Error]  Node UID error in splitting edge\n" );
      return 999;
   }
   mnp->x = 0.5*( np1->x + np2->x );
   mnp->y = 0.5*( np1->y + np2->y );
   mnp->z = 0.5*( np1->z + np2->z );

   // form two new edges (the order is 1st edge shares node 1)
   sMesh_Edge* sep1 = new sMesh_Edge( np1, mnp );
   if( sep1 == NULL ) {
      FPRINTF( stdout, " [Error]  Could not create edge object \n" );
      return -2;
   }
   if( edge_uid != NULL ) (*edge_uid)[ sep1->getUID() ] = sep1;
   if( edge_ptr != NULL ) (*edge_ptr)[ sep1 ] = sep1->getUID();

   sMesh_Edge* sep2 = new sMesh_Edge( np2, mnp );
   if( sep2 == NULL ) {
      FPRINTF( stdout, " [Error]  Could not create edge object \n" );
      return -3;
   }
   if( edge_uid != NULL ) (*edge_uid)[ sep2->getUID() ] = sep2;
   if( edge_ptr != NULL ) (*edge_ptr)[ sep2 ] = sep2->getUID();

   // assign splitting edge child pointers; convention is N1 -CE1- M -CE2- N2
   cp1 = sep1;
   cp2 = sep2;

   return 0;
}


//
// Methods for the quadrilateral object
//

// Constructor requires pointers to edge objects and 4 bits indicating the
// direction of each edge compared to the counter-clockwise convention of the
// quadrilateral.
sMesh_Quad::sMesh_Quad( sMesh_Edge * ne1_, sMesh_Edge * ne2_,
                        sMesh_Edge * ne3_, sMesh_Edge * ne4_,
                        unsigned char dirs_ )
{
   uid = global_UID_factory->getNewQuadUID( this );

   eptr[0] = ne1_;
   eptr[1] = ne2_;
   eptr[2] = ne3_;
   eptr[3] = ne4_;
   eattr = dirs_ & 0xF0;     // copy the first 4 bits [XXXX ____] as directions
                             // for the faces indexed [1234 ____]
   for(int k=0;k<4;++k) eptr[k]->addRef( (void*) this );
}

sMesh_Quad::~sMesh_Quad()
{
   for(int k=0;k<4;++k) eptr[k]->removeRef( (void*) this );
}

long sMesh_Quad::getUID() const
{
   return uid;
}

unsigned char sMesh_Quad::getEdgeAttr() const
{
   return eattr;
}

sMesh_Edge* sMesh_Quad::getEdgePtr( int which_one ) const
{
   if( which_one < 0 || 3 < which_one ) return NULL;
   return eptr[ which_one ];
}

int sMesh_Quad::needsSubdivision() const
{
   if( sattr ) return 0;
   int idiv=0;
   if( eptr[0]->isSplit() || eptr[2]->isSplit() ) idiv = 1;
   if( eptr[1]->isSplit() || eptr[3]->isSplit() ) idiv += 2;
   return idiv;
}

int sMesh_Quad::setChildren( int index, sMesh_Quad* p, ChildSetToken& token )
{
   if( index < 0 || 3 < index ) return 1;

   child[ index ] = p;

   if( index == 0 || index == 2 ) {
      sattr |= 0x01;
   } else {
      sattr |= 0x02;
   }

   return 0;
}


sMesh_Quad* sMesh_Quad::getChildPtr( int index ) const
{
   if( index < 0 || 3 < index ) return NULL;
   return child[ index ];
}



//
// Methods for the triangle object
//

// Constructor requires pointers to edge objects and 3 bits indicating the
// direction of each edge compared to the counter-clockwise convention of the
// triangle.
sMesh_Tri::sMesh_Tri( sMesh_Edge * ne1_, sMesh_Edge * ne2_, sMesh_Edge * ne3_,
                      unsigned char dirs_ )
{
   uid = global_UID_factory->getNewTriUID( this );

   eptr[0] = ne1_;
   eptr[1] = ne2_;
   eptr[2] = ne3_;
   eattr = dirs_ & 0xE0;     // copy the first 3 bits [XXX_ ____] as directions
                             // for the faces indexed [123__ ___]
   for(int k=0;k<3;++k) eptr[k]->addRef( (void*) this );
}

sMesh_Tri::~sMesh_Tri()
{
   for(int k=0;k<3;++k) eptr[k]->removeRef( (void*) this );
}

long sMesh_Tri::getUID() const
{
   return uid;
}

unsigned char sMesh_Tri::getEdgeAttr() const
{
   return eattr;
}

sMesh_Edge* sMesh_Tri::getEdgePtr( int which_one ) const
{
   if( which_one < 0 || 2 < which_one ) return NULL;
   return eptr[ which_one ];
}


int sMesh_Tri::computeHeuristics( unsigned char & attr_, double & len_max )
{
   const unsigned char bit7 = 0x01 << 7;
   const unsigned char bit6 = 0x01 << 6;
   const unsigned char bit5 = 0x01 << 5;
   const unsigned char bit4 = 0x01 << 4;

   len_max=0.0;
   double edge_len[3];
   for(int k=0;k<3;++k) {
      edge_len[k] = eptr[k]->getLength();
      if( edge_len[k] > len_max ) len_max = edge_len[k];
   }
   double edge_rat[3];
   edge_rat[0] = edge_len[1] / edge_len[0];    // ratio of 1 on 0
   edge_rat[1] = edge_len[2] / edge_len[1];    // ratio of 2 on 1
   edge_rat[2] = edge_len[0] / edge_len[2];    // ratio of 0 on 2

   const double ratio=0.15;    // thresshold for declaring isosceles at angle
   if( fabs( edge_rat[0] - 1.0/edge_rat[2] ) <= ratio ) hattr |= bit6;
   if( fabs( edge_rat[1] - 1.0/edge_rat[0] ) <= ratio ) hattr |= bit5;
   if( fabs( edge_rat[2] - 1.0/edge_rat[1] ) <= ratio ) hattr |= bit4;

   sMesh_Node* node_ptr[3];
   if( eattr & bit7 ) { node_ptr[0] = eptr[0]->getNodePtr(2);
                        node_ptr[1] = eptr[0]->getNodePtr(1); }
   else               { node_ptr[0] = eptr[0]->getNodePtr(1);
                        node_ptr[1] = eptr[0]->getNodePtr(2); }
   if( eattr & bit6 ) { node_ptr[2] = eptr[1]->getNodePtr(1); }
   else               { node_ptr[2] = eptr[1]->getNodePtr(2); }

   double angle_cos[3];     // angle opposite of an edge
   angle_cos[0] = (node_ptr[0]->x - node_ptr[2]->x) *   // nodes 0-2-1
                  (node_ptr[1]->x - node_ptr[2]->x)
                + (node_ptr[0]->y - node_ptr[2]->y) *
                  (node_ptr[1]->y - node_ptr[2]->y)
                + (node_ptr[0]->z - node_ptr[2]->z) *
                  (node_ptr[1]->z - node_ptr[2]->z);
   angle_cos[1] = (node_ptr[1]->x - node_ptr[0]->x) *   // nodes 1-0-2
                  (node_ptr[2]->x - node_ptr[0]->x)
                + (node_ptr[1]->y - node_ptr[0]->y) *
                  (node_ptr[2]->y - node_ptr[0]->y)
                + (node_ptr[1]->z - node_ptr[0]->z) *
                  (node_ptr[2]->z - node_ptr[0]->z);
   angle_cos[2] = (node_ptr[2]->x - node_ptr[1]->x) *   // nodes 2-1-0
                  (node_ptr[0]->x - node_ptr[1]->x)
                + (node_ptr[2]->y - node_ptr[1]->y) *
                  (node_ptr[0]->y - node_ptr[1]->y)
                + (node_ptr[2]->z - node_ptr[1]->z) *
                  (node_ptr[0]->z - node_ptr[1]->z);
   // bits for angles: [____ _123] (set for obtuse angles)
   if( angle_cos[0] < 0.0 ) hattr |= 0x04;
   if( angle_cos[1] < 0.0 ) hattr |= 0x02;
   if( angle_cos[2] < 0.0 ) hattr |= 0x01;

   attr_ = hattr;

   return 0;
}


int sMesh_Tri::setChildren( int index, void* p, ChildSetToken& token )
{
   if( index < 0 || 2 < index ) return 1;

   child[ index ] = p;

   if( index == 0 )      sattr |= 0x04;
   else if( index == 1 ) sattr |= 0x02;
   else if( index == 2 ) sattr |= 0x01;

   return 0;
}


void* sMesh_Tri::getChildPtr( int index ) const
{
   if( index < 0 || 2 < index ) return NULL;
   return child[ index ];
}


int sMesh_Tri::setSubdivision( unsigned char n, ChildSetToken& token )
{
   unsigned char tmp = (sattr & 0xF0) >> 4;
   if( tmp ) return 1;
   if( n > 4 ) return 2;

   sattr |= (n << 4);
   return 0;
}


//
// Methods for the mesh object
//

sMesh_Core::sMesh_Core()
{

}

sMesh_Core::~sMesh_Core()
{
   for(ptrmap_it pit = tri_ptr_map.begin(); pit != tri_ptr_map.end(); ++pit) {
      delete ((sMesh_Tri*) pit->first);
   }
   for(ptrmap_it pit = quad_ptr_map.begin(); pit != quad_ptr_map.end(); ++pit) {
      delete ((sMesh_Quad*) pit->first);
   }
   for(ptrmap_it pit = edge_ptr_map.begin(); pit != edge_ptr_map.end(); ++pit) {
      delete ((sMesh_Edge*) pit->first);
   }
   for(ptrmap_it pit = node_ptr_map.begin(); pit != node_ptr_map.end(); ++pit) {
      delete ((sMesh_Node*) pit->first);
   }
}


//
// Public method to receive arrays of node and element data (that is provided
// in a conventional sparse format) and generate the internals of a mesh
// object.
// NOTE: Quadrilaterals need to have a proper winding order. The order can be
// clockwise or counter-clockwise, but there has to be a winding order.
//

int sMesh_Core::loadData( int nno, const node_t nodes[],
                          int nel, const face_t faces[] )
{
   if( nno <= 0 || nel <= 0 ) {
      FPRINTF( stdout, " [Error]  Sizes (%d,%d) cannot be zero\n", nno, nel );
      return 1;
   }

   if( nodes == NULL || faces == NULL ) {
      FPRINTF( stdout, " [Error]  Pointers (%p,%p) cannot be null\n", nodes, faces );
      return 2;
   }

   // count triangles and quadrilaterals
   int ntri=0, nquad=0;
   for(int n=0;n<nel;++n) {
      if( faces[n].nodes[3] < 0 ) ++ntri;
      else ++nquad;
   }


   // continuation needs to trap individual object allocations
   int ierr=0;

   node_uid_map.clear();
   node_ptr_map.clear();
   for(int n=0;n<nno;++n) {
      sMesh_Node* np = new sMesh_Node();
      if( np == NULL ) { ierr=-111; break; }

      np->x = nodes[n].x;
      np->y = nodes[n].y;
      np->z = nodes[n].z;
      node_uid_map[ np->getUID() ] = np;
      node_ptr_map[ np ] = np->getUID();
   }

   // allocate the edges (derived from the faces)
   edge_uid_map.clear();
   edge_ptr_map.clear();

   nquad=0; ntri=0;
   for(int n=0;n<nel && ierr==0;++n) {
      char ic=4;
      if( faces[n].nodes[3] < 0 ) ic=3;

      long node_pairs[4][2] = {{-2,-2},{-2,-2},{-2,-2},{-2,-2}};
      for(int k=0;k<4;++k) {
         node_pairs[k][0] = faces[n].nodes[k];
         node_pairs[k][1] = faces[n].nodes[0];
         if( ic == 4 ) {
            if( k < 3 ) node_pairs[k][1] = faces[n].nodes[k+1];
         } else {
            if( k < 2 ) node_pairs[k][1] = faces[n].nodes[k+1];
         }
      }

      // allocate edges individually
      sMesh_Edge* edges_ptr[4] = {NULL, NULL, NULL, NULL};
      unsigned char edges_dir[4] = {9,9,9,9};
      for(int k=0;k<ic && ierr==0;++k) {
         edgeid_t eid;
         int idir = set_edge_uid( eid, (unsigned long) node_pairs[k][0],
                                       (unsigned long) node_pairs[k][1] );
         edges_dir[k] = (unsigned char) idir;
         edgeid_map_it eit;
         if( (eit = edgeid_map.find( eid )) == edgeid_map.end() ) {
            long nid1 = node_pairs[k][0];
            long nid2 = node_pairs[k][1];
            if( nid1 > nid2 ) {
               nid2 = node_pairs[k][0];
               nid1 = node_pairs[k][1];
            }
            sMesh_Node* np1 = (sMesh_Node*) node_uid_map[ nid1 ];
            sMesh_Node* np2 = (sMesh_Node*) node_uid_map[ nid2 ];
            sMesh_Edge* ep = new sMesh_Edge( np1, np2 );
            if( ep == NULL ) { ierr=-121; break; }

            ep->computeLength();
            edge_uid_map[ ep->getUID() ] = ep;
            edge_ptr_map[ ep ] = ep->getUID();
            edgeid_map[ eid ] = ep;
            edges_ptr[k] = ep;
         } else {
            edges_ptr[k] = (sMesh_Edge*) eit->second;
         }

         // set side bits
         if( idir == 1 ) edges_ptr[k]->flags |= 0x01;
         else            edges_ptr[k]->flags |= 0x02;
      }

      // create the face object
      unsigned dirs_=0x00;
      for(int k=0;k<ic;++k) if( edges_dir[k] == 2 ) dirs_ |= 0x01 << (3 - k);
      if( ic==4 ) {
         sMesh_Quad* qp = new sMesh_Quad( edges_ptr[0], edges_ptr[1],
                                          edges_ptr[2], edges_ptr[3],
                                          dirs_ << 4 );
         if( qp == NULL ) { ierr=-131; break; }
         quad_uid_map[ qp->getUID() ] = qp;
         quad_ptr_map[ qp ] = qp->getUID();
      } else {
         sMesh_Tri* tp = new sMesh_Tri( edges_ptr[0], edges_ptr[1],
                                        edges_ptr[2], dirs_ << 4 );
         if( tp == NULL ) { ierr=-131; break; }
         tri_uid_map[ tp->getUID() ] = tp;
         tri_ptr_map[ tp ] = tp->getUID();
      }
   }
   if( ierr ) {
      FPRINTF( stdout, " [Error]  Something went really wrong... \n" );
      return ierr;
   }

   return 0;
}


//
// Public method to receive arrays of node and element data (that is provided
// in a conventional sparse format) and generate the internals of a mesh
// object.
//

int sMesh_Core::quadify()
{
   struct trih_s { unsigned char attr; double s; };
   std::map< sMesh_Tri*, struct trih_s > tri_map_rules;
   for(ptrmap_it it=tri_ptr_map.begin(); it!=tri_ptr_map.end();++it) {
      sMesh_Tri* tp = (sMesh_Tri*) it->first;
      unsigned char attr;
      double tmp=0.0;
      tp->computeHeuristics( attr, tmp );
      // when any of the angles is obtuse
      if( attr & 0x07 ) {
         tri_map_rules[ tp ] = { attr, tmp };
      }
   }

   int ierr=0;
   std::map< sMesh_Tri*, struct trih_s > :: const_iterator it;
   for( it=tri_map_rules.begin(); it!=tri_map_rules.end(); ++it) {

      // find out which edge needs to be subdivided
      unsigned char attr = it->second.attr;
      int index=-2;
      if( attr & 0x04 ) index=0;
      if( attr & 0x02 ) { if( index==-2 ) { index=1; } else { index=-1; } }
      if( attr & 0x01 ) { if( index==-2 ) { index=2; } else { index=-1; } }
      // Should return an index {0,1,2}
      // "-1" is an error of duplicate obtuse angle!
      if( index < 0 ) {
         FPRINTF( stdout, " [Error]  Invalid Rule 3 index %d \n", ierr );
         ierr=999;
         break;
      } else {
         ierr=subdivideByRule3( it->first, index );
      }
   }
   if( ierr ) {
      FPRINTF( stdout, " [Error]  Something went really wrong... \n" );
      return 1;
   }
   tri_map_rules.clear();

   for(ptrmap_it it=tri_ptr_map.begin(); it!=tri_ptr_map.end();++it) {
      sMesh_Tri* tp = (sMesh_Tri*) it->first;
      unsigned char sattr = tp->getSubdivAttr();
      if( (sattr & 0x0F) == 0  ) {
         tri_map_rules[ tp ] = { sattr, 0.0 };
      }
   }

   for( it=tri_map_rules.begin(); it!=tri_map_rules.end(); ++it) {
      ierr=subdivideByRule1( it->first );
      if( ierr ) break;
   }
   if( ierr ) {
      FPRINTF( stdout, " [Error]  Something went really wrong... \n" );
      return 1;
   }
   tri_map_rules.clear();

   char ic=1;     // iteration
   while( ic && ierr==0 ) {

      std::map< sMesh_Quad*, unsigned char > quad_map_rules;
      for(ptrmap_it it=quad_ptr_map.begin(); it!=quad_ptr_map.end();++it) {
         sMesh_Quad* qp = (sMesh_Quad*) it->first;
         unsigned char uc = (unsigned char) qp->needsSubdivision();
         if( uc ) quad_map_rules[ qp ] = uc;
      }
      if( quad_map_rules.size() == 0 ) ic=0;    // iteration termination

      for(std::map< sMesh_Quad*, unsigned char > :: const_iterator
         it=quad_map_rules.begin(); it!=quad_map_rules.end();++it) {
         sMesh_Quad* qp = (sMesh_Quad*) it->first;
         if( it->second == 1 ) {
            ierr=subdivideByRule2u( qp );
         } else if( it->second == 2 ) {
            ierr=subdivideByRule2v( qp );
         } else if( it->second == 3 ) {
            // We will force a "u-subdivision" and allow the iteration to pick
            // up the subdivisions of the child quads in the next epoch.
            ierr=subdivideByRule2u( qp );
         } else {
            ierr=999;
         }
         if( ierr ) {
            ic=0;
            break;
         }
      }

#ifdef _DEBUG_
      dumpEdges( "edges.dat", 1 );
#endif
   }
   if( ierr ) {
      FPRINTF( stdout, " [Error]  Something went really wrong... \n" );
      return 1;
   }

   return 0;
}


//
// Function that performs subdivision by "rule 3" given an angle index
//

int sMesh_Core::subdivideByRule3( sMesh_Tri* p, int index )
{
   if( p == NULL ) {
      FPRINTF( stdout, " [Error]  Triangle pointer is null \n" );
      return 1;
   }
   if( index < 0 || 3 < index ) {
      FPRINTF( stdout, " [Error]  Bad index (%d) in R3 subdivision\n",
               index );
      return 2;
   }

   const unsigned char bit7 = 0x01 << 7;          // picks flags for face 0
   const unsigned char bit6 = 0x01 << 6;          // picks flags for face 1
   const unsigned char bit5 = 0x01 << 5;          // picks flags for face 2

   // get all the things we know about this triangle that we will need
   unsigned char eattr = p->getEdgeAttr();
   unsigned char sdir=0;                        // direction of split edge
   sMesh_Edge* sep = p->getEdgePtr( index );    // edge to split
   sMesh_Edge* oep1=NULL,* oep2=NULL;           // lateral edges of big triangle
   sMesh_Node* np=NULL;                         // opposite node
   unsigned char dirs[2] = { 0x00, 0x00 };      // edge directions of new tris
   if( index == 0 ) {
      if( eattr & bit7 ) sdir=1;
      oep1 = p->getEdgePtr(2);                  //        /|    Conventions
      oep2 = p->getEdgePtr(1);                  //       / |      0:  2 & 1
      if( eattr & bit5 ) dirs[0] |= bit7;       //    0 /  | 2             
      if( eattr & bit6 ) dirs[1] |= bit5;       //     /   |      1:  0 & 2
      if( eattr & bit5 ) {                      //    /____|               
         np = oep1->getNodePtr(2);              //       1        2:  1 & 0
      } else {
         np = oep1->getNodePtr(1);
      }
{     sMesh_Node* tmp=NULL;  // sanity check block
      if( eattr & bit6 ) tmp = oep2->getNodePtr(1);
      else tmp = oep2->getNodePtr(2);
      if( tmp == np ) {
      } else {
         FPRINTF( stdout, " [Error]  Opposite node: %ld \n", np->getUID() );
         return 999;
}     }  // endof sanity check block
   } else if( index == 1 ) {
      if( eattr & bit6 ) sdir=1;
      oep1 = p->getEdgePtr(0);
      oep2 = p->getEdgePtr(2);
      if( eattr & bit7 ) dirs[0] |= bit7;
      if( eattr & bit5 ) dirs[1] |= bit5;
      if( eattr & bit7 ) {
         np = oep1->getNodePtr(2);
      } else {
         np = oep1->getNodePtr(1);
      }
{     sMesh_Node* tmp=NULL;  // sanity check block
      if( eattr & bit5 ) tmp = oep2->getNodePtr(1);
      else tmp = oep2->getNodePtr(2);
      if( tmp == np ) {
      } else {
         FPRINTF( stdout, " [Error]  Opposite node: %ld \n", np->getUID() );
         return 999;
}     }  // endof sanity check block
   } else {
      if( eattr & bit5 ) sdir=1;
      oep1 = p->getEdgePtr(1);
      oep2 = p->getEdgePtr(0);
      if( eattr & bit6 ) dirs[0] |= bit7;
      if( eattr & bit7 ) dirs[1] |= bit5;
      if( eattr & bit6 ) {
         np = oep1->getNodePtr(2);
      } else {
         np = oep1->getNodePtr(1);
      }
{     sMesh_Node* tmp=NULL;  // sanity check block
      if( eattr & bit7 ) tmp = oep2->getNodePtr(1);
      else tmp = oep2->getNodePtr(2);
      if( tmp == np ) {
      } else {
         FPRINTF( stdout, " [Error]  Opposite node: %ld \n", np->getUID() );
         return 999;
}     }  // endof sanity check block
   }

   // spliting edge
   sMesh_Node* mnp=NULL;                // new or existing node at midpoint
   sMesh_Edge *sep1=NULL,*sep2=NULL;    // new edges splitting the original one
   if( sep->isSplit() ) {
   } else {
      int iret = sep->split( &node_uid_map, &node_ptr_map,
                             &edge_uid_map, &edge_ptr_map );
      if( iret ) {
         FPRINTF( stdout, " [Error]  Fatal in splitting edge attempt \n" );
         return 4;
      }
   }

   // get pointers to the two edges
   sep1 = sep->getChildPtr(1);
   sep2 = sep->getChildPtr(2);

   // orientations of split edges in the new triangles
   if( sdir ) {
      if( sep2->getNodePtr(1) != sep->getNodePtr(2) ) dirs[0] |= bit6;
      if( sep1->getNodePtr(2) != sep->getNodePtr(1) ) dirs[1] |= bit6;
   } else {
      if( sep1->getNodePtr(1) != sep->getNodePtr(1) ) dirs[0] |= bit6;
      if( sep2->getNodePtr(2) != sep->getNodePtr(2) ) dirs[1] |= bit6;
   }

   // locate middle node
   if( sep1->getNodePtr(1) == sep->getNodePtr(1) ) {
      mnp = sep1->getNodePtr(2);
   } else {
      mnp = sep1->getNodePtr(1);
   }
   if( mnp == sep->getNodePtr(1) || mnp == sep->getNodePtr(2) ) {
      FPRINTF( stdout, " [Error]  Midpoint node pointer error \n" );
      return 999;
   }

   // create bisecting edge
   sMesh_Edge* bep = new sMesh_Edge( np, mnp ); // new bisecting edge
   if( mnp == NULL ) {
      FPRINTF( stdout, " [Error]  Could not create edge object \n" );
      return -4;
   }
   edge_uid_map[ bep->getUID() ] = bep;
   edge_ptr_map[ bep ] = bep->getUID();
   // setting edge attribute of this edge for the triangles
   if( bep->getNodePtr(2) == np ) {
      dirs[1] |= bit7;
   } else {
      dirs[0] |= bit5;
   }

   // create two triangles
   sMesh_Tri *tp1=NULL, *tp2=NULL;
   if( sdir ) {
      tp1 = new sMesh_Tri( oep1, sep2, bep,  dirs[0] );
      tp2 = new sMesh_Tri( bep,  sep1, oep2, dirs[1] );
   } else {
      tp1 = new sMesh_Tri( oep1, sep1, bep,  dirs[0] );
      tp2 = new sMesh_Tri( bep,  sep2, oep2, dirs[1] );
   }
   if( tp1 == NULL || tp2 == NULL ) {
      FPRINTF( stdout, " [Error]  Could not create triangle object \n" );
      if( tp1 != NULL ) delete tp1;
      if( tp2 != NULL ) delete tp2;
      return -5;
   }
   tri_uid_map[ tp1->getUID() ] = tp1;
   tri_ptr_map[ tp1 ] = tp1->getUID();
   tri_uid_map[ tp2->getUID() ] = tp2;
   tri_ptr_map[ tp2 ] = tp2->getUID();

   // Area elements pointer assignments
   unsigned char sa = 1;
   if( index == 1 ) sa = 2;
   else if( index == 2 ) sa = 3;
   ChildSetToken token;
   p->setSubdivision( sa, token );
   p->setChildren( 0, tp1, token );
   p->setChildren( 1, tp2, token );

   return 0;
}


//
// Function that performs subdivision by "rule 1"
//

int sMesh_Core::subdivideByRule1( sMesh_Tri* p )
{
   if( p == NULL ) {
      FPRINTF( stdout, " [Error]  Triangle pointer is null \n" );
      return 1;
   }

   const unsigned char bit7 = 0x01 << 7;          // picks flags for face 0
   const unsigned char bit6 = 0x01 << 6;          // picks flags for face 1
   const unsigned char bit5 = 0x01 << 5;          // picks flags for face 2
   const unsigned char bit4 = 0x01 << 4;          // picks flags for face 3

   sMesh_Node* cnp = new sMesh_Node();      // "centroid" node
   if( cnp == NULL ) {
      FPRINTF( stdout, " [Error]  Could not create node object \n" );
      return -1;
   }
   node_uid_map[ cnp->getUID() ] = cnp;
   node_ptr_map[ cnp ] = cnp->getUID();
   cnp->x = 0.0; cnp->y = 0.0; cnp->z = 0.0;
   cnp->flags |= bit7;

   // get all the things we know about this triangle that we will need
   unsigned char eattr = p->getEdgeAttr();
   unsigned char dirs[3] = {0x00, 0x00, 0x00}; // edge directions of 3 new quads
   sMesh_Edge* qep[3][4] = {{ NULL, NULL, NULL, NULL },   // edges of quads
                            { NULL, NULL, NULL, NULL },
                            { NULL, NULL, NULL, NULL }};
   for(int k=0;k<3;++k) {    // sweep over edges
      sMesh_Edge* sep = p->getEdgePtr(k);
      unsigned char sdir=0, eqA=4, eqB=4;
      if( k==0 ) {
         sdir = (eattr & bit7) >> 7;
         eqA=0; eqB=1;
      } else if( k==1 ) {
         sdir = (eattr & bit6) >> 6;
         eqA=1; eqB=2;
      } else {
         sdir = (eattr & bit5) >> 5;
         eqA=2; eqB=0;
      }
      cnp->x += (sep->getNodePtr(1)->x + sep->getNodePtr(2)->x)/6.0;
      cnp->y += (sep->getNodePtr(1)->y + sep->getNodePtr(2)->y)/6.0;
      cnp->z += (sep->getNodePtr(1)->z + sep->getNodePtr(2)->z)/6.0;

      if( sep->isSplit() ) {
      } else {
         int iret = sep->split( &node_uid_map, &node_ptr_map,
                                &edge_uid_map, &edge_ptr_map );
         if( iret ) {
            FPRINTF( stdout, " [Error]  Fatal in splitting edge attempt \n" );
            return 2;
         }
      }

      // get pointers to the two edges
      sMesh_Edge* sep1 = sep->getChildPtr(1);
      sMesh_Edge* sep2 = sep->getChildPtr(2);


      // orientations and pointers of split edges in the new triangles
      if( sdir ) {
         if( sep2->getNodePtr(1) != sep->getNodePtr(2) ) dirs[eqA] |= bit7;
         if( sep1->getNodePtr(2) != sep->getNodePtr(1) ) dirs[eqB] |= bit4;
         qep[eqA][0] = sep2;
         qep[eqB][3] = sep1;
      } else {
         if( sep1->getNodePtr(1) != sep->getNodePtr(1) ) dirs[eqA] |= bit7;
         if( sep2->getNodePtr(2) != sep->getNodePtr(2) ) dirs[eqB] |= bit4;
         qep[eqA][0] = sep1;
         qep[eqB][3] = sep2;
      }

      // locate midpoint node
      sMesh_Node* mnp=NULL;
      if( sep1->getNodePtr(1) == sep->getNodePtr(1) ) {
         mnp = sep1->getNodePtr(2);
      } else {
         mnp = sep1->getNodePtr(1);
      }

      // create mid-point connecting edge
      sMesh_Edge* tmp = new sMesh_Edge( cnp, mnp );
      if( tmp == NULL ) {
         FPRINTF( stdout, " [Error]  Could not create edge object \n" );
         return -2;
      }
      edge_uid_map[ tmp->getUID() ] = tmp;
      edge_ptr_map[ tmp ] = tmp->getUID();

      // store pointers to new edge
      qep[eqA][1] = tmp;
      qep[eqB][2] = tmp;
      if( mnp->getUID() > cnp->getUID() ) {
         dirs[eqA] |= bit6;
      } else {
         dirs[eqB] |= bit5;
      }
   }

   // create three quadrilaterals
   for(int k=0;k<3;++k) {
      sMesh_Quad* tmp = new sMesh_Quad( qep[k][0], qep[k][1],
                                        qep[k][2], qep[k][3], dirs[k] );
      if( tmp == NULL ) {
         FPRINTF( stdout, " [Error]  Could not create area object \n" );
         return -3;
      }
      quad_uid_map[ tmp->getUID() ] = tmp;
      quad_ptr_map[ tmp ] = tmp->getUID();
   }

   return 0;
}


//
// Function that performs subdivision by "rule 2" in the "u" direction
//

int sMesh_Core::subdivideByRule2u( sMesh_Quad* p )
{
   if( p == NULL ) {
      FPRINTF( stdout, " [Error]  Quadrilateral pointer is null \n" );
      return 1;
   }

   const unsigned char bit7 = 0x01 << 7;          // picks flags for face 0
   const unsigned char bit6 = 0x01 << 6;          // picks flags for face 1
   const unsigned char bit5 = 0x01 << 5;          // picks flags for face 2
   const unsigned char bit4 = 0x01 << 4;          // picks flags for face 3

   unsigned char eattr = p->getEdgeAttr();     // adge attributes of quad
   sMesh_Node *mnpA=NULL, *mnpB=NULL;          // mid-edge nodes (diagram order)
   unsigned char dirs[2] = { 0x00, 0x00 };     // edge directions of new quads
   sMesh_Edge* qep[2][4] = {{ NULL, NULL, NULL, NULL },   // edges of quads
                            { NULL, NULL, NULL, NULL }};

   // work on edge 3 ("left")
   qep[0][3] = p->getEdgePtr(3);
   if( eattr & bit4 ) dirs[0] |= bit4;

   // work on edge 1 ("right")
   qep[1][1] = p->getEdgePtr(1);
   if( eattr & bit6 ) dirs[1] |= bit6;

   // work on edge 0 ("bottom")
   sMesh_Edge *etmp = p->getEdgePtr(0);        // get edge 0 of the quad
   if( etmp->isSplit() ) {                     // edge 0 is split
   } else {
      int iret = etmp->split( &node_uid_map, &node_ptr_map,
                              &edge_uid_map, &edge_ptr_map );
      if( iret ) {
         FPRINTF( stdout, " [Error]  Fatal in splitting edge attempt \n" );
         return 2;
      }
   }

   // locate midpoint of "bottom" edge
   if( eattr & bit7 ) {                     // direction is reversed
      // working on "left" quad
      sMesh_Edge* etmp2 = etmp->getChildPtr(2);         // get 2nd child
      qep[0][0] = etmp2;                                // edge 0 of quad A
      if( etmp2->getNodePtr(2) == etmp->getNodePtr(2) ) {
         mnpA = etmp2->getNodePtr(1);
         dirs[0] |= bit7;
      } else {
         mnpA = etmp2->getNodePtr(2);
      }
      // working on "right" quad
      etmp2 = etmp->getChildPtr(1);                     // get 1st child
      qep[1][0] = etmp2;                                // edge 0 of quad B
      if( etmp2->getNodePtr(1) == etmp->getNodePtr(1) ) {
         dirs[1] |= bit7;
      }
   } else {
      // working on "left" quad
      sMesh_Edge* etmp2 = etmp->getChildPtr(1);         // get 1st child
      qep[0][0] = etmp2;                                // edge 0 of quad A
      if( etmp2->getNodePtr(2) == etmp->getNodePtr(1) ) {
         mnpA = etmp2->getNodePtr(1);
         dirs[0] |= bit7;
      } else {
         mnpA = etmp2->getNodePtr(2);
      }
      // working on "right" quad
      etmp2 = etmp->getChildPtr(2);                     // get 2nd child
      qep[1][0] = etmp2;                                // edge 3 of quad B
      if( etmp2->getNodePtr(1) == etmp->getNodePtr(2) ) {
         dirs[1] |= bit7;
      }
   }

   // work on edge 2 ("top")
   etmp = p->getEdgePtr(2);                    // get edge 2 of the quad
   if( etmp->isSplit() ) {                     // edge 2 is split
   } else {
      int iret = etmp->split( &node_uid_map, &node_ptr_map,
                              &edge_uid_map, &edge_ptr_map );
      if( iret ) {
         FPRINTF( stdout, " [Error]  Fatal in splitting edge attempt \n" );
         return 2;
      }
   }

   // locate midpoint of "top" edge
   if( eattr & bit5 ) {                     // direction is reversed
      // working on "left" quad
      sMesh_Edge* etmp2 = etmp->getChildPtr(1);         // get 1st child
      qep[0][2] = etmp2;                                // edge 0 of quad A
      if( etmp2->getNodePtr(1) == etmp->getNodePtr(1) ) {
         mnpB = etmp2->getNodePtr(2);
         dirs[0] |= bit5;
      } else {
         mnpB = etmp2->getNodePtr(1);
      }
      // working on "right" quad
      etmp2 = etmp->getChildPtr(2);                     // get 2nd child
      qep[1][2] = etmp2;                                // edge 2 of quad B
      if( etmp2->getNodePtr(2) == etmp->getNodePtr(2) ) {
         dirs[1] |= bit5;
      }
   } else {
      // working on "left" quad
      sMesh_Edge* etmp2 = etmp->getChildPtr(2);         // get 2nd child
      qep[0][2] = etmp2;                                // edge 2 of quad A
      if( etmp2->getNodePtr(1) == etmp->getNodePtr(2) ) {
         mnpB = etmp2->getNodePtr(2);
         dirs[0] |= bit5;
      } else {
         mnpB = etmp2->getNodePtr(1);
      }
      // working on "right" quad
      etmp2 = etmp->getChildPtr(1);                     // get 1st child
      qep[1][2] = etmp2;                                // edge 2 of quad B
      if( etmp2->getNodePtr(2) == etmp->getNodePtr(1) ) {
         dirs[1] |= bit5;
      }
   }

   // sanity check
   if( mnpA == NULL || mnpB == NULL ) {
      FPRINTF( stdout, " [Error]  Midpoint pointers cannot be null %p, %p\n",
               mnpA, mnpB );
      return 999;
   }

   // create element-splitting edge
   sMesh_Edge* sep = new sMesh_Edge( mnpA, mnpB );
   if( sep == NULL ) {
      FPRINTF( stdout, " [Error]  Could not create edge object \n" );
      return -1;
   }
   edge_uid_map[ sep->getUID() ] = sep;
   edge_ptr_map[ sep ] = sep->getUID();
   // assign pointers
   qep[0][1] = sep;
   qep[1][3] = sep;

   // orientation
   if( mnpA->getUID() < mnpB->getUID() ) {
      dirs[1] |= bit4;
   } else {
      dirs[0] |= bit6;
   }

   // create two quadrilaterals
   for(int k=0;k<2;++k) {
      sMesh_Quad* tmp = new sMesh_Quad( qep[k][0], qep[k][1],
                                        qep[k][2], qep[k][3], dirs[k] );
      if( tmp == NULL ) {
         FPRINTF( stdout, " [Error]  Could not create area object \n" );
         return -2;
      }
      quad_uid_map[ tmp->getUID() ] = tmp;
      quad_ptr_map[ tmp ] = tmp->getUID();

      // set pointer to child
      ChildSetToken token;
      if( k==0 ) p->setChildren( 0, tmp, token );
      if( k==1 ) p->setChildren( 1, tmp, token );
   }

   return 0;
}


//
// Function that performs subdivision by "rule 2" in the "v" direction
//

int sMesh_Core::subdivideByRule2v( sMesh_Quad* p )
{
   if( p == NULL ) {
      FPRINTF( stdout, " [Error]  Quadrilateral pointer is null \n" );
      return 1;
   }

   const unsigned char bit7 = 0x01 << 7;          // picks flags for face 0
   const unsigned char bit6 = 0x01 << 6;          // picks flags for face 1
   const unsigned char bit5 = 0x01 << 5;          // picks flags for face 2
   const unsigned char bit4 = 0x01 << 4;          // picks flags for face 3

   unsigned char eattr = p->getEdgeAttr();     // adge attributes of quad
   sMesh_Node *mnpA=NULL, *mnpB=NULL;          // mid-edge nodes (diagram order)
   unsigned char dirs[2] = { 0x00, 0x00 };     // edge directions of new quads
   sMesh_Edge* qep[2][4] = {{ NULL, NULL, NULL, NULL },   // edges of quads
                            { NULL, NULL, NULL, NULL }};

   // work on edge 0 ("bottom")
   qep[0][0] = p->getEdgePtr(0);
   if( eattr & bit7 ) dirs[0] |= bit7;

   // work on edge 2 ("top")
   qep[1][2] = p->getEdgePtr(2);
   if( eattr & bit5 ) dirs[1] |= bit5;

   // work on edge 3 ("left")
   sMesh_Edge *etmp = p->getEdgePtr(3);        // get edge 3 of the quad
   if( etmp->isSplit() ) {                     // edge 3 is split
   } else {
      int iret = etmp->split( &node_uid_map, &node_ptr_map,
                              &edge_uid_map, &edge_ptr_map );
      if( iret ) {
         FPRINTF( stdout, " [Error]  Fatal in splitting edge attempt \n" );
         return 2;
      }
   }

   // locate midpoint of "left" edge
   if( eattr & bit4 ) {                     // direction is reversed
      // working on "bottom" quad
      sMesh_Edge* etmp2 = etmp->getChildPtr(1);         // get 1st child
      qep[0][3] = etmp2;                                // edge 3 of quad A
      if( etmp2->getNodePtr(1) == etmp->getNodePtr(1) ) {
         mnpA = etmp2->getNodePtr(2);
         dirs[0] |= bit4;
      } else {
         mnpA = etmp2->getNodePtr(1);
      }
      // working on "top" quad
      etmp2 = etmp->getChildPtr(2);                     // get 2nd child
      qep[1][3] = etmp2;                                // edge 3 of quad B
      if( etmp2->getNodePtr(2) == etmp->getNodePtr(2) ) {
         dirs[1] |= bit4;
      }
   } else {
      // working on "bottom" quad
      sMesh_Edge* etmp2 = etmp->getChildPtr(2);         // get 2nd child
      qep[0][3] = etmp2;                                // edge 3 of quad A
      if( etmp2->getNodePtr(1) == etmp->getNodePtr(2) ) {
         mnpA = etmp2->getNodePtr(2);
         dirs[0] |= bit4;
      } else {
         mnpA = etmp2->getNodePtr(1);
      }
      // working on "top" quad
      etmp2 = etmp->getChildPtr(1);                     // get 1st child
      qep[1][3] = etmp2;                                // edge 3 of quad B
      if( etmp2->getNodePtr(2) == etmp->getNodePtr(1) ) {
         dirs[1] |= bit4;
      }
   }

   // work on edge 1 ("right")
   etmp = p->getEdgePtr(1);                    // get edge 1 of the quad
   if( etmp->isSplit() ) {                     // edge 1 is split
   } else {
      int iret = etmp->split( &node_uid_map, &node_ptr_map,
                              &edge_uid_map, &edge_ptr_map );
      if( iret ) {
         FPRINTF( stdout, " [Error]  Fatal in splitting edge attempt \n" );
         return 3;
      }
   }

   // locate midpoint of "right" edge
   if( eattr & bit6 ) {                     // direction is reversed
      sMesh_Edge* etmp2 = etmp->getChildPtr(1);         // get 1st child
      qep[1][1] = etmp2;                                // edge 1 of quad B
      if( etmp2->getNodePtr(1) == etmp->getNodePtr(1) ) {
         mnpB = etmp2->getNodePtr(2);
         dirs[1] |= bit6;
      } else {
         mnpB = etmp2->getNodePtr(1);
      }
      etmp2 = etmp->getChildPtr(2);                     // get 2nd child
      qep[0][1] = etmp2;                                // edge 1 of quad A
      if( etmp2->getNodePtr(2) == etmp->getNodePtr(2) ) {
         dirs[1] |= bit6;
      }
   } else {
      sMesh_Edge* etmp2 = etmp->getChildPtr(2);         // get 2nd child
      qep[1][1] = etmp2;                                // edge 1 of quad B
      if( etmp2->getNodePtr(1) == etmp->getNodePtr(2) ) {
         mnpB = etmp2->getNodePtr(2);
         dirs[1] |= bit6;
      } else {
         mnpB = etmp2->getNodePtr(1);
      }
      etmp2 = etmp->getChildPtr(1);                     // get 1st child
      qep[0][1] = etmp2;                                // edge 1 of quad A
      if( etmp2->getNodePtr(2) == etmp->getNodePtr(1) ) {
         dirs[0] |= bit6;
      }
   }

   // sanity check
   if( mnpA == NULL || mnpB == NULL ) {
      FPRINTF( stdout, " [Error]  Midpoint pointers cannot be null %p, %p\n",
               mnpA, mnpB );
      return 999;
   }

   // create element-splitting edge
   sMesh_Edge* sep = new sMesh_Edge( mnpA, mnpB );
   if( sep == NULL ) {
      FPRINTF( stdout, " [Error]  Could not create edge object \n" );
      return -1;
   }
   edge_uid_map[ sep->getUID() ] = sep;
   edge_ptr_map[ sep ] = sep->getUID();
   // assign pointers
   qep[0][2] = sep;
   qep[1][0] = sep;

   // orientation
   if( mnpA->getUID() < mnpB->getUID() ) {
      dirs[0] |= bit5;
   } else {
      dirs[1] |= bit7;
   }

   // create two quadrilaterals
   for(int k=0;k<2;++k) {
      sMesh_Quad* tmp = new sMesh_Quad( qep[k][0], qep[k][1],
                                        qep[k][2], qep[k][3], dirs[k] );
      if( tmp == NULL ) {
         FPRINTF( stdout, " [Error]  Could not create area object \n" );
         return -2;
      }
      quad_uid_map[ tmp->getUID() ] = tmp;
      quad_ptr_map[ tmp ] = tmp->getUID();

      // set pointer to child
      ChildSetToken token;
      if( k==0 ) p->setChildren( 2, tmp, token );
      if( k==1 ) p->setChildren( 3, tmp, token );
   }

   return 0;
}


#ifdef _DEBUG_
int sMesh_Core::dumpEdges( const char filename[], int iop ) const
{
#ifdef _DEBUG_
   char FUNC[] = "dumpEdges";
#endif
   fprintf( stdout, " [sMesh_Core:%s]  Dumping edges for Gnuplot-ing\n",FUNC );
   FILE *fp = fopen( filename, "w" );
   fprintf( fp, "# Edges after Rule 3 subdivision\n" );
   uidmap_it eit;
   for( eit = edge_uid_map.begin(); eit != edge_uid_map.end(); ++eit ) {
      sMesh_Edge* ep = (sMesh_Edge*) eit->second;
      int ic=1;   // default is to print
      if( ep->isSplit() == 1 && iop == 1 ) ic=0;   // skip split edges

      if( ic ) {
         sMesh_Node* np = ep->getNodePtr( 1 );
         fprintf( fp, " %lf %lf \n", np->x, np->y );
                     np = ep->getNodePtr( 2 );
         fprintf( fp, " %lf %lf \n", np->x, np->y );
         fprintf( fp, "\n" );
         fprintf( stdout, " [%ld]  %ld %ld   bc: %d   ", ep->getUID(),
                  ep->getNodePtr(1)->getUID(), ep->getNodePtr(2)->getUID(),
                  ep->isBoundary() );
         if( ep->getNodePtr(1)->getUID() > ep->getNodePtr(2)->getUID() )
             { fprintf( stdout, "BAD! \n" ); } else { fprintf( stdout, "\n"); }
         }
   }
   fclose(fp);

   return 0;
}
#endif



#ifdef __cplusplus
}
#endif


#ifdef _COMMENTED_OUT_
Deleted after commit: c537f4257a3bc87a1586b341d9eed69e64e7fb14
#endif

