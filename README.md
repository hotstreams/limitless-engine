# C++ Graphics Engine

####The project is cross-platform 3D graphics engine, focused on high-performance, low-overhead rendering with modern OpenGL.

- Features:
    - State caching
    - ShaderCompiler + shader program introspection [ GL_ARB_program_interface_query ]
    - Indexed buffers auto binding
    - Buffer streaming: Orphaning, Unsynchonized, Persistent, Coherent, + fences
    - Immutable storage support [ GL_ARB_buffer_storage/GL_ARB_texture_storage ]
    - Named buffer objects support [  GL_ARB_direct_state_access ]
    - Bindless textures [ GL_ARB_bindless_texture ]
    - Persistent/Coherent mapping + TripleBuffering + Explicit Synchronization [ GL_ARB_buffer_storage ]

- Material System
  - Albedo
  - Specular
  - Normal
  - Emissive color
  - Emissive mask
  - Blend mask
  - Roughness
  - Metalness
  + Blending: Opaque, Translucent, Additive, Modulate
  + Shading type: Lit, Unlit
  + Shading models: Phong, Blinn-Phong
  + Custom materials via shader code
  
  ![](screenshots/materials.png)
  
- Dynamic Direct Lighting
- Rigged Model Animations

![me](screenshots/model_loader.gif)

- Post Processing: HDR, tone mapping, gamma correction, bloom, vignette, tone shading

![](screenshots/tone_shading.png)

![](screenshots/tone_shading2.png)

Sponza model

![](screenshots/sponza.png)

# Dependencies
- glfw3
- glew
- glm
- assimp
- stb_image


    
    