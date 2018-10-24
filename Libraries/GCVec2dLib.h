/* GCVec2dLib.h  by Andrew Glassner
 * modified by Giulio Casciola (2009) */

#ifndef GC_H
#define GC_H 1

/*********************/
/* 2d geometry types */
/*********************/

/* 2d point */
typedef struct Point2Struct 
{
  double x, y;
}Point2;
typedef Point2 Vector2;

/* 2d integer point */
typedef struct IntPoint2Struct 
{
  int x, y;
}IntPoint2;

/* 3-by-3 matrix */
typedef struct Matrix3Struct 
{
  double element[3][3];
}Matrix3;

/* 2d box */
typedef struct Box2dStruct 
{
  Point2 min, max;
}Box2;

typedef struct{
	float xmin, xmax, ymin, ymax;
} RECT;

typedef RECT VIEWPORT;
typedef RECT WINDOW;

/***********************/
/* one-argument macros */
/***********************/

/* absolute value of a */
#define ABS(a)		(((a)<0) ? -(a) : (a))

/* round a to nearest int */
#define ROUND(a)	((a)>0 ? (int)((a)+0.5) : -(int)(0.5-(a)))

/* take sign of a, either -1, 0, or 1 */
#define ZSGN(a)		(((a)<0) ? -1 : (a)>0 ? 1 : 0)	

/* take binary sign of a, either -1, or 1 if >= 0 */
#define SGN(a)		(((a)<0) ? -1 : 1)

/* shout if something that should be true isn't */
#define ASSERT(x) \
if (!(x)) fprintf(stderr," Assert failed: x\n");

/* square a */
#define SQR(a)		((a)*(a))	

/***********************/
/* two-argument macros */
/***********************/

/* find minimum of a and b */
#define MIN(a,b)	(((a)<(b))?(a):(b))	

/* find maximum of a and b */
#define MAX(a,b)	(((a)>(b))?(a):(b))	

/* swap a and b; run for integral operands only */
#define SWAP(a,b)	{ a^=b; b^=a; a^=b; }

/* linear interpolation from l (when a=0) to h (when a=1)*/
/* (equal to (a*h)+((1-a)*l) */
#define LERP(a,l,h)	((l)+(((h)-(l))*(a)))

/* clamp the input to the specified range */
#define CLAMP(v,l,h)	((v)<(l) ? (l) : (v) > (h) ? (h) : v)

/****************************/
/* memory allocation macros */
/****************************/

/* create a new instance of a structure */
#define NEWSTRUCT(x)	(struct x *)(malloc((unsigned)sizeof(struct x)))

/* create a new instance of a type */
#define NEWTYPE(x)	(x *)(malloc((unsigned)sizeof(x)))

/* create a new instance of an array of structure */
#define ARR_NEWSTRUCT(n,x)	(struct x *)(malloc(n*(unsigned)sizeof(struct x)))

/* create a new instance of an array of type */
#define ARR_NEWTYPE(n,x)	(x *)(malloc(n*(unsigned)sizeof(x)))

/********************/
/* useful constants */
/********************/

#define PI		3.141592	/* the venerable pi */
#define PITIMES2	6.283185	/* 2 * pi */
#define PIOVER2		1.570796	/* pi / 2 */
#define E		2.718282	/* the venerable e */
#define SQRT2		1.414214	/* sqrt(2) */
#define SQRT3		1.732051	/* sqrt(3) */
#define GOLDEN		1.618034	/* the golden ratio */
#define DTOR		0.017453	/* convert degrees to radians */
#define RTOD		57.29578	/* convert radians to degrees */
#define SMALL_NUMBER    1.e-8           /* small number*/

/************/
/* booleans */
/************/

#define TRUE		1
#define FALSE		0
#define ON		1
#define OFF 		0
typedef int boolean;			/* boolean data type */
typedef boolean flag;			/* flag data type */

/*********************************/
/* extern functions declarations */
/*********************************/

extern double V2SquaredLength(Vector2 *);
extern double V2Length(Vector2 *);
extern double V2Dot(Vector2 *,Vector2 *); 
extern double V2DistanceBetween2Points(Point2 *, Point2 *); 
extern Vector2 *V2Negate(Vector2 *);
extern Vector2 *V2Normalize(Vector2 *);
extern Vector2 *V2Scale(Vector2 *, double);
extern Vector2 *V2Add(Vector2 *,Vector2 *,Vector2 *);
extern Vector2 *V2Sub(Vector2 *,Vector2 *,Vector2 *);
extern Vector2 *V2Lerp(Vector2 *, Vector2 *, double, Vector2 *);
extern Vector2 *V2Combine(Vector2 *, Vector2 *, Vector2 *, double , double );
extern Vector2 *V2Mul(Vector2 *, Vector2 *, Vector2 *);
extern Vector2 *V2MakePerpendicular(Vector2 *, Vector2 *);
extern Vector2 *V2New(double , double );
extern Vector2 *V2Duplicate(Vector2 *);
extern void V2Swap(Point2 *, Point2 *);
extern Point2 *V2MulPointByMatrix(Point2 *, Matrix3 *);
extern Matrix3 *V2MatMul(Matrix3 *, Matrix3 *, Matrix3 *);
extern double RegulaFalsi(double (*)(), double , double );
extern double NewtonRaphson(double (*)(), double (*)(), double );
extern double findroot(double ,double ,double , double (*)(), double (*)());
extern double det2x2( double , double , double , double );
extern double det3x3(Matrix3 *);
extern void adjoint(Matrix3 *, Matrix3 *);
extern int inverse(Matrix3 *, Matrix3 *t);
extern Matrix3 *MatMul(Matrix3 *a, Matrix3 *b, Matrix3 *c);
extern double distanzapunti(IntPoint2 *a, IntPoint2 *b);
extern void get_scale (RECT r1, RECT r2, float *scx, float *scy);
extern void wind_view (float px, float py, int *ix, int *iy, VIEWPORT view, WINDOW win);
extern Point2 ruota_punto(Point2 point, Point2 centro, float angolo);
extern double min(double,double);
extern double max(double,double);
#endif
