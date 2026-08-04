# Handwritten Procedural Terrain Engine

A high-performance procedural terrain rendering engine built from scratch in C++17 and OpenGL 3.3 Core without external graphics software libraries or engine frameworks (only GLFW and GLAD).

---

## Key Highlights

* **Infinite Dynamic Landscapes**: Generates an endless, seamless world featuring rolling green plains, steep carved river canyons, and towering mountain ranges as you explore.
  * *Technical*: Multi-octave fractional Brownian motion (fBm) continentalness noise blending distinct terrain height regimes with domain warping.
* **Real-time Sun and Moon Shadows**: Dynamic shadows sweep across mountain slopes, trees, and rocks as the sun and moon orbit through a full day/night cycle.
  * *Technical*: Directional shadow mapping using a 1024x1024 depth FBO pass with 9-tap PCF (Percentage-Closer Filtering) and normal-offset bias.
* **Realistic Water with Planar Reflections**: Water surfaces feature real-time reflections of the sky and terrain, depth-based turquoise coloring, and animated wave motion.
  * *Technical*: Off-screen frame buffer object (FBO) planar reflection rendering, Fresnel equation reflection-refraction blending, and wave normal perturbation.
* **Dynamic 24-Hour Day/Night Atmosphere**: Seamless lighting changes from golden sunrise to blue daytime sky, crimson sunsets, and starry nights with a glowing moon disc and floating fireflies.
  * *Technical*: Time-of-day uniform driving hemispheric sky/ground ambient lighting, adaptive distance fog, starfield noise procedural generation, and point particle attenuation.
* **Cinematic Post-Processing Pipeline**: Sun shafts cut through valleys, glowing sunlight, and balanced colors create a filmic aesthetic.
  * *Technical*: Radial blur volumetric God Rays, ACES filmic tone mapping, Gaussian bloom extraction, and dynamic vignette post-processing quad pass.
* **Animated Foliage and Grass Systems**: Dense carpets of grass sway in the wind, and leaf canopies glow translucent when backlit by the sun.
  * *Technical*: 3-plane star tuft grass geometry with scrolling 2D wind vector vertex displacement and foliage subsurface backlight scattering (SSS).
* **Thermal Terrain Erosion**: Mountain cliffs and steep peaks naturally wear down into smooth slopes over time.
  * *Technical*: Iterative heightmap thermal collapse algorithm smoothing talus slope gradients.

---

## Controls

* **WASD**: Movement
* **Shift**: Sprint
* **Mouse**: Look around
* **T**: Toggle Day/Night Cycle
* **F11**: Toggle Fullscreen
* **ESC**: Exit

---

## Build Instructions

*(Only tested on Arch Linux)*

### Prerequisites

* GCC or Clang supporting C++17
* CMake 3.10+
* GLFW3
* OpenGL drivers

### Build and Run

```bash
git clone https://github.com/ayan-waqas/handwritten-terrain-engine.git
cd handwritten-terrain-engine
cmake -B build
cmake --build build
./build/handwritten-terrain-engine
```

---

## Project Architecture

```
handwritten-terrain-engine/
├── models/                   # Low-poly 3D Wavefront OBJ assets
├── shaders/                  # GLSL shaders (OpenGL 3.3 Core)
│   ├── basic.vert / .frag    # Terrain shading (Biomes, Rock strata, AO, PCF Shadows)
│   ├── water.vert / .frag    # Planar water reflections, Fresnel, and depth color
│   ├── skybox.vert / .frag   # Day/Night sky gradient, procedural stars, and moon
│   ├── tree.vert / .frag     # OBJ trees, rocks, and foliage subsurface scattering
│   ├── grass.vert / .frag    # Animated grass tufts and wind displacement
│   ├── particle.vert / .frag # Firefly particle halo shader
│   ├── shadow.vert / .frag   # Directional shadow map depth pass
│   └── postprocess.vert / .frag # Volumetric God rays, Bloom, and ACES tone mapping
├── src/
│   ├── camera/Camera.h       # First-person camera and reflection view matrix
│   ├── math/                 # Custom Vec3 and Mat4 vector/matrix math classes
│   ├── noise/PerlinNoise.h   # Multi-octave fBm and continentalness noise
│   ├── terrain/
│   │   ├── Heightmap.h       # Heightmap grid mesh, seamless borders, thermal erosion
│   │   ├── ChunkManager.h    # Infinite chunk loading, distance culling, object placement
│   │   ├── Water.h           # Water plane geometry
│   │   ├── Skybox.h          # Skybox cube geometry
│   │   ├── Tree.h            # Custom 3D OBJ parser and mesh
│   │   ├── Rock.h            # Procedural low-poly rock mesh
│   │   ├── Grass.h           # 3-plane grass blade geometry
│   │   └── Particles.h       # Floating 3D firefly particle system
│   ├── Engine.h / .cpp       # Main render loop, FBO pipeline, input handling
│   └── main.cpp              # Application entry point
└── CMakeLists.txt            # CMake build manifest
```