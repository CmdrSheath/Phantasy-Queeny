# Phantasy Queeny - Sega Genesis RPG
# Makefile for SGDK

GDK = $(GDK)
SRC = src/main.c src/game.c src/title.c src/cutscene.c src/overworld.c src/combat.c src/dungeon.c src/ui.c src/pause_menu.c

RES = res/resources.res

.PHONY: all clean

all: 
	$(MAKE) -f $(GDK)/makefile.gen

clean:
	$(MAKE) -f $(GDK)/makefile.gen clean

RESOURCES += res/resources.res

# Audio processing flags
WAVCONVERT_FLAGS = -rate 22050 -format pcm8
