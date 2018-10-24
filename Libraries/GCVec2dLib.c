/* 2d Vector C Library 
by Andrew Glassner
modified by Giulio Casciola (2009) */

#include <math.h>
#include <stdlib.h>
#include "GCVec2dLib.h"

/******************/
/*   2d Library   */
/******************/

/* returns squared length of input vector */	
double V2SquaredLength(Vector2 *a) 
{
//  return((a->x * a->x)+(a->y * a->y));
  return(SQR(a->x) + SQR(a->y));
}
	
/* returns length of input vector */
double V2Length(Vector2 *a) 
{
  return(sqrt(V2SquaredLength(a)));
}
	
/* negates the input vector and returns it */
Vector2 *V2Negate(Vector2 *v) 
{
  v->x = -v->x;  
  v->y = -v->y;
  return(v);
}

/* normalizes the input vector and returns it */
Vector2 *V2Normalize(Vector2 *v) 
{
double len = V2Length(v);

  if (len != 0.0) {
    v->x /= len;  
    v->y /= len; 
  }
  return(v);
}

/* scales the input vector to the new length and returns it */
Vector2 *V2Scale(Vector2 *v, double newlen) 
{
double len = V2Length(v);

  if (len != 0.0) { 
    v->x *= newlen/len;   
    v->y *= newlen/len; 
  }
  return(v);
}

/* return vector sum c = a+b */
Vector2 *V2Add(Vector2 *a, Vector2 *b, Vector2 *c)
{
  c->x = a->x + b->x;  
  c->y = a->y + b->y;
  return(c);
}
	
/* return vector difference c = a-b */
Vector2 *V2Sub(Vector2 *a, Vector2 *b, Vector2 *c)
{
  c->x = a->x - b->x;  
  c->y = a->y - b->y;
  return(c);
}

/* return the dot product of vectors a and b */
double V2Dot(Vector2 *a, Vector2 *b) 
{
  return((a->x * b->x) + (a->y * b->y));
}

/* linearly interpolate between vectors by an amount alpha */
/* and return the resulting vector. */
/* When alpha=0, result=lo.  When alpha=1, result=hi. */
Vector2 *V2Lerp(Vector2 *lo, Vector2 *hi, double alpha, Vector2 *result) 
{
  result->x = LERP(alpha, lo->x, hi->x);
  result->y = LERP(alpha, lo->y, hi->y);
  return(result);
}

/* make a linear combination of two vectors and return the result. */
/* result = (a * ascl) + (b * bscl) */
Vector2 *V2Combine (Vector2 *a, Vector2 *b, Vector2 *result, double ascl, double bscl) 
{
  result->x = (ascl * a->x) + (bscl * b->x);
  result->y = (ascl * a->y) + (bscl * b->y);
  return(result);
}

/* multiply two vectors together component-wise */
Vector2 *V2Mul (Vector2 *a, Vector2 *b, Vector2 *result) 
{
  result->x = a->x * b->x;
  result->y = a->y * b->y;
  return(result);
}

/* return the distance between two points */
double V2DistanceBetween2Points(Point2 *a, Point2 *b)
{
double dx = a->x - b->x;
double dy = a->y - b->y;

  return(sqrt((dx*dx)+(dy*dy)));
}

/* return the vector perpendicular to the input vector a */
Vector2 *V2MakePerpendicular(Vector2 *a, Vector2 *ap)
{
  ap->x = -a->y;
  ap->y = a->x;
  return(ap);
}

/* create, initialize, and return a new vector */
Vector2 *V2New(double x, double y)
{
Vector2 *v = NEWTYPE(Vector2);

  v->x = x;  
  v->y = y; 
  return(v);
}
	
/* create, initialize, and return a duplicate vector */
Vector2 *V2Duplicate(Vector2 *a)
{
Vector2 *v = NEWTYPE(Vector2);

  v->x = a->x;  
  v->y = a->y; 
  return(v);
}

