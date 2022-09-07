.PHONY: all clean clean-all

GXX=g++
FLAGS=-lSDL -lSDL_ttf
BIN_DIRECTORY=bin
SRC_DIRECTORY=src

all: $(BIN_DIRECTORY)/app_map $(BIN_DIRECTORY)/app_training

#app_map
$(BIN_DIRECTORY)/app_map: $(BIN_DIRECTORY)/main_map.o
	$(GXX) $^ -o $@ $(FLAGS)

$(BIN_DIRECTORY)/main_map.o: main_map.cpp
	$(GXX) -o $@ -c $< $(FLAGS)

#app_training
$(BIN_DIRECTORY)/app_training: $(BIN_DIRECTORY)/main_training.o $(SRC_DIRECTORY)/m_learning.o $(SRC_DIRECTORY)/world.o
	$(GXX) $^ -o $@ $(FLAGS)

$(BIN_DIRECTORY)/main_training.o: main_training.cpp
	$(GXX) -o $@ -c $< $(FLAGS)


$(BIN_DIRECTORY)/world.o: $(SRC_DIRECTORY)/world.cpp $(SRC_DIRECTORY)/world.h
	$(GXX) -o $@ -c $< $(FLAGS)

$(BIN_DIRECTORY)/m_learning.o: $(SRC_DIRECTORY)/m_learning.cpp $(SRC_DIRECTORY)/m_learning.h
	$(GXX) -o $@ -c $<


clean:
	rm -f $(BIN_DIRECTORY)/*.o

clean-all: clean
	rm -f $(BIN_DIRECTORY)/app_map