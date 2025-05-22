CC = g++

COMPILER_FLAGS = -g -Wall

LINKER_FLAGS = -lSDL2

APP_NAME = RPG

SRCS=src/App.cc src/Map.cc
OBJS=$(subst .cc,.o,$(SRCS))

all: $(APP_NAME)

clean:
	rm -rf $(OBJS)

App.o : src/App.cc

Map.o : src/Map.cc

$(APP_NAME) : $(OBJS)
	$(CC) $^ $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $@
