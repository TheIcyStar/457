# 	CMPSC 457 Section 001
# 	Homework 6
#
# 	Alexander Petrov
# 	aop5448
#
# 	11/18/25


# compiler to be used
CC = g++

# flags to the compiler
# CXX_FLAGS = -Wall -std=c++0x -pedantic -O2
CXX_FLAGS = -Wall -std=c++0x -pedantic -O1
CXX_FLAGS += -g





# path to directories containing header files
#INC_DIR = -I. -I/home/cmpsc457/glm-master/
INC_DIR = -I.

# GL related libraries
GL_LIBS = -lglut -lGLU -lGL

# X related libraries
X_LIBS = -lXext -lm

# Dependent files
DEP_H = model.h
DEP_CXX = model.cxx



#### TARGETS ####

template: template.cxx $(DEP_H) $(DEP_CXX)
	$(CC) -o template template.cxx $(DEP_CXX) $(CXX_FLAGS) $(INC_DIR) $(GL_LIBS) $(X_LIBS)

run: template
	./template

clean:
	rm -f template  *.o *~
