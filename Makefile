CC = g++

CPPFLAGS = -g -Wall

LINKER_FLAGS = -lSDL2

APP_NAME = RPG

SRCS=src/App.cc src/Map.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: $(APP_NAME)

clean:
	rm -rf $(OBJS)

App.o : src/App.cc src/*.h

Map.o : src/Map.cc src/*.h

$(APP_NAME) : $(OBJS)
	$(CC) $^ $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $@