/*swap two points*/
void V2Swap(Point2 *p1, Point2 *p2)
{
double tmp;

   tmp=p1->x;
   p1->x=p2->x;;
   p2->x=tmp;
   tmp=p1->y;
   p1->y=p2->y;;
   p2->y=tmp;
}
	
/* multiply a point by a matrix and return the transformed point */
Point2 *V2MulPointByMatrix(Point2 *p, Matrix3 *m)
{
double w;
Point2 ptmp;

  ptmp.x = (p->x * m->element[0][0]) + 
           (p->y * m->element[1][0]) + m->element[2][0];
  ptmp.y = (p->x * m->element[0][1]) + 
           (p->y * m->element[1][1]) + m->element[2][1];
  w      = (p->x * m->element[0][2]) + 
           (p->y * m->element[1][2]) + m->element[2][2];
  if (w != 0.0) { 
    ptmp.x /= w;  
    ptmp.y /= w; 
  }
  *p = ptmp;
  return(p);
}

/* multiply together 3x3 matrices c = ab */
/* note that c must not point to either of the input matrices */
Matrix3 *V2MatMul(Matrix3 *a, Matrix3 *b, Matrix3 *c)
{
int i, j, k;

  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) {
	c->element[i][j] = 0;
        for (k=0; k<3; k++) 
	  c->element[i][j] += a->element[i][k] * b->element[k][j];
    }
  }
  return(c);
}

/***********************/
/*   Useful Routines   */
/***********************/

/* return roots of ax^2+bx+c */
/* stable algebra derived from Numerical Recipes by Press et al.*/
int quadraticRoots(double a, double b, double c, double *roots)
{
double d, q;
int count = 0;

  d = (b*b)-(4*a*c);
  if (d < 0.0) { 
    *roots = *(roots+1) = 0.0;  
    return(0); 
  }
  q =  -0.5 * (b + (SGN(b)*sqrt(d)));
  if (a != 0.0) { 
    *roots++ = q/a; 
    count++; 
  }
  if (q != 0.0) { 
    *roots++ = c/q; 
    count++; 
  }
  return(count);
}

/* generic 1d regula-falsi step.  f is function to evaluate */
/* interval known to contain root is given in left, right */
/* returns new estimate */
double RegulaFalsi(double (*f)(), double left, double right)
{
double d = (*f)(right) - (*f)(left);

  if (d != 0.0) 
    return (right - (*f)(right)*(right-left)/d);
  return((left+right)/2.0);
}

/* generic 1d Newton-Raphson step. f is function, df is derivative */
/* x is current best guess for root location. Returns new estimate */
double NewtonRaphson(double (*f)(), double (*df)(), double x)
{
double d = (*df)(x);

  if (d != 0.0) 
    return (x-((*f)(x)/d));
  return(x-1.0);
}

/* hybrid 1d Newton-Raphson/Regula Falsi root finder. */
/* input function f and its derivative df, an interval */
/* left, right known to contain the root, and an error tolerance */
/* Based on Blinn */
double findroot(double left,double  right,double  tolerance, 
                double (*f)(), double (*df)())
{
double newx = left;

  while (ABS((*f)(newx)) > tolerance) {
    newx = NewtonRaphson(f, df, newx);
    if (newx < left || newx > right) 
      newx = RegulaFalsi(f, left, right);
    if ((*f)(newx) * (*f)(left) <= 0.0) 
      right = newx;  
    else 
      left = newx;
  }
  return(newx);
} 

double det2x2( double a, double b, double c, double d)
{
double ans;

    ans = a * d - b * c;
    return ans;
}

double det3x3(Matrix3 *in) 
{
double a1, a2, a3, b1, b2, b3, c1, c2,  c3;
double ans;

    a1 = in->element[0][0]; 
    b1 = in->element[0][1]; 
    c1 = in->element[0][2];

    a2 = in->element[1][0]; 
    b2 = in->element[1][1]; 
    c2 = in->element[1][2];

    a3 = in->element[2][0]; 
    b3 = in->element[2][1];
    c3 = in->element[2][2];

    ans = a1 * det2x2( b2, b3, c2, c3 )
        - b1 * det2x2( a2, a3, c2, c3 )
        + c1 * det2x2( a2, a3, b2, b3 );
    return ans;
}

