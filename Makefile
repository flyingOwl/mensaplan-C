LIBS = -lcurl

CFLAGS        = -Wall
CFLAGSRELEASE = -Wall -O2
CFLAGSDEBUG   = -Wall -g

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

EXEFILE    = Mensaplan-C
EXEFILEDBG = MensaDebug

all: clean build

debug: CFLAGS  = $(CFLAGSDEBUG)
debug: EXEFILE = $(EXEFILEDBG)
debug: clean build

release: CFLAGS = $(CFLAGSRELEASE)
release: clean build

build: $(OBJS)
	gcc -o $(EXEFILE) $^ $(CFLAGS) $(LIBS)

clean:
	rm -f *.o
	rm -f $(EXEFILE)
	rm -f $(EXEFILEDBG)

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)
