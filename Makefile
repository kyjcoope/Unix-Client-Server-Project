#
# Makefile for Chapter 11
#
# Type  make    to compile all the programs
# in the chapter 
#
all: tclient tserver

clean:
	rm -f tclient tserver

tclient: client.c
	cc -o tclient client.c -lcurses

tserver: server.c
	cc -o tserver server.c
