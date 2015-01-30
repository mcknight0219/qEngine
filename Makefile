EXECUTABLE = qEngine

GLES_INCLUDE = /opt/Imagination/PowerVR_Graphics/PowerVR_SDK/SDK_3.4/Builds/Include

CFLAGS = -Wall -g -I$(GLES_INCLUDE)
CFLAGS += `sdl-config --cflags`
LDFLAGS = -lGLEW -lGL -lGLU -lIL -lm `sdl-config --libs`

engine_SOURCES := $(wildcard ./*.cpp)
engine_OBJECTS := $(engine_SOURCES:.cpp=.o)

OBJECTS = $(engine_OBJECTS)

all: $(EXECUTABLE)

qEngine: $(OBJECTS)
	g++ -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	g++ -o $@ -c $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
