CC = g++ -g -std=gnu++11
INCLUDE = -I/usr/include/SDL2/ -I../../Editor/Libs/DDS/ -I../../Editor/Libs/FreeImage/Src/Source -I../../Core/Libs/FreeType/include -I../../Core/Inc/ -I../../Editor/Inc

#FRAMEWORK = -framework OpenGL -framework GLUT -framework Carbon -framework SystemConfiguration
LIBS_PATHS = -L/usr/lib/x86_64-linux-gnu/ -L../../Editor/Libs/FreeImage/Src/Dist 
LIBS = -lm -lSDL2 -lSDL2main -lGL -lGLU -lGLEW -lfreetype -lfreeimage
#-lfbxsdk_gcc4_ub -lm -lstdc++ -liconv -fexceptions -lz 
SOURCES = $(wildcard ../../Core/Src/*.cpp) $(wildcard ../../Core/Src/Render/*.cpp) $(wildcard ../../Core/Src/System/*.cpp) $(wildcard ../../Editor/Src/*.cpp) ../../Editor/Libs/DDS/image_DXT.c
OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS_C = $(SOURCES:.c=.o)


all: $(OBJECTS) $(OBJECTS_C)
	$(CC) *.o $(LIBS_PATHS) $(FRAMEWORK) $(LIBS) -o test


%.o: %.c*
	$(CC) $(INCLUDE) -c -o `basename $@` $<


clean:
	rm *.o test
