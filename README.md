# Handwritten Procedural Terrain Engine

A C++17 and OpenGL 3.3 Core procedural terrain engine built from scratch using C++, OpenGL, GLFW, and GLAD without third-party graphics frameworks.

---

## Key Highlights

* **Continental Geographic Terrain Noise**: Generates infinite, diverse landscapes featuring lush rolling plains, carved river canyons, crater puddles, and alpine mountain chains by blending multi-octave Fractal Brownian Motion (fBm) noise functions.
* **Real-Time Directional Shadow Mapping**: Calculates dynamic sun and moon shadows cast across mountains and trees using a 1024x1024 depth framebuffer with 9-tap Gaussian Percentage-Closer Filtering (PCF) for smooth, soft shadow edges without visual acne.
* **Dynamic 24-Hour Day/Night Cycle**: Simulates time-of-day progression with orbiting directional sun lighting, atmospheric sky color gradients, procedural night stars, a moon disk, and ambient glowing fireflies.
* **Animated Wind Grass Blade System**: Renders dense grass carpets using 3-plane star-tuft geometry deformed in real-time by a scrolling 2D vector wind field for natural foliage sway.
* **Real-Time Planar Water Reflections**: Simulates water surfaces using an off-screen reflection framebuffer pass with Fresnel light blending, depth color gradients, and animated wave normal distortion.
* **Cinematic Post-Processing Pipeline**: Enhances visual fidelity through an off-screen post-processing pass combining volumetric god rays (radial sun shafts), sun flare halos, ACES filmic tone mapping, bloom, and screen vignette.
* **Geological Rock Strata and Surface Detailing**: Applies procedural triplanar noise texturing to eliminate texture stretching on cliffs, steepness-dependent snow accumulation, crevice ambient occlusion, and Blinn-Phong rock specular glints.
* **Custom 3D Model Parser and Particle System**: Includes a custom Wavefront OBJ file parser for low-poly tree assets and an active particle engine managing 600 distance-attenuated 3D pollen and firefly motes.
* **Heightmap Slope Erosion**: Applies iterative heightmap erosion to smooth out artificial cliff artifacts into natural scree slopes and river basins.

---

## Controls

* **WASD**: Movement
* **Shift**: Sprint
* **Mouse**: Look
* **T**: Toggle Day/Night Cycle Auto-Advance
* **F11**: Toggle Fullscreen
* **ESC**: Exit

---

## Build Instructions

Note: Only tested on Arch Linux.

### Prerequisites

* GCC / Clang with C++17 support
* CMake 3.10+
* GLFW3 library

### Building and Running

```bash
git clone https://github.com/ayan-waqas/handwritten-terrain-engine.git
cd handwritten-terrain-engine

cmake -B build
cmake --build build

./build/handwritten-terrain-engine
```

---

## Project Structure

```
handwritten-terrain-engine/
├── models/                   # 3D OBJ Assets (Low-poly Trees)
├── shaders/                  # GLSL Shaders (OpenGL 3.3 Core)
│   ├── basic.vert / .frag    # Terrain Shading, Biomes & Rock Strata
│   ├── water.vert / .frag    # Planar Water Reflections & Waves
│   ├── skybox.vert / .frag   # Day/Night Sky Gradient, Stars & Moon
│   ├── tree.vert / .frag     # OBJ Trees, Subsurface Scattering & Shadows
│   ├── grass.vert / .frag    # Animated Grass Blades & Wind Ripples
│   ├── particle.vert / .frag # Firefly & Pollen Particle Halo Shader
│   ├── shadow.vert / .frag   # Directional Shadow Depth Pass
│   └── postprocess.vert / .frag # God Rays, Bloom & ACES Tone Mapping
├── src/
│   ├── camera/Camera.h       # First-Person Camera & Reflection Matrix
│   ├── math/                 # Custom Vec3 & Mat4 Math Classes
│   ├── noise/PerlinNoise.h   # Multi-Octave fBm & Continental Noise
│   ├── terrain/
│   │   ├── Heightmap.h       # Grid Mesh & Thermal Erosion
│   │   ├── ChunkManager.h    # Infinite Chunk Loading & Culling
│   │   ├── Water.h           # Water Plane Mesh
│   │   ├── Skybox.h          # Sky Box Geometry
│   │   ├── Tree.h            # OBJ Loader & Mesh Setup
│   │   ├── Rock.h            # Procedural Low-Poly Rock Geometry
│   │   ├── Grass.h           # Curved 3-Plane Grass Blade Tuft
│   │   └── Particles.h       # Floating 3D Particle System
│   ├── Engine.h / .cpp       # Render Loop, FBO Pipeline & Input Handling
│   └── main.cpp              # Entry point
└── CMakeLists.txt            # Build System Script
```