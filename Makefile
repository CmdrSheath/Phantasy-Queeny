# Phantasy Queeny - Sega Genesis RPG
# Makefile for SGDK

GDK = $(GDK)

SRC = src/main.c src/game.c src/overworld.c src/combat.c src/dungeon.c src/ui.c
RES = res/resources.res

.PHONY: all clean

all: 
	$(MAKE) -f $(GDK)/makefile.gen

clean:
	$(MAKE) -f $(GDK)/makefile.gen clean

