CXX = g++
CXXFLAGS = -Wall -DGL_SILENCE_DEPRECATION
FRAMEWORKS = -framework GLUT -framework OpenGL -framework Cocoa

SRC = main.cpp vector3D.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = main 

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(FRAMEWORKS) $(CXXFLAGS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
