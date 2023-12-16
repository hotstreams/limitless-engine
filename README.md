![me](docs/preview/logo.jpg)

## Limitless Engine is a 3D graphics engine, focused on high-performance, low-overhead rendering with modern OpenGL & C++17.

[![Linux Build Status](https://github.com/hotstreams/limitless-engine/workflows/Linux/badge.svg)](https://github.com/hotstreams/limitless-engine/actions?query=workflow%3ALinux)
[![Windows Build Status](https://github.com/hotstreams/limitless-engine/workflows/Windows/badge.svg)](https://github.com/hotstreams/limitless-engine/actions?query=workflow%3AWindows)

### Core Features
- OpenGL state caching for reducing driver overhead
- Multithreaded OpenGL context resource sharing
- Shader Compiler & Shader Program introspection
- Indexed buffers automatic binding
- Textures automatic binding
- Texture Compression support
- Time queries
- Buffer data streaming: Orphaning, Unsynchonized, Persistent, Coherent, TrippleBuffering, Explicit synchronization
- Extension support:
    - [x] GL_ARB_buffer_storage
    - [x] GL_ARB_direct_state_access
    - [x] GL_ARB_shader_storage_buffer_object
    - [x] GL_ARB_shading_language_420pack
    - [x] GL_ARB_explicit_uniform_location
    - [x] GL_ARB_tessellation_shader
    - [x] GL_ARB_program_interface_query
    - [x] GL_ARB_texture_storage
    - [x] GL_ARB_bindless_texture
    - [x] GL_EXT_texture_filter_anisotropic
    - [x] GL_EXT_texture_compression_s3tc
    - [x] GL_ARB_texture_compression_bptc
    - [x] GL_ARB_texture_compression_rgtc
---

https://github.com/hotstreams/limitless-engine/assets/37740577/89b6e76b-e033-4644-829c-9abc744a2341

https://github.com/hotstreams/limitless-engine/assets/37740577/e8ac6aed-a6f1-4125-b2bc-b67584bbaf93

### Rendering

- Forward & Deferred rendering
- Cook-Torrance microfacet specular BRDF
- Lambertian diffuse BRDF
- Roughness-Metallic workflow
- Translucent materials
- Normal mapping
- Ambient occlusion mapping
- Dynamic directional light, point lights and spot lights
- Directional Cascade Shadow maps
- Percentage-Closer Filtering
- Skybox
- Screen space ambient occlusion
- Screen space reflections
- Screen space refraction
- Tone mapping
- Gamma correction
- HDR Bloom
- FXAA
- Deferred Decals
- Instancing

---

### Material System

- Lit, unlit shading models
- Base color (scalar, texture)
- Metallic (scalar, texture)
- Roughness (scalar, texture)
- Refraction
- Normal
- Emissive color
- Emissive mask
- Blend mask
- Ambient occlusion
- Transparency: Translucent, Additive, Modulate
- Material layering
- Custom materials via GLSL snippets that allow you to create whatever material you want
- All properties are run-time changeable

---

### Effect System

- Sprite, Mesh, Beam emitters
- Initial, By life modules types
- Const, Range value distributions
- Interoperation with material custom properties
- Modules:
    - Color
    - Location
    - Velocity
    - Acceleration
    - Size
    - Lifetime
    - Mesh location
    - Mesh attachment
    - Rotation
    - Rotation rate
    - Custom material
    - Beam targets, speed, offset, rebuild, displacement

<div style="padding: 5px">
    <img src="docs/preview/smoke.gif" width="250" />
    <img src="docs/preview/shield.gif" width="250" />
    <img src="docs/preview/explosion.gif" width="250" />
    <img src="docs/preview/ball.gif" width="250" />
    <img src="docs/preview/lightning_effect.gif" width="250" />
    <img src="docs/preview/attachment.gif" width="250" />
    <img src="docs/preview/skeleton_spawn_effect.gif" width="250" />
    <img src="docs/preview/aura_effect.gif" width="250" />
</div>

---

### Examples

<div style="padding: 5px">
    <img src="docs/preview/bistro.png" width="250" />
    <img src="docs/preview/scene.png" width="250" />
    <img src="docs/preview/sponza1.png" width="250" />
    <img src="docs/preview/alienhelmet.png" width="250" />
    <img src="docs/preview/boombox.png" width="250" />
    <img src="docs/preview/damagedhelmet.png" width="250" />
    <img src="docs/preview/helmet.png" width="250" />
    <img src="docs/preview/bottle.png" width="250" />
    <img src="docs/preview/sponza.png" width="250" />
    <img src="docs/preview/warlocks.png" width="250" />
    <img src="docs/preview/thanos.png" width="250" />
    <img src="docs/preview/daenerys.png" width="250" />
    <img src="docs/preview/taskmaster.png" width="250" />
    <img src="docs/preview/k2.png" width="250" />
    <img src="docs/preview/skeleton.png" width="250" />
    <img src="docs/preview/skeletal_model.gif" width="250" />
    <img src="docs/preview/backpack.png" width="250" />
    <img src="docs/preview/cyborg.png" width="250" />
    <img src="docs/preview/drone.png" width="250" />
    <img src="docs/preview/elemental.png" width="250" />
</div>

# Build
This project requires C++17 compiler and CMake.

1) After cloning this Git repo, initialize its Git submodules, which contain 3rd party dependencies and build glew extensions:
```sh
git submodule init
git submodule update
cd thirdparty/glew
make extensions
```

2) Create a new directory for resulting build and start it:
```sh
mkdir build && cd build
cmake ..
make -j12 limitless_demo
./limitless_demo
```

# Dependencies
- glfw3
- glew
- OpenGL
- glm
- stb_image
- stb_image_resize
- freetype
