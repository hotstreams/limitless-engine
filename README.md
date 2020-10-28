# C++ Graphics Engine

### The project is cross-platform 3D graphics engine, focused on high-performance, low-overhead rendering with modern OpenGL.

- Features:
    - State caching
    - Multithreaded context resource sharing
    - MaterialCompiler + shader program introspection
    - Indexed buffers auto binding
    - Texture unit auto binding
    - Buffer streaming: Orphaning, Unsynchonized, Persistent, Coherent, + fences
    - Immutable storage support
    - Named buffer objects support
    - Bindless textures
    - Persistent/Coherent mapping + TripleBuffering + Explicit Synchronization

- Material System
  - Albedo
  - Specular
  - Normal
  - Emissive color
  - Emissive mask
  - Blend mask
  - Roughness + per sample
  - Metalness + per sample
  + Blending: Opaque, Translucent, Additive, Modulate
  + Shading shader_type: Lit, Unlit
  + Shading models: Phong, Blinn-Phong
  + Custom materials via shader code
  
![](screenshots/materials.png)
  
- Dynamic Direct Lighting
- Rigged Model Animations

![me](screenshots/model_loader.gif)

- Post Processing: HDR, tone mapping, gamma correction, bloom, vignette, tone shading, FXAA

![](screenshots/tone_shading.png)

Sponza model

![](screenshots/sponza.png)


# Usage

[Core](https://github.com/hotstreams/graphics-engine/wiki/Engine-Core)

# Dependencies
- glfw3
- glew
- glm
- assimp
- stb_image
