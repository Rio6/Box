SRCDIR:=src
BUILDDIR:=build

CFLAGS+=-O2
LIBS:=-lSDL2 -lm
INCLUDES:=-I/usr/include/SDL2/
SRCS:=$(wildcard $(SRCDIR)/*.c)
HDRS:=$(wildcard $(SRCDIR)/*.h)
OBJS:= $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))
TGT:=box

all: $(BUILDDIR) $(TGT)
win32: winicon32
	$(MAKE) CC=i686-w64-mingw32-gcc LIBS="-static-libgcc -Wl,-Bstatic -lpthread -Wl,-Bdynamic -lmingw32 -lSDL2main $(LIBS) -mwindows" TGT="$(TGT).exe" OBJS="$(OBJS) $(BUILDDIR)/icon.o" all
win64: winicon64
	$(MAKE) CC=x86_64-w64-mingw32-gcc LIBS="-static-libgcc -Wl,-Bstatic -lpthread -Wl,-Bdynamic -lmingw32 -lSDL2main $(LIBS) -mwindows" TGT="$(TGT).exe" OBJS="$(OBJS) $(BUILDDIR)/icon.o" all

winicon32: $(BUILDDIR)/icon.rc
	i686-w64-mingw32-windres -i $(BUILDDIR)/icon.rc -O coff -o $(BUILDDIR)/icon.o
winicon64: $(BUILDDIR)/icon.rc
	x86_64-w64-mingw32-windres -i $(BUILDDIR)/icon.rc -O coff -o $(BUILDDIR)/icon.o

$(BUILDDIR)/icon.rc: $(BUILDDIR) box.png
	convert -resize 256x256 box.png $(BUILDDIR)/box.ico
	echo 0 ICON box.ico > $(BUILDDIR)/icon.rc

$(TGT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TGT)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	$(RM) -r build

depend: .depend

.depend: $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) -MM $^ | sed 's|[a-zA-Z0-9_-]*\.o|$(BUILDDIR)/&|' > .depend

include .depend
