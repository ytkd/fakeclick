TARGET=FMouse.EXE
DLL=FMHOOK.DLL
ASM=WASM
CC	= WCC386
RC	= WRC
HC	= HC31
DISASM	= WDIS
LNK	= WLINK

ODIR=OBJ\

INCLUDE=$(WC)\H;$(WC)\H\NT

COPTS = -zk0 -bt=nt -mf

DEFINES	=
RESFLAGS=  -r
OBJS	= &
	$(ODIR)KCLICK.OBJ

DLL_OBJS = &
	$(ODIR)DLL.OBJ

LINK_OBJS = &
	$(ODIR)KCLICK

DLL_LINK_OBJS = &
	$(ODIR)DLL


LINK_FLAGS= NAME $(TARGET) &
	LIBP $(WC)\lib386;$(WC)\lib386\nt &
	LANG JA &
	FORM WINDOWS NT RU WIN=4.0 &
	SORT COMMIT HEAP=1M COMMIT STACK=4K &
	OP STACK=4M,HEAPSIZE=4M,Map=MAPFILE.TXT,EL,&
	ALIGNMENT=512,OBJA=4096, &
	DE FAKEMOUSE,VERS=1.0, &
	MODN=FM,VERBOSE,STATICS

DLL_LINK_FLAGS= NAME $(DLL) &
	LIBP $(WC)\lib386;$(WC)\lib386\nt &
	LANG JA &
	SYS NT_DLL &
	SEGMENT Class DATA SH &
	EXPORT InstallHook='_InstallHook@8', UninstallHook='_UninstallHook@0', get_stat='_get_stat@4'&
	SORT COMMIT HEAP=1M COMMIT STACK=4K &
	OP STACK=4M,HEAPSIZE=4M,Map=MAPDLL.TXT,EL, &
	ALIGNMENT=512,OBJA=4096, &
	DE FMHOOK,VERS=1.0, &
	MODN=FMHOOK,VERBOSE,STATICS


LINK_LIBS = LIB kernel32,user32,gdi32,shell32
DLL_LINK_LIBS = LIB kernel32,user32

OPTION =

!ifdef D
CFLAGS = -D_DEBUG -3r -d2 -db -hd -s -w=4
LINK_OPTS= DEBUG DWARF
DLL_LINK_OPTS= DEBUG DWARF
!else
CFLAGS = -DRELEASE -5r -oabhkrs -s -w=4 -d0
LINK_OPTS = OP NOD,NOR,START='Entry_'
DLL_LINK_OPTS = OP NOD,START='_DllEntryPoint@12'
!endif

.EXTENSIONS:
.EXTENSIONS: .exe .lib .obj .asm .c .cpp .res .rc .h

.c.obj:
	$(CC) -i=$(INCLUDE) $(COPTS) $(CFLAGS) $(OPTION) -fo$@ $[@

.asm.obj:
	$(ASM) $(AFLAGS) -fo$@ $[@

.rc.res:
	$(RC) $(RCDEFINES) $(RESFLAGS) -i=$(INCLUDE) -fo$@ $[@

all :: $(TARGET) $(DLL)


$(TARGET): $(OBJS) $(RES)
	SET WATCOM=$(WC)
	-del $(TARGET)
	$(LNK) @<<
	$(LINK_FLAGS)
	$(LINK_OPTS)
	FILE $(LINK_OBJS)
	$(LINK_LIBS)
<<
	SET WATCOM=

$(DLL): $(DLL_OBJS)
	SET WATCOM=$(WC)
	-del $(DLL)
	$(LNK) @<<
	$(DLL_LINK_FLAGS)
	$(DLL_LINK_OPTS)
	FILE $(DLL_LINK_OBJS)
	$(DLL_LINK_LIBS)
<<
	SET WATCOM=
# 	name $(DLL) sys nt_dll op m op nod libp d:\wc\lib386\nt libr user32, kernel32 exp InstallHook='_InstallHook@8',UninstallHook='_UninstallHook@0' op q op symf op el op nod SEGMENT TYPE DATA SHARED FILE OBJ\DLL.OBJ

