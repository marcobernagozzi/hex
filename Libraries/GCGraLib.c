/**
 * file GCGraLib.c
 * by Giulio Casciola 2008
 */
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_video.h>
#include "GCGraLib.h"
#include "GCVec2dLib.h"

/************************************************************************
GC_DrawText: funzione di disegno di testo;
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2) puntatore alla fonte da usare;
-(arg.3-6) quadrupla RGBA del colore per il testo;
-(arg.7-10) quadrupla RGBA del colore di sfondo del testo;
-(arg.11) stringa di testo da disegnare;
-(arg.12-13) posizione (ascissa e ordinata) schermo in cui mettere
             la stringa (vertice alto sinistro della matrice di pixel);
-(arg.14) qualita' di rendering della fonte: "solid", "shaded", "blended";
***************************************************************************/
void GC_DrawText(SDL_Surface *s, TTF_Font *fonttodraw, char fgR, char fgG, char fgB, char fgA,
              char bgR, char bgG, char bgB, char bgA, char text[], int x, int y,
              enum textquality quality)
{
SDL_Color tmpfontcolor = {fgR,fgG,fgB,fgA};
SDL_Color tmpfontbgcolor = {bgR, bgG, bgB, bgA};
SDL_Rect dstrect;
SDL_Surface *resulting_text;

  resulting_text=NULL;
  if (quality == solid)
    resulting_text = TTF_RenderText_Solid(fonttodraw, text, tmpfontcolor);
  else
    if (quality == shaded)
       resulting_text = TTF_RenderText_Shaded(fonttodraw, text, tmpfontcolor, tmpfontbgcolor);
    else
      if (quality == blended)
         resulting_text = TTF_RenderText_Blended(fonttodraw, text, tmpfontcolor);

  if ( resulting_text != NULL ) 
  {
    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = resulting_text->w;
    dstrect.h = resulting_text->h;
    SDL_BlitSurface(resulting_text, NULL, s, &dstrect);
  }
  SDL_FreeSurface(resulting_text);
}

/**********************************************************************
GC_DrawRect: funzione di disegno di un rettangolo;
Input: 
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2-3) coordiante vertice alto sinistro del rettangolo;
-(arg.4-5) ampiezze del rettangolo;
-(arg.6) colore di disegno.
**********************************************************************/
void GC_DrawRect(SDL_Surface *s,int ax,int ay,int width,int height, Uint32 c)
{
  SDL_LockSurface(s);  
//  GC_DrawLine(s,ax,ay,ax+width,ay,c);
//  GC_DrawLine(s,ax+width,ay,ax+width,ay+height,c);
//  GC_DrawLine(s,ax+width,ay+height,ax,ay+height,c);
//  GC_DrawLine(s,ax,ay+height,ax,ay,c);
    GC_HorizLine(s, ax, ax+width, ay, c);
    GC_HorizLine(s, ax, ax+width, ay+height, c);
    GC_VerticLine(s, ax, ay, ay+height, c);
    GC_VerticLine(s, ax+width, ay, ay+height, c);
  SDL_UnlockSurface(s);
}

/*******************************************************************************
GC_LoadImage: funzione per caricare unimmagine in formato PPM, PNG o JPG da file;
tale immagine verra' memorizzata in una struttura di tipo SDL_Surface allocata
dalla funzione stessa.
Input: 
-(arg.1) puntatore al file immagine da leggere;
-(arg.2) esito del caricamento (0 fallito, 1 riuscito)
Output:
valore di ritorno: puntatore alla struttura SDL_Surface allocata per contenere 
l'immagine.
*******************************************************************************/
SDL_Surface* GC_LoadImage(char *file, int *exitstate)
{
SDL_Surface *tmp;

 tmp = IMG_Load(file);
 *exitstate = 0;
 if (tmp == NULL)
 {
   fprintf(stderr, "Error: '%s' could not be opened: %s\n", file, IMG_GetError());
 }
 else
 {
//   if(SDL_SetColorKey(tmp, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(tmp->format, 255, 0, 255)) == -1)
//     fprintf(stderr, "Warning: colorkey will not be used, reason: %s\n", SDL_GetError());
   *exitstate = 1;
 }
 return tmp;
}

