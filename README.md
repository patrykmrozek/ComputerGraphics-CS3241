# Computer Graphics - CS3241

A collection of OpenGL projects and exercises for Computer Graphics class.

## Interactive 3D Sphere

A wireframe sphere renderer with interactive camera controls built using OpenGL and GLUT.

<video width="600" controls>
  <source src="media/sphereWireframe.mp4" type="video/mp4">
  Your browser does not support the video tag.
</video>

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

*CS3241 Computer Graphics coursework*
