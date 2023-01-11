.PHONY: all clean clean-all

GXX=g++
FLAGS=-lSDL -lSDL_ttf -pthread -lstdc++fs -lstdc++ -std=c++17 
BIN_DIRECTORY=bin
SRC_DIRECTORY=src

all: $(BIN_DIRECTORY)/app_map $(BIN_DIRECTORY)/app_training $(BIN_DIRECTORY)/extract_nn $(BIN_DIRECTORY)/app_algo

#extract_nn
$(BIN_DIRECTORY)/extract_nn: $(BIN_DIRECTORY)/extract_nn.o  $(SRC_DIRECTORY)/m_learning.o
	$(GXX) -o $@ $^ 

$(BIN_DIRECTORY)/extract_nn.o: extract_neuralnetwork.cpp
	$(GXX) -o $@ -c $< $(FLAGS) 


#app_map
$(BIN_DIRECTORY)/app_map: $(BIN_DIRECTORY)/main_map.o
	$(GXX) $^ -o $@ $(FLAGS)

$(BIN_DIRECTORY)/main_map.o: main_map.cpp
	$(GXX) -o $@ -c $< $(FLAGS)

#app_algo
$(BIN_DIRECTORY)/app_algo: $(BIN_DIRECTORY)/main_algo.o $(SRC_DIRECTORY)/world.o $(SRC_DIRECTORY)/draw.o $(SRC_DIRECTORY)/car.o  $(SRC_DIRECTORY)/utils.o $(SRC_DIRECTORY)/m_learning.o $(SRC_DIRECTORY)/capteur.o $(SRC_DIRECTORY)/display.o
	$(GXX) $^ -o $@ $(FLAGS)

$(BIN_DIRECTORY)/main_algo.o: main_algo.cpp
	$(GXX) -o $@ -c $< $(FLAGS)

#app_training
$(BIN_DIRECTORY)/app_training: $(BIN_DIRECTORY)/main_training.o $(SRC_DIRECTORY)/m_learning.o $(SRC_DIRECTORY)/world.o $(SRC_DIRECTORY)/draw.o $(SRC_DIRECTORY)/car.o $(SRC_DIRECTORY)/capteur.o $(SRC_DIRECTORY)/display.o $(SRC_DIRECTORY)/liveTerminal.o $(SRC_DIRECTORY)/multithread.o $(SRC_DIRECTORY)/utils.o
	$(GXX) $^ -o $@ $(FLAGS)

$(BIN_DIRECTORY)/main_training.o: main_training.cpp src/genetic_algorithm_const.h src/utils.h src/utils.cpp
	$(GXX) -o $@ -c $< $(FLAGS)

$(SRC_DIRECTORY)/world.o: $(SRC_DIRECTORY)/world.cpp $(SRC_DIRECTORY)/world.h src/genetic_algorithm_const.h src/utils.h src/utils.cpp
	$(GXX) -o $@ -c $< $(FLAGS)

$(SRC_DIRECTORY)/capteur.o: $(SRC_DIRECTORY)/capteur.cpp $(SRC_DIRECTORY)/capteur.h src/genetic_algorithm_const.h src/utils.h src/utils.cpp
	$(GXX) -o $@ -c $< $(FLAGS)

$(SRC_DIRECTORY)/draw.o: $(SRC_DIRECTORY)/draw.cpp $(SRC_DIRECTORY)/draw.h src/genetic_algorithm_const.h src/utils.h src/utils.cpp
	$(GXX) -o $@ -c $< $(FLAGS)

$(SRC_DIRECTORY)/m_learning.o: $(SRC_DIRECTORY)/m_learning.cpp $(SRC_DIRECTORY)/m_learning.h src/genetic_algorithm_const.h src/utils.h src/utils.cpp
	$(GXX) -o $@ -c $<

$(SRC_DIRECTORY)/display.o: $(SRC_DIRECTORY)/display.cpp $(SRC_DIRECTORY)/display.h src/genetic_algorithm_const.h src/utils.h src/utils.cpp
	$(GXX) -o $@ -c $<

$(SRC_DIRECTORY)/car.o: $(SRC_DIRECTORY)/car.cpp $(SRC_DIRECTORY)/car.h src/genetic_algorithm_const.h src/utils.h src/utils.cpp
	$(GXX) -o $@ -c $<

$(SRC_DIRECTORY)/liveTerminal.o: $(SRC_DIRECTORY)/liveTerminal.cpp $(SRC_DIRECTORY)/liveTerminal.h src/genetic_algorithm_const.h src/utils.h src/utils.cpp
	$(GXX) -o $@ -c $<

$(SRC_DIRECTORY)/multithread.o: $(SRC_DIRECTORY)/multithread.cpp $(SRC_DIRECTORY)/multithread.h src/genetic_algorithm_const.h src/utils.h src/utils.cpp
	$(GXX) -o $@ -c $<

$(SRC_DIRECTORY)/utils.o: $(SRC_DIRECTORY)/utils.cpp $(SRC_DIRECTORY)/utils.h src/genetic_algorithm_const.h 
	$(GXX) -o $@ -c $<

clean:
	rm -f $(BIN_DIRECTORY)/*.o
	rm -f $(SRC_DIRECTORY)/*.o

clean-all: clean
	rm -f $(BIN_DIRECTORY)/app_map $(BIN_DIRECTORY)/app_training $(BIN_DIRECTORY)/extract_nn
