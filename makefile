.SUFFIXES: .c .rc

ALL: atccnr.RES \
     atccnr.exe atccnr.hlp

atccnr.rc: atccnr.h jet.ico

atccnr.hlp: atccnr.ipf
  ipfc $*.ipf

atccnr.res: atccnr.rc


atccnr.exe:  \
  atccnr.res \
  atccnr.obj \
  help.obj \
  MAKEFILE
   @REM @<<atccnr.@0
     $(LINKOPTS) +
     atccnr.OBJ +
     help.OBJ
     atccnr.exe
     
     k:\toolkt21\os2lib\os2386.lib +
     k:\ibmcpp\lib\dde4mbs.lib
     ;
<<
   LINK386.EXE @atccnr.@0
   rc atccnr.res atccnr.exe

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE  .\$*.c
