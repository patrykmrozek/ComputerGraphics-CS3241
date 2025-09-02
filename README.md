# Computer Graphics - CS3241

A collection of OpenGL projects and exercises for Computer Graphics class.

## Interactive 3D Sphere

A wireframe sphere renderer with interactive camera controls built using OpenGL and GLUT.

![Demo](media/demo.gif)

### Controls

- **W/S**: Orbit camera up/down
- **Z/C**: Orbit camera left/right  
- **A/D**: Rotate sphere
- **Q/E**: Scale sphere up/down
- **ESC**: Exit

### Building and Running

```bash
gcc -o sphere sphere.c -lGL -lGLU -lglut -lm
./sphere
```

---

*CS3241 Computer Graphics coursework*
