
typedef struct {
   long int id;
   double x,y,z;
} vertex_t;

typedef struct triangle_s triangle_t;     // forward declaration of triangle

typedef struct {
   long int id;
   vertex_t *va, *vb;
   triangle_t *tl, *tr;
} edge_t;

struct triangle_s {
   long int id;
   edge_t *e1, *e2, *e3;
   char d1,d2,d3,foo;
};

typedef struct {
   long int nv,ne,nt;
   vertex_t *v;
   edge_t *e;
   triangle_t *t;
} mesh_t;


struct ingeom_tris_s {
   int np,nt;
   int *icon;
   double *x;
};

struct ingeom_sphere_s {
   int np,nt;
   int *icon;
   double *x;
   int ns;
   double x0[3],x1[3],x2[3],x3[3];
};

// -------------------- function prototypes/signatures --------------------

int incg_MakeMesh_OneTriangle( mesh_t* m );

int incg_MakeMesh_TwoTriangles( mesh_t* m );

int incg_MakeMesh_Cube( mesh_t* m );

int incg_RefineMesh_Uniform( mesh_t* m );

