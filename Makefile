# Simple Makefile
CXX = g++
#CXX = gcc
PROJ = OpenCV_OpenGL
#EDITOR = gvim
EDITOR = emacs
SISTEMA = linux

OPENCV_VERSION = `pkg-config --modversion opencv4`
ifeq ($(OPENCV_VERSION), )
	OPENCV_VERSION = opencv
else
	OPENCV_VERSION = opencv4
endif

all: 	${SISTEMA}	
	

clean:
	-rm -f *.o core $(PROJ)
edit:
	-$(EDITOR) $(PROJ).cpp &

PHONY: mac linux

linux:	$(PROJ).cpp
	@echo "Treballant en OpenCV >${OPENCV_VERSION}<"
ifeq ($(strip $(OPENCV_VERSION)), )
	$(CXX) $(PROJ).cpp -o $(PROJ) -g `pkg-config --cflags opencv glu gl` -lglut `pkg-config --libs opencv glu gl`
else
	$(CXX) $(PROJ).cpp -o $(PROJ) -g `pkg-config --cflags opencv4 glu gl` -lglut `pkg-config --libs opencv4 glu gl`
endif

mac: $(PROJ).cpp
	$(CXX)  $(PROJ).cpp -o $(PROJ)_mac -g `pkg-config --cflags opencv` -framework GLUT -framework OpenGL `pkg-config --libs opencv`
