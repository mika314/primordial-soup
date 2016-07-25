TARGET=$(shell basename `pwd`)
SOURCES=$(shell echo *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
$(TARGET): $(OBJECTS)
	g++ -pthread -lGL -lSDL2 $(OBJECTS) -o $(TARGET)
%.o: %.cpp
	g++ -Wall -march=native -gdwarf-3 -pthread -g -std=c++1y -c -O3 -MM $< > $@.mk~
	g++ -Wall -march=native -gdwarf-3 -pthread -g -std=c++1y -c -O3 -o $@ $<
-include *.mk~