/********************************************************************************
GC_DrawImage: funzione di disegno di una immagine;
Input: 
-(arg.1) puntatore alla struttura immagine che si vuole disegnare;
-(arg.2-3) coordiante vertice alto sinistro dell'immagine che si vuole disegnare;
-(arg.4-5) ampiezze del rettangolo immagine che si vuole disegnare;
-(arg.6) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.7-8) coordiante vertice alto sinistro del rettangolo finestra in cui si 
vuole disegnare;
*********************************************************************************/
void GC_DrawImage(SDL_Surface *srcimg, int sx, int sy, int sw, int sh, SDL_Surface *dstimg,
               int dx, int dy)
{
SDL_Rect src, dst;

  if (srcimg == NULL)
    return; //If theres no image, or its 100% transparent.

  src.x = sx;  src.y = sy;  src.w = sw;  src.h = sh;
  dst.x = dx;  dst.y = dy;  dst.w = src.w;  dst.h = src.h;
  SDL_BlitSurface(srcimg, &src, dstimg, &dst);
}

static int iabs(int i) { if(i<0) return(-i); else return(i); }

static inline int incdir(int a,int b)
{
  if(a>b) return(-1);
  else    return(1);
}

static inline void swap(int *a, int *b)
{
  int tmp=*a;
  *a=*b;
  *b=tmp;
}

/***********************************************************************
GC_GetPixelImage: funzione di lettura di un pixel di una immagine;
 * Return the pixel value at (x, y); useful for PPM, JPG and PNG formats
 * that use BIG-ENDIAN!!!
 * NOTE: The surface/image must be locked before calling this!
Input: 
-(arg.1) puntatore alla struttura immagine a cui si vuole accedere;
-(arg.2-3) coordiante del pixel che si vuole leggere;
Output:
valore di ritorno: colore del pixel.
************************************************************************/
Uint32 GC_GetPixelImage(SDL_Surface *image, int x, int y)
{
int bpp = image->format->BytesPerPixel;
/* Here p is the address to the pixel we want to retrieve */
Uint8 *p = (Uint8 *)image->pixels + y * image->pitch + x * bpp;

  switch(bpp) {
  case 1:
    return *p;
  case 2:
    return *(Uint16 *)p;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[2] << 16 | p[1] << 8 | p[0];
    else
      return p[2] | p[1] << 8 | p[0] << 16;
  case 4:
    return *(Uint32 *)p;
  default:
    return 0; /* shouldn't happen, but avoids warnings */
  }
}

/********************************************************************
GC_GetPixel: funzione di lettura di un pixel di una finestra grafica;
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
Input: 
-(arg.1) puntatore alla struttura finestra a cui si vuole accedere;
-(arg.2-3) coordiante del pixel che si vuole leggere;
Output:
valore di ritorno: colore del pixel.
********************************************************************/
Uint32 GC_GetPixel(SDL_Surface *surface, int x, int y)
{
int bpp = surface->format->BytesPerPixel;
/* Here p is the address to the pixel we want to retrieve */
Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp) {
  case 1:
    return *p;
  case 2:
    return *(Uint16 *)p;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
//original
      return p[0] | p[1] << 8 | p[2] << 16;
//new
//      printf("getpixel %d %d %d\n",p[2],p[1],p[0]);
      return p[2] | p[1] << 8 | p[0] << 16;
  case 4:
    return *(Uint32 *)p;
  default:
    return 0; /* shouldn't happen, but avoids warnings */
  }
}

/***********************************************************************
GC_PutPixel: funzione di scrittura di un pixel di una finestra grafica;
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
Input: 
-(arg.1) puntatore alla struttura finestra a cui si vuole accedere;
-(arg.2-3) coordiante del pixel a cui si vuole accedere;
-(arg.4) colore che si vuole scrivere.
************************************************************************/
void GC_PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    if((x>surface->w)||(y>surface->h)||(x<0)||(y<0)) return;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
//versione originale
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
//printf("%ld\n",pixel);
//printf("%d %d %d\n",p[2],p[1],p[0]);
//versione modificata
//            p[2] = pixel & 0xff;
//            p[1] = (pixel >> 8) & 0xff;
//            p[0] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

/*********************************************************************
GC_HorizLine: funzione di disegno di una linea orizzontale;
 * Quickly draws horizontal lines, taking advantage of the fact that
 * characters are grouped by horizontal line in image[][].  After 
 * checking for clipping it can simply fill x2-x characters in a row 
 * with cur_color by calling memset.
Input: 
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2-3) ascisse degli estremi della linea;
-(arg.4) ordinata della linea;
-(arg.5) colore di disegno.
***********************************************************************/
void GC_HorizLine(SDL_Surface *s, int x, int x2, int y, Uint32 color)
{
  SDL_Rect r;

  SDL_LockSurface(s);
  r.y=y;
  r.h=1;

  if(x2<x)
  {
    r.x=x2;
    r.w=x-x2;
  }
  else
  {
    r.x=x;
    r.w=x2-x;
  }
  SDL_FillRect(s,&r,color);
  SDL_UnlockSurface(s);
}

