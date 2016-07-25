TARGET=$(shell basename `pwd`)
SOURCES=$(shell echo *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
$(TARGET): $(OBJECTS)
	g++ -pthread `sdl2-config --libs` $(OBJECTS) -o $(TARGET)
%.o: %.cpp
	g++ -Wall -march=native -gdwarf-3 -pthread -g -std=c++1y `sdl2-config --cflags` -c -O3 -MM $< > $@.mk~
	g++ -Wall -march=native -gdwarf-3 -pthread -g -std=c++1y `sdl2-config --cflags` -c -O3 -o $@ $<
-include *.mk~
clean:
	rm -f *~
	rm -f *.o
	rm -f $(TARGET)
