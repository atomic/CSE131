.PHONY: clean

# Set the default target. When you make with no arguments,
# this will be the target built.
LEXER = lexer
PRODUCTS = $(LEXER) $(PREPROCESSOR)
default: $(PRODUCTS)

# Set up the list of source and object files
SRCS = errors.cc

# OBJS can deal with either .cc or .c files listed in SRCS
OBJS = lex.yy.o $(patsubst %.cc, %.o, $(filter %.cc,$(SRCS))) $(patsubst %.c, %.o, $(filter %.c, $(SRCS)))

# Link with standard C library, math library, and lex library
LIBS = -lc -lm -ll

# Define the tools we are going to use
CC= g++
LD = g++
LEX = flex

JUNK =  *.o lex.yy.c lexer y.output *~

# We want debugging and most warnings, but lex/yacc generate some
# static symbols we don't use, so turn off unused warnings to avoid clutter
# Also STL has some signed/unsigned comparisons we want to suppress
CFLAGS = -g -Wall -Wno-unused -Wno-sign-compare

# The -d flag tells lex to set up for debugging. Can turn on/off by
# setting value of global yy_flex_debug inside the scanner itself
LEXFLAGS = -d

lex.yy.c: lexer.l 
	$(LEX) $(LEXFLAGS) lexer.l

.cc.o: $*.cc
	$(CC) $(CFLAGS) -c -o $@ $*.cc

$(LEXER) : $(PREPROCESSOR) $(OBJS)
	$(LD) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(JUNK)  