/*********************************************************************
GC_VerticLine: funzione di disegno di una linea verticale;
 * Quickly draws vertical lines.  After 
 * checking for clipping it can simply fill y2-y characters in a column 
 * with cur_color by calling memset.
Input: 
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2) ascissa della linea;
-(arg.3-4) ordinate degli estremi della linea;
-(arg.5) colore di disegno.
***********************************************************************/
void GC_VerticLine(SDL_Surface *s, int x, int y, int y2, Uint32 color)
{
  SDL_Rect r;

  SDL_LockSurface(s);
  r.x=x;
  r.w=1;

  if(y2<y)
  {
    r.y=y2;
    r.h=y-y2;
  }
  else
  {
    r.y=y;
    r.h=y2-y;
  }
  SDL_FillRect(s,&r,color);
  SDL_UnlockSurface(s);
}

/*******************************************************************
GC_DrawCircle: funzione di disegno di una circonferenza;
* Converts one 1/8 of an arc into a full circle. *
Input: 
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2-3) coordinate centro della circonferenza;
-(arg.4) raggio della circonferenza;
-(arg.5) colore di disegno.
********************************************************************/
void GC_DrawCircle(SDL_Surface *s, int x1, int y1, int ir, Uint32 color) 
{
int x,y,d;

  SDL_LockSurface(s);
  x = ir;
  y = 0;
  d = 3 - 2*ir;
  while (y < x )
   {
    GC_PutPixel(s,x+x1,y+y1,color);
    GC_PutPixel(s,y+x1,x+y1,color);
    GC_PutPixel(s,-y+x1,x+y1,color);
    GC_PutPixel(s,-x+x1,y+y1,color);
    GC_PutPixel(s,-x+x1,-y+y1,color);
    GC_PutPixel(s,-y+x1,-x+y1,color);
    GC_PutPixel(s,y+x1,-x+y1,color);
    GC_PutPixel(s,x+x1,-y+y1,color);
    if (d < 0)
      d = d + 4*y + 6;
    else
     {
      d = d+4*(y-x)+10;
      x--;
     }
    y++;
  }
  if (x==y)
    {
     GC_PutPixel(s,x+x1,y+y1,color);
     GC_PutPixel(s,-x+x1,y+y1,color);
     GC_PutPixel(s,-x+x1,-y+y1,color);
     GC_PutPixel(s,x+x1,-y+y1,color);
    }
  SDL_UnlockSurface(s);
}

/********************************************************************
GC_FillCircle: funzione di disegno di un cerchio pieno;
 * Calculates circle points with the integer midpoint algorithm.
 *
 * There are 8 reflections of the generated arc, 4 on the left
 * and for on the right.  Horizontal lines are drawn between
 * reflections of the same row to fill it.  Horizontal lines
 * are used because they are a special case that draws extremely
 * quickly.
 *
 * Overdraw is reduced by considering that the top-most and bottom-most 
 * reflections only need to be filled  when the row value changes;
 * when the column value alone changes, it just makes the line wider by
 * two pixels.
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2-3) coordinate centro del cerchio;
-(arg.4) raggio del cerchio;
-(arg.5) colore di disegno.
***********************************************************************/
void GC_FillCircle(SDL_Surface *s, int xin, int yin, int rad, Uint32 color)
{
  int x=0,y=rad,d=1-rad,deltaE=3,deltaSE=5-(rad*2);

  if(rad<0) return; // sanity checking

  SDL_LockSurface(s);
  GC_HorizLine(s,xin-y,xin+y,yin,color); // Center line

  while(y>x)
  {
    if(d<0)
    {
      d+=deltaE;
      deltaE+=2;
      deltaSE+=2;
    }
    else
    {
      // Only need to draw these lines when y changes
      GC_HorizLine(s,xin-x,xin+x,yin+y,color); // Bottom-most reflections
      GC_HorizLine(s,xin-x,xin+x,yin-y,color); // Top-most reflections
      d+=deltaSE;
      deltaE+=2;
      deltaSE+=4;
      y--;
    }

    x++;
    // These lines change y every time x increments.
    GC_HorizLine(s,xin-y,xin+y,yin-x,color); // Upper middle reflections
    GC_HorizLine(s,xin-y,xin+y,yin+x,color); // Lower middle reflections
  }
  SDL_UnlockSurface(s);
}

