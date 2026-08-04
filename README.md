# Handwritten Procedural Terrain Engine

A high-performance C++17 / OpenGL 3.3 Core procedural terrain rendering engine built from scratch without external graphics software libraries (only GLFW and GLAD).

---

## 🌟 Key Features

* **Real-time Directional Shadow Mapping**: Dynamic sun & moon shadows rendered into a 2048×2048 depth FBO with 3×3 PCF (Percentage-Closer Filtering) soft shadow sampling across mountains, trees, and rocks.
* **Continental Geographic Noise**: Procedural landform generation partitioning the infinite world into **Lush Rolling Plains**, **Terraced River Canyons**, and **Alpine Mountain Ranges**.
* **Dynamic 24-Hour Day/Night Cycle**: Seamless time-of-day transitions (Sun orbit, Dawn/Dusk sky color palettes, Procedural Stars, Night Moon disc, and glowing Fireflies). Press **`T`** to toggle auto-advance.
* **LesleyLai-Style Grass Blade System**: Dense 3-plane star tuft grass geometry featuring a vertex shader 2D scrolling wind field ripple effect.
* **Real-time Planar Water Reflections**: Lower-resolution reflection FBO pass sampling distorted scene reflections with Fresnel blending, wave normal dynamics, and depth color gradients.
* **Geological Rock Strata & Micro-Detailing**: Multi-frequency domain-warped rock strata, crevice ambient occlusion, steep snow ice glint, and Half-Lambert foliage subsurface lighting.
* **Post-Processing Pipeline**: Volumetric God Rays (radial sun shaft blur), ACES Filmic Tone Mapping, Bloom, and Vignette.
* **Custom 3D Wavefront OBJ Model Loading**: Custom OBJ parser supporting low-poly tree assets with leaf/bark color assignment.
* **Dynamic Particle System**: 600 floating firefly / pollen motes wrapped around player coordinates with point size attenuation and radial glow halos.
* **Thermal & Slope Erosion**: Fast heightmap erosion pass smoothing out unnatural cliff needles into natural scree slopes.

---

## 🎮 Controls

| Key | Action |
|---|---|
| **W / A / S / D** | Move Camera (Forward / Left / Backward / Right) |
| **Shift** | Sprint (Speed Boost) |
| **Mouse** | Look around (FPS Camera) |
| **T** | Toggle Dynamic Day/Night Cycle Auto-Advance |
| **F11** | Toggle Fullscreen Mode |
| **ESC** | Exit Application |

---

## 🛠️ Build Instructions

### Prerequisites
* **Compiler**: GCC / Clang supporting C++17
* **Build System**: CMake 3.10+
* **Dependencies**: GLFW3, OpenGL drivers (GLAD included in repository)

### Building & Running

```bash
# Clone the repository
git clone https://github.com/ayan-waqas/handwritten-terrain-engine.git
cd handwritten-terrain-engine

# Generate build files & compile
cmake -B build
cmake --build build

# Run the engine
./build/handwritten-terrain-engine
```

---

## 📂 Project Architecture

```
handwritten-terrain-engine/
├── models/                   # 3D OBJ Assets (Lowpoly Trees)
├── shaders/                  # GLSL Shaders (OpenGL 3.3 Core)
│   ├── basic.vert / .frag    # Terrain Shading (Biomes, Rock Strata, AO)
│   ├── water.vert / .frag    # Planar Water Reflections & Waves
│   ├── skybox.vert / .frag   # Day/Night Sky Gradient, Stars & Moon
│   ├── tree.vert / .frag     # OBJ Trees & Subsurface Foliage Lighting
│   ├── grass.vert / .frag    # Animated Grass Blades & Wind Ripples
│   ├── particle.vert / .frag # Firefly & Pollen Particle Halo Shader
│   └── postprocess.vert / .frag # God Rays, Bloom & ACES Tone Mapping
├── src/
│   ├── camera/Camera.h       # First-Person Camera & Reflection Matrix
│   ├── math/                 # Custom Vec3 & Mat4 Math Classes
│   ├── noise/PerlinNoise.h   # Multi-Octave fBm & Continental Noise
│   ├── terrain/
│   │   ├── Heightmap.h       # Grid Mesh & Thermal Erosion
│   │   ├── ChunkManager.h    # Infinite Infinite Chunk Loading & Culling
│   │   ├── Water.h           # Water Plane Mesh
│   │   ├── Skybox.h          # Sky Box Geometry
│   │   ├── Tree.h            # OBJ Loader & Mesh Setup
│   │   ├── Rock.h            # Procedural Low-Poly Rock Geometry
│   │   ├── Grass.h           # Curved 3-Plane Grass Blade Tuft
│   │   └── Particles.h       # Floating 3D Particle System
│   ├── Engine.h / .cpp       # Main Loop, FBO Pipeline & Input Handling
│   └── main.cpp              # Entry point
└── CMakeLists.txt            # Build System Script
```