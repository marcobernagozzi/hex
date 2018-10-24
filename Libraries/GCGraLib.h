/*
 * file GCGraLib.h
 * by Giulio Casciola 2008
 */
#ifndef __GCGraLib_H__
#define __GCGraLib_H__
struct{
	Uint32 red;
	Uint32 green;
	Uint32 rectangle;
	Uint32 black;
	Uint32 white;
	Uint32 rosso;
}colori;

enum textquality {solid, shaded, blended};

extern void GC_DrawText(SDL_Surface *, TTF_Font *, char , char , char , char ,
              char , char , char , char , char* , int , int , enum textquality );

extern void GC_DrawRect(SDL_Surface * ,int ,int ,int ,int , Uint32 );

extern SDL_Surface* GC_LoadImage(char *file, int *exitstate);

extern void GC_DrawImage(SDL_Surface *srcimg, int sx, int sy, int sw, int sh, 
                       SDL_Surface *dstimg, int dx, int dy);

extern Uint32 GC_GetPixelImage(SDL_Surface *image, int x, int y);

extern Uint32 GC_GetPixel(SDL_Surface *surface, int x, int y);

extern void GC_PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

extern void GC_HorizLine(SDL_Surface *s, int x, int x2, int y, Uint32 color);

extern void GC_VerticLine(SDL_Surface *s, int x, int y, int y2, Uint32 color);

extern void GC_DrawCircle(SDL_Surface *s, int x1, int y1, int ir, Uint32 color);

extern void GC_FillCircle(SDL_Surface *s, int xin, int yin, int rad, Uint32 color);

extern void GC_DrawLine(SDL_Surface *s, int x0, int y0, int x1, int y1, Uint32 color);

extern int trovacolore(int colore1,int colore2, int colore3, int colore4, float x, float y);

extern void conv(Uint32 m,int b,int a[3]);

extern void scala_immagine(SDL_Surface *screen,int finestra, char *);

#endif/*__GCGraLib_H__*/
