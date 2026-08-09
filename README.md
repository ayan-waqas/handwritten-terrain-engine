# Procedural Terrain Engine

A 3D procedural terrain engine built from scratch using C++17, OpenGL 3.3 Core, GLFW, and GLAD without using external math libraries (like GLM), game engines, or heavy frameworks.

---

## Highlights

* **Procedural Land Generation**: Generates infinite landscapes with rolling plains, river canyons, and mountain ranges on the fly by combining multi-layered Perlin noise (fBm) with seamless chunk heightmap stitching.
* **Hydraulic Terrain Erosion**: Simulates natural water run-off and sediment transport across heightmaps to carve realistic gullies, river channels, and weathered mountain slopes.
* **Handwritten 3D Math Library**: Built without external math dependencies like GLM, implementing custom 3D vector, matrix, perspective, and orthographic transformation math from scratch.
* **Real-Time Dynamic Shadows**: Renders crisp sun and moon shadows across mountains and trees using an off-screen depth shadow map framebuffer paired with Percentage-Closer Filtering (PCF) for soft edges.
* **24-Hour Day & Night Cycle**: Smoothly transitions sky colors, lighting, and celestial bodies based on sun angle calculations, complete with procedural night stars, a moon disc, and glowing fireflies.
* **Wind-Animated Grass**: Renders dense 3D grass fields that sway naturally in real-time by feeding a scrolling 2D noise wind field straight into vertex shader displacement logic.
* **Water Reflections & Waves**: Renders realistic water with dynamic wave motion and planar reflections created using an inverted-camera render pass blended with Schlick's Fresnel model.
* **Post-Processing Effects**: Enhances visual fidelity using custom screen-space post-processing shaders for volumetric god rays, bloom, subtle vignette, and ACES filmic color grading.
* **Mountain Detail & Rock Strata**: Keeps steep mountain cliffs sharp and un-stretched using triplanar normal texture mapping, combined with slope-based snow cover on high peaks.
* **Custom 3D Model Loader & Particles**: Features a custom Wavefront OBJ parser that feeds low-poly tree geometry directly into OpenGL vertex buffers alongside a 3D particle system for ambient pollen and fireflies.
* **Background Audio**: Keeps atmospheric background music looping seamlessly in the background using an embedded, lightweight miniaudio C engine.

---

## Controls

* **WASD**: Movement
* **Shift**: Sprint
* **T**: Toggle Day/Night Cycle
* **F11**: Toggle Fullscreen
* **ESC**: Exit

---

## Build Instructions

Tested only on Arch Linux(btw)

### Prerequisites

* GCC / Clang compiler with C++17 support
* CMake 3.20+
* GLFW3

### Building and Running

```bash
git clone https://github.com/ayan-waqas/terrain-engine.git
cd terrain-engine

cmake -B build
cmake --build build

./build/terrain-engine
```

---

## Project Structure

```
terrain-engine/
├── audio/                    # Background Audio Files
├── external/                 # GLAD & Miniaudio Libraries
├── models/                   # 3D Low-Poly Tree Assets
├── shaders/                  # OpenGL 3.3 GLSL Shaders
└── src/                      # C++ Engine Source Code
    ├── audio/                # Audio Engine
    ├── camera/               # First-Person Camera
    ├── math/                 # Vector & Matrix Math Classes
    ├── noise/                # Procedural Noise Generators
    └── terrain/              # Chunks, Heightmaps & Mesh Generators
```