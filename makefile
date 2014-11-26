# Make recognizes makefile and Makefile, otherwise need to -f and specify
#	make -f mymakefile.txt

COMPILER = g++			# compiler specific to file extension, g++ only invoked on .cpp
PROGNAME = funproj
CFLAGS = -g -Wall -O2 -pthread -std=c++11	# gnu99 in for declarations, c++11 for non static data member initialiazer
#LIBS = -lmyLib 
#LIBS = -L/usr/local/lib/apr -lapr-1		# require -L for spec directory, otherwise -l 
LIBS = -lGL -lglut
SRC =  funProj.cpp #-I/usr/local/include/gl
OBJ = #funProj.o 

#-I/usr/local/include/apr
# Explicit rules: Which files depend on the compile of other files
#myProgram : main.c
#g++ -o myProgram main.c	# command here, tab char before each command

# Explicit rules: Which files depend on the compile of other files #$(OBJ)
$(PROGNAME) : $(SRC) 
	$(COMPILER) $(CFLAGS) $(SRC) $(LIBS) $(OBJ) -o $(PROGNAME)


# Implicit rules: like explicit, but no commands, suffex tells what commandtodo
funProj.o: funProj.cpp

# Note;
# -v for verbose; print all steps gcc/g++ takes to do compile
# -Werror; warnings are errors
# add external flags from a file using @;
#	gcc main.c @optionFile

#ifdef MY_MACRO
  #printf("\n Macro defined \n");
#endif

#then; use -D, to def macro; called compile time macros
#gcc -Wall -DMY_MACRO main.c -o main
