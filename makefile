# nmake makefile
#
# Tools used:
#  Compile::Watcom Resource Compiler
#  Compile::GNU C
#  Make: nmake or GNU make
all : atccnr.exe atccnr.hlp

atccnr.exe : atccnr.obj help.obj atccnr.res atccnr.def
	gcc -Zomf atccnr.obj help.obj atccnr.res atccnr.def -o atccnr.exe
	wrc atccnr.res

atccnr.obj : atccnr.c atccnr.h
	gcc -Wall -Zomf -DOS2EMX_PLAIN_CHAR -c -O2 atccnr.c -o atccnr.obj

help.obj : help.c help.h
	gcc -Wall -Zomf -c -O2 help.c -o help.obj

atccnr.res : atccnr.rc jet.ico
	wrc -r atccnr.rc

atccnr.hlp: atccnr.ipf
	wipfc atccnr.ipf

clean :
	rm -rf *exe *RES *obj *HLP
