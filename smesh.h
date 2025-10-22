#include <stdio.h>
#include <stdlib.h>

#ifndef _SMESH_H_
#define _SMESH_H_

#include <map>
#include <vector>
#include <list>

//#include "inalloc.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "debug.h"


//
// structs used for loading and visualization
//

typedef struct node_s {
   double x,y,z;
} node_t;

typedef struct face_s {
   long nodes[4];
} face_t;



//
// a unique identifier that acts as a "hash"; operators are included
// It is a "union" comprised of two structs; the first is used when programming
// while the second is used for the "comparator" inside key-value pair maps.
//

struct edgeid_s {
   // the primal "indices" that we set in an octree grid
   // (Each of of the 64-bit i,j,k store in big endian the direction index in
   // their high 32 bits and the subdivision binary in their lower 32 bits.)
   unsigned long i,j;        // two node indices in big endian interpretation
};

struct edgebytes_s {
#define UIDSIZE 16
   unsigned char digits[UIDSIZE];    // indices encoding in machine endian-ness

   // equality testing (unary?) operator
   bool operator==( const struct edgebytes_s & other ) const {
      int n=UIDSIZE-1;
      while( n < UIDSIZE ) {   // sweep is little endian, but it does not matter
         if( digits[n] != other.digits[n] ) return false;
         --n;
      }
      return true;
   }

   // ordinality comparison operator (comparator)
   bool operator<( const struct edgebytes_s & other ) const {
      int n=UIDSIZE-1;
      while( n >= 0 ) {        // sweep is big endian
         if( digits[n] != other.digits[n] ) {
           if( digits[n] > other.digits[n] ) return false;
           return true;
         }
         --n;
      }
      return false;    // all digits are equal
   }
};

union edgeid_u {
   struct edgeid_s ids;
   struct edgebytes_s bytes;

   // equality testing (unary?) operator
   bool operator==( const edgeid_u & other ) const {
      int n=0;
      while( n < UIDSIZE ) {   // sweep is little endian, but it does not matter
         if( bytes.digits[n] != other.bytes.digits[n] ) return false;
         --n;
      }
      return true;
   }

   // ordinality comparison operator (comparator)
   bool operator<( const edgeid_u & other ) const {
      int n=UIDSIZE-1;
      while( n >= 0 ) {        // sweep is big endian
         if( bytes.digits[n] != other.bytes.digits[n] ) {
           if( bytes.digits[n] > other.bytes.digits[n] ) return false;
           return true;
         }
         --n;
      }
      return false;    // all digits are equal
   }
};

typedef edgeid_u edgeid_t;


// a "hash table" map of key-value pairs for edges
typedef std::map< edgeid_u, void* > edgeid_map_t;
typedef std::map< edgeid_u, void* > :: const_iterator edgeid_map_it;
typedef std::map< void*, edgeid_u > edgeptr_map_t;
typedef std::map< void*, edgeid_u > :: const_iterator edgeptr_map_it;

typedef std::map< long, void* > uidmap_t;
typedef std::map< long, void* > :: const_iterator uidmap_it;
typedef std::map< void*, long > ptrmap_t;
typedef std::map< void*, long > :: const_iterator ptrmap_it;


//----------------------------------------------------------------------------

//
// Forward declaration
//

class sMesh_uid_factory;

//
// Global variable pointing to the UID FACTORY used by object constructors
//

extern sMesh_uid_factory* global_UID_factory;


//
// The mesh node class
//

class sMesh_Node {
 public:
   sMesh_Node();
   ~sMesh_Node();

   unsigned char flags;
   // Flag bits [X000 0000]
   //            |____________ 0: primary node, 1: injected node
   double x,y,z;

   long getUID() const;
   int addRef( void * );
   int removeRef( void * );
   unsigned int getNumRefs() const;

 protected:

 private:
   long uid;
   std::list< void* > refs;
};


//
// The mesh edge class
//

class sMesh_Edge {
 public:
   sMesh_Edge( sMesh_Node * np1_, sMesh_Node * np2_ );
   ~sMesh_Edge();

   unsigned char flags;
   // Flag bits [XX00 00XX]
   //            ||     ||____ 1: "right" side element exists, 0: does not exist
   //            ||     !_____ 1: "left" side element exists,  0: does not exist
   //            ||___________ 0: is "rear" child, 1: is "forward" child (subd.)
   //            |____________ 0: primary edge, 1: injected (by subdivision)

   long getUID() const;
   int addRef( void * );
   int removeRef( void * );
   unsigned int getNumRefs() const;
   sMesh_Node* getNodePtr( int which_one ) const;
   int isBoundary() const;
   int computeLength();
   double getLength() const;
   sMesh_Edge* getChildPtr( int which_one ) const;
   int isSplit() const;
   int split( uidmap_t* node_uid, ptrmap_t* node_ptr,
              uidmap_t* edge_uid, ptrmap_t* edge_ptr );

