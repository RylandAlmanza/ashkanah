CC=gcc
LIBS=-lncurses
DEPS=colorutils.h components.h directions.h point.h world.h

main: $(DEPS) colorutils.c main.c world.c
	$(CC) colorutils.c main.c world.c -o main $(LIBS) -g

mapeditor: colorutils.h directions.h point.h tilepreview.h colorutils.c tilepreview.c mapeditor.c
	$(CC) colorutils.c tilepreview.c mapeditor.c -o mapeditor $(LIBS) -g