/***********************************************************************
GC_DrawLine: Funzione di disegno di una linea;
*Algoritmo di Linea di Bresenham*
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;;
-(arg.2-3) coordinate primo estremo del linea;
-(arg.4-5) coordinate secondo estremo della linea;
-(arg.6) colore di disegno.
************************************************************************/
void GC_DrawLine(SDL_Surface *s, int x0, int y0, int x1, int y1, Uint32 color)
{
  // absolute values of dx and dy, so as to not screw up calculation
  int dx=iabs(x1-x0),dy=iabs(y1-y0),x=x0,y=y0;
  // When true, the loop will iterate through y instead of x
  int reverse=dy>dx;
  // These record which direction the line should go
  int xdir=incdir(x0,x1),ydir=incdir(y0,y1);
  int d,incrE,incrNE;

  SDL_LockSurface(s);
  // Swap dx and dy if reversed, so as to not fubar equation
  if(reverse) swap(&dy,&dx);

  // Initialize.  If 
  d=(dy*2)-dx;
  incrE=dy*2;
  incrNE=(dy-dx)*2;

  // Draw first pixel
  GC_PutPixel(s,x,y,color);
  if(reverse)
    while(y!=y1) // Iterate through y
    {
      y+=ydir;
      if(d<=0)
        d+=incrE;
      else
      {
        x+=xdir;
        d+=incrNE;
      }
      // Draw next pixel
      GC_PutPixel(s,x,y,color);
    }
  else
    while(x!=x1) // Iterate through x
    {
      x+=xdir;
      if(d<=0)
        d+=incrE;
      else
      {
        y+=ydir;
        d+=incrNE;
      }
      // Draw pixel
      GC_PutPixel(s,x,y,color);
    }
  SDL_UnlockSurface(s);
}
/* end of draw_line.c */

int trovacolore(int colore1,int colore2, int colore3, int colore4, float x, float y){
	int colore=colore1*(1-x)*(1-y)+colore2*x*(1-y)+colore3*(1-x)*y+colore4*x*y;
	return colore;
}

void conv(Uint32 m,int b,int a[3]){
	int p;
	a[0]=a[1]=a[2]=1;
	p=-1;
	while(m!=0){
		p++;
		a[p]=m%b;
		m=m/b;
	}
}


void scala_immagine(SDL_Surface *sr,int finestra, char *c){
	int i,j;
	int w,h,color,res;
	Uint32 pixelvalue1,pixelvalue2,pixelvalue3,pixelvalue4;
	SDL_Surface *tux;
	//carica un'immagine dal file
	tux = GC_LoadImage(c,&res);
	if (res == 0){
		printf("impossibile caricare l'immagine di sfondo");
		return;
	}
	w=tux->w;
	h=tux->h;
	int dimensionex=finestra;
	int dimensioney=finestra;
	Point2 punto_reale;	
	SDL_LockSurface(tux);
	for (i=0;i<dimensionex;i++)
		for (j=0;j<dimensioney;j++)
		{
			punto_reale.x=i*tux->w/dimensionex;
			punto_reale.y=j*tux->h/dimensioney;
			pixelvalue1=GC_GetPixelImage(tux,(int)(punto_reale.x),(int)(punto_reale.y));
			pixelvalue2=GC_GetPixelImage(tux,(int)(punto_reale.x+1),(int)(punto_reale.y));
			pixelvalue3=GC_GetPixelImage(tux,(int)(punto_reale.x),(int)(punto_reale.y+1));
			pixelvalue4=GC_GetPixelImage(tux,(int)(punto_reale.x+1),(int)(punto_reale.y+1));
			
			double xx,yy;
			xx=punto_reale.x-(int)(punto_reale.x);
			yy=punto_reale.y-(int)(punto_reale.y);
			int a[3], b[3],c[3],d[3];
			conv(pixelvalue1,256,a);
			conv(pixelvalue2,256,b);
			conv(pixelvalue3,256,c);
			conv(pixelvalue4,256,d);
			
			int red1,green1,blue1;
			red1=  trovacolore(a[2], b[2], c[2], d[2],xx,yy);
			green1=trovacolore(a[1], b[1], c[1], d[1],xx,yy);
			blue1= trovacolore(a[0], b[0], c[0], d[0],xx,yy);
			
			color=SDL_MapRGB(sr->format,red1,green1,blue1);
			GC_PutPixel(sr,i,j,color);
		}
	SDL_UnlockSurface(tux);
	SDL_FreeSurface(tux);
}