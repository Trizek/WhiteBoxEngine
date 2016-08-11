CC = g++ -std=gnu++11
INCLUDE = -I../../Core/Inc/ -I../../Editor/Inc

#FRAMEWORK = -framework OpenGL -framework GLUT -framework Carbon -framework SystemConfiguration
LIBS_PATHS = -L../../Core/Libs/FreeImage 
LIBS = -lfreeimage -lm
#-lfbxsdk_gcc4_ub -lm -lstdc++ -liconv -fexceptions -lz 
SOURCE = ../../Core/Src/System/*.cpp 
#../../Core/Src/Render/*.cpp ../../Core/Src/System/*.cpp ../../Core/Src/Animation/*.cpp ../../Editor/Src/*.cpp


all:
	$(CC) $(INCLUDE) $(LIBS_PATHS) $(FRAMEWORK) $(LIBS) $(SOURCE) -o test
