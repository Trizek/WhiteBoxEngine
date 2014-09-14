CC = g++
INCLUDE = -ICore/Inc/ -IEditor/Inc
FRAMEWORK = -framework OpenGL -framework GLUT
LIBS_PATHS = -LCore/Libs/FreeImage
LIBS = -lfreeimage
SOURCE = main.cpp Core/Src/*.cpp Core/Src/Render/*.cpp Core/Src/System/*.cpp Editor/Src/*.cpp

all:
	$(CC) $(INCLUDE) $(LIBS_PATHS) $(FRAMEWORK) $(LIBS) $(SOURCE) -o test
