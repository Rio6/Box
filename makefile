SRCDIR:=src
BUILDDIR:=build

CFLAGS+=-O2
LIBS:=-lSDL2 -lm
#INCLUDES:=-Iinclude
SRCS:=$(wildcard $(SRCDIR)/*.c)
HDRS:=$(wildcard $(SRCDIR)/*.h)
OBJS:= $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))
TGT:=box

all: $(BUILDDIR) $(TGT)

$(TGT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TGT)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	$(RM) -r build

depend: .depend

.depend: $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -MM $^ | sed 's|[a-zA-Z0-9_-]*\.o|$(BUILDDIR)/&|' > .depend

include .depend
