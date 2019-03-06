#
## Makefile
#
SOURCES.c= run_convert.c NowcastToWDSS2.c
$(CC)=gcc 
# insert here -lnetcdf
CFLAGS=-Wall -g -I/usr/local/netcdf/include
LDFLAGS=-lm -L/usr/local/netcdf/lib -lnetcdf

PROGRAM=NowcastToWDSS2 

OBJECTS= $(SOURCES.c:.c=.o)

.KEEP_STATE:

debug := CFLAGS= -g -s

all debug: $(PROGRAM)

$(PROGRAM): $(INCLUDES) $(OBJECTS)
		$(LINK.c) -o $@ $(OBJECTS) $(LDLIBS)

clean:
		rm -f $(PROGRAM) $(OBJECTS)

