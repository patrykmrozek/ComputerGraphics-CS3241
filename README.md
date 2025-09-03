# Computer Graphics - CS3241

A collection of OpenGL projects and exercises for Computer Graphics class.

## Interactive 3D Sphere

A wireframe sphere renderer with interactive camera controls built using OpenGL and GLUT.

![Demo](https://raw.githubusercontent.com/patrykmrozek/ComputerGraphics-CS3241/main/media/sphereWireframe.gif)

### Controls

- **W/S**: Orbit camera up/down
- **Z/C**: Orbit camera left/right  
- **A/D**: Rotate sphere
- **Q/E**: Scale sphere up/down
- **ESC**: Exit

### Building and Running

```bash
g++ -o main main.cpp -framework GLUT -framework OpenGL -DGL_SILENCE_DEPRECATION -framework Cocoa && ./main
./sphere
```
---
## 2D Skull Drawing (Assignment 1)

Strawhat Pirates Logo made in OpenGL.

![Skull](https://raw.githubusercontent.com/patrykmrozek/ComputerGraphics-CS3241/main/media/strawhats-ss2.JPG)
### Features

- Display Lists: GPU-optimized rendering with pre-compiled commands
- Animated Background: Triangle wave color transitions
- Interactive Controls: Real-time transformation and scaling

### Controls

- **A/D**: Rotate skull clockwise/counter-clockwise
- **Q/E**: Scale up/down
- **W/S**: Move up/down
- **Z/C**: Move left/right
- **ESC**: Exit

### Building and Running
```bash
g++ -o main main.cpp -framework GLUT -framework OpenGL -DGL_SILENCE_DEPRECATION -framework Cocoa && ./main
./main
```

---


*CS3241 Computer Graphics coursework*
