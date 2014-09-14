CC = g++
INCLUDE = -I../../../Core/Inc/ -I../../../Editor/Inc -I../../../Editor/Libs/OsX/FbxSdk/include/ -I../../../Editor/Libs/OsX/FbxSdk/include/kbaselib

FRAMEWORK = -framework OpenGL -framework GLUT -framework Carbon -framework SystemConfiguration
LIBS_PATHS = -L../../../Core/Libs/FreeImage -L../../../Editor/Libs/OsX/FbxSdk/lib/ 
LIBS = -lfreeimage -lfbxsdk_gcc4_ub -lm -lstdc++ -liconv -fexceptions -lz 
SOURCE = ../../../Core/Src/*.cpp ../../../Core/Src/Render/*.cpp ../../../Core/Src/System/*.cpp ../../../Editor/Src/*.cpp

all:
	$(CC) $(INCLUDE) $(LIBS_PATHS) $(FRAMEWORK) $(LIBS) $(SOURCE) -o test