 protected:

 private:
   long uid;
   sMesh_Node *np1, *np2;
   std::list< void* > refs;
   double length=0.0;
   sMesh_Edge *cp1=NULL, *cp2=NULL;   // edge subdivision children
};


//
// A token class to bypass compile-time issues with class-method friend-ing
//

class ChildSetToken {
 private:
   ChildSetToken() { };
   friend class sMesh_Core;     // only the Core class can create this object
};



//
// The mesh quadrilateral class
//

class sMesh_Quad {
 public:
   sMesh_Quad( sMesh_Edge * ne1_, sMesh_Edge * ne2_,
               sMesh_Edge * ne3_, sMesh_Edge * ne4_, unsigned char dirs_ );
   ~sMesh_Quad();

   long getUID() const;
   unsigned char getEdgeAttr() const;
   sMesh_Edge* getEdgePtr( int which_one ) const;
   int needsSubdivision() const;

   // the specific token is needed for this method to be called
   int setChildren( int index, sMesh_Quad* p, ChildSetToken& token );
   sMesh_Quad* getChildPtr( int index ) const;

 protected:

 private:
   long uid;
   sMesh_Edge* eptr[4];
   sMesh_Quad* child[4] = { NULL, NULL, NULL, NULL };
   unsigned char eattr;
   // Attr bits [XXXX oooo] for edges
   //            |  |___ 1: direction reversed for 4th edge
   //            |______ 1: direction reversed for 1st edge
   unsigned char sattr=0x00;
   // Attr bits [oooo ooXX] for subdivision
   //                   ||_____ 1: subdivided with edges 0 & 2 split
   //                   |______ 1: subdivided with edges 1 & 3 split
};


//
// The mesh triangle class
//

class sMesh_Tri {
 public:
   sMesh_Tri( sMesh_Edge * ne1_, sMesh_Edge * ne2_,
              sMesh_Edge * ne3_, unsigned char dirs_ );
   ~sMesh_Tri();

   long getUID() const;
   unsigned char getEdgeAttr() const;
   sMesh_Edge* getEdgePtr( int which_one ) const;
   int computeHeuristics( unsigned char & attr_, double & len_max );

   // the specific token is needed for this method to be called
   int setSubdivision( unsigned char n, ChildSetToken& token );
   int setChildren( int index, void* p, ChildSetToken& token );
   void* getChildPtr( int index ) const;
   unsigned char getSubdivAttr() const { return sattr; };

 protected:

 private:
   long uid;
   sMesh_Edge* eptr[3];
   void* child[3] = { NULL, NULL, NULL };
   unsigned char eattr;
   // Attr bits [XXXo oooo] for edges
   //            |||____ 1: direction reversed for 3rd edge, 0: normal
   //            ||_____ 1: direction reversed for 2nd edge, 0: normal
   //            |______ 1: direction reversed for 1st edge, 0: normal
   unsigned char hattr=0x00;
   // Attr bits [oXXX oXXX] for heuristics
   //             |||  |||____ 1: angle opposite 3rd edge is obtuse
   //             |||  |!_____ 1: angle opposite 2nd edge is obtuse
   //             |||  !______ 1: angle opposite 1st edge is obtuse
   //             |||_________ 1: isosceles with base edge 2
   //             ||__________ 1: isosceles with base edge 1
   //             |___________ 1: isosceles with base edge 0
   unsigned char sattr=0x00;
   // Attr bits [XXXX oXXX] for triangle subdivision
   //            ||||  |||____ 1: pointers that are assigned
   //            ||||    |____ 1: implies subdivision to quads (test for this)
   //            ||||_________ N: (4-bit) subdivision convention 1,2,3,4
};


//
// The mesh core class
//

class sMesh_Core {
 public:
   sMesh_Core();
   ~sMesh_Core();

   int loadData( int nno, const node_t nodes[],
                 int nel, const face_t faces[] );
   int quadify();
#ifdef _DEBUG_
   int dumpEdges( const char filename[], int iop ) const;
#endif

 protected:

 private:
   edgeid_map_t edgeid_map;
   edgeptr_map_t edgeptr_map;

   uidmap_t node_uid_map;
   ptrmap_t node_ptr_map;
   uidmap_t edge_uid_map;
   ptrmap_t edge_ptr_map;
   uidmap_t quad_uid_map;
   ptrmap_t quad_ptr_map;
   uidmap_t tri_uid_map;
   ptrmap_t tri_ptr_map;

   std::list< sMesh_Edge* > rem_eptr;
   std::list< sMesh_Quad* > rem_qptr;
   std::list< sMesh_Tri* > rem_tptr;

   int subdivideByRule3( sMesh_Tri* p, int index );
   int subdivideByRule1( sMesh_Tri* p );
   int subdivideByRule2u( sMesh_Quad* p );
   int subdivideByRule2v( sMesh_Quad* p );
};



#ifdef __cplusplus
}
#endif
#endif
