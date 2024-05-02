SRCDIR=./src
COMMONSRCDIR=../lib/src
INCDIR=../lib/include
CC=gcc
INCLUDE = -IC:/msys64/mingw64/include/SDL2 -I$(INCDIR) 
CFLAGS = -g $(INCLUDE) -c
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lm

characterServer: main.o character.o world.o bullet.o text.o
	$(CC) -o characterServer main.o character.o world.o bullet.o text.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c $(INCDIR)/netdata.h $(INCDIR)/character.h $(INCDIR)/world.h $(INCDIR)/text.h 
	$(CC) $(CFLAGS) $(SRCDIR)/main.c 

character.o: $(COMMONSRCDIR)/character.c $(INCDIR)/netdata.h $(INCDIR)/character.h $(INCDIR)/bullet.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/character.c

world.o: $(COMMONSRCDIR)/world.c $(INCDIR)/world.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/world.c

text.o: $(COMMONSRCDIR)/text.c $(INCDIR)/text.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/text.c

bullet.o: $(COMMONSRCDIR)/bullet.c $(INCDIR)/bullet.h $(INCDIR)/netdata.h
	$(CC) $(CFLAGS) $(COMMONSRCDIR)/bullet.c

clean:
	rm *.o
	rm *.exe