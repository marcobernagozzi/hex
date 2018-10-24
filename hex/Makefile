
LDIR=../Libraries/
LDIR2=../ai/

# Library linking 
LDLIB = -L/usr/X11R6/lib -lX11 -lSDL -lSDL_ttf -lSDL_image -lm

# ---- Files --- 
SRC1= hex.c $(LDIR)GCVec2dLib.c $(LDIR)GCVec2dLib.h $(LDIR)GCGraLib.c $(LDIR)GCGraLib.h $(LDIR)graph_code.c $(LDIR)graph_code.h $(LDIR2)y_reduction.c $(LDIR2)y_reduction.h $(LDIR2)min_max.c $(LDIR2)min_max.h

OBJ1= hex.o GCVec2dLib.o GCGraLib.o graph_code.o y_reduction.o min_max.o

hex:$(OBJ1)
	gcc $(OBJ1) $(LDLIB) -o $@

y_reduction.o: $(LDIR2)y_reduction.c $(LDIR2)y_reduction.h  $(LDIR)GCVec2dLib.c $(LDIR)GCVec2dLib.h  $(LDIR2)min_max.c $(LDIR2)min_max.h 
	gcc -c -Wall  $(LDIR2)y_reduction.c -o $@

min_max.o: $(LDIR2)min_max.c $(LDIR2)min_max.h  $(LDIR)GCVec2dLib.c $(LDIR)GCVec2dLib.h
	gcc -c -Wall  $(LDIR2)min_max.c -o $@


GCGraLib.o: $(LDIR)GCGraLib.c $(LDIR)GCGraLib.h  $(LDIR)GCVec2dLib.c $(LDIR)GCVec2dLib.h
	gcc -c -Wall $(LDIR)GCGraLib.c -o $@

GCVec2dLib.o: $(LDIR)GCVec2dLib.c $(LDIR)GCVec2dLib.h
	gcc -c -Wall  $(LDIR)GCVec2dLib.c -o $@

graph_code.o: $(LDIR)graph_code.c $(LDIR)graph_code.h
	gcc -c -Wall  $(LDIR)graph_code.c -o $@



hex.o:$(SRC1)
	gcc -DDEBUG -c -Wall hex.c -o $@

.c.o:
	gcc  -c -Wall *.c

clean:
	rm -f *.o *.*~ *~ $(LDIR)*.o $(LDIR)*.*~ $(LDIR)*~ $(LDIR2)*.o $(LDIR2)*.*~ $(LDIR2)*~ hex