void adjoint(Matrix3 *in, Matrix3 *out )
{
double a1, a2, a3, b1, b2, b3, c1, c2, c3;

    a1 = in->element[0][0]; 
    b1 = in->element[0][1]; 
    c1 = in->element[0][2];

    a2 = in->element[1][0]; 
    b2 = in->element[1][1]; 
    c2 = in->element[1][2];

    a3 = in->element[2][0]; 
    b3 = in->element[2][1];
    c3 = in->element[2][2];

    out->element[0][0]  =   det2x2( b2, b3, c2, c3);
    out->element[1][0]  = - det2x2( a2, a3, c2, c3);
    out->element[2][0]  =   det2x2( a2, a3, b2, b3);
        
    out->element[0][1]  = - det2x2( b1, b3, c1, c3);
    out->element[1][1]  =   det2x2( a1, a3, c1, c3);
    out->element[2][1]  = - det2x2( a1, a3, b1, b3);
        
    out->element[0][2]  =   det2x2( b1, b2, c1, c2);
    out->element[1][2]  = - det2x2( a1, a2, c1, c2);
    out->element[2][2]  =   det2x2( a1, a2, b1, b2);
}

int inverse(Matrix3 *in, Matrix3 *out)
{
int i, j;
double det;

    /* calculate the adjoint matrix */

    adjoint( in, out );

    /*  calculate the 3x3 determinant
     *  if the determinant is zero, 
     *  then the inverse matrix is not unique.
     */

    det = det3x3( in );

    if ( fabs( det ) < SMALL_NUMBER) {
/* La matrice non puo' essere invertita!*/
      return(0);
    }

    /* scale the adjoint matrix to get the inverse */

    for (i=0; i<3; i++)
        for(j=0; j<3; j++)
	    out->element[i][j] = out->element[i][j] / det;
   
    return(1);
}

/* multiply together matrices c = ab */
/* note that c must not point to either of the input matrices */
Matrix3 *MatMul(Matrix3 *a, Matrix3 *b, Matrix3 *c)
{
int i, j, k;

    for (i=0; i<3; i++) {
       for (j=0; j<3; j++) {
          c->element[i][j] = 0;
          for (k=0; k<3; k++) 
            c->element[i][j] += a->element[i][k] * b->element[k][j];
       }
    }
    return(c);
}


double distanzapunti(IntPoint2 *a, IntPoint2 *b){
	double dx = a->x - b->x;
	double dy = a->y - b->y;
	return(sqrt((dx*dx)+(dy*dy)));
}

void get_scale (RECT r1, RECT r2, float *scx, float *scy){
	*scx = (r1.xmax - r1.xmin) / (r2.xmax - r2.xmin);
	*scy = (r1.xmax - r1.xmin) / (r2.xmax - r2.xmin);
}

void wind_view (float px, float py, int *ix, int *iy, VIEWPORT view, WINDOW win){
	float scx, scy;
	get_scale (view, win, &scx, &scy);
	*ix = 0.5 + scx * (px - win.xmin) + view.xmin;
	*iy = 0.5 + scy * (win.xmin - py) + view.xmax;
}

Point2 ruota_punto(Point2 point, Point2 centro, float angolo){
	Point2 point_ruotato;
	point_ruotato.x = (point.x-centro.x)*cos(angolo) - (point.y-centro.y)*sin(angolo);
	point_ruotato.y = (point.x-centro.x)*sin(angolo) + (point.y-centro.y)*cos(angolo);
	point_ruotato.x += centro.x;
	point_ruotato.y += centro.y;
	return point_ruotato;
}

double min(double a,double b){
	if(a>b)
		return b;
	if(a<=b)
		return a;
	return 0;
}

double max(double a,double b){
	if(a>b)
		return a;
	if(a<=b)
		return b;
	return 0;
}