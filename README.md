![me](content/logo.jpg)

## The project is 3D graphics engine, focused on high-performance, low-overhead rendering with modern OpenGL & C++17.

### Lighting

- Forward/Deferred shading
- Dynamic directional light
- Dynamic point lights
- Dynamic spot lights
- Directional Cascade Shadow maps
- Percentage-Closer Filtering
- Skybox

![me](content/lighting.gif)

---

### Material System

Material Properties:
- Сolor
- Albedo texture
- Normal map
- Emissive color
- Emissive mask
- Blend mask
- Refraction
- Ambient occlusion
- Transparency: Translucent, Additive, Modulate
- Shading: Lit, Unlit
- Metallic & roughness workflow
- Material layering
- Custom materials via GLSL snippets that allow you to create whatever material you want
- All properties are run-time changeable

![me](content/color.png) 
![me](content/albedo.png)
![me](content/emissive.png)
![me](content/open.gif)
![me](content/masked.png)
![me](content/refraction.png)

![me](content/basic1.png)
![me](content/basic2.png)
![me](content/basic3.png)
![me](content/basic4.png)
![me](content/basic5.png)
![me](content/basic6.png)
![me](content/basic7.png)

![me](content/translucent.png)
![me](content/additive_modulate.png)

![me](content/custom_materials.gif)
![me](content/layering.gif)

![me](content/fireball_hue_shift.gif)

---

### Effect System

- Sprite / Mesh / Beam emitters
- 'initial' & 'by life' modules, const & range value distributions
- Interoperation with material custom properties
- Color, location, velocity, acceleration, size, lifetime, mesh location, mesh attachment, rotation, rotation rate, custom material, beam targets & speed & offset & rebuild & displacement

![me](content/smoke.gif)
![me](content/shield.gif)
![me](content/fire.gif)
![me](content/explosion.gif)
![me](content/ball.gif)
![me](content/lightning_effect.gif)
![me](content/attachment.gif)
![me](content/skeleton_spawn_effect.gif)
![me](content/aura_effect.gif)
  
---

### Model Loading

- Model loading using Assimp
- Skeletal models with animations
- Instancing support

![me](content/thanos.png)
![me](content/daenerys.png)
![me](content/taskmaster.png)
![me](content/k2.png)
![me](content/skeleton.png)
![me](content/skeletal_model.gif)
![me](content/backpack.png)
![me](content/cyborg.png)
![me](content/drone.png)
![me](content/elemental.png)

---

### Core Features
- OpenGL state caching for reducing driver overhead 
- Multithreaded OpenGL context resource sharing
- ShaderCompiler & ShaderProgram introspection
- Indexed buffers automatic binding to location
- Textures automatic binding to texture units
- Buffer data streaming: Orphaning, Unsynchonized, Persistent, Coherent, TrippleBuffering, Explicit synchronization
- Supports GL_ARB_buffer_storage for immutable buffers
- Supports GL_ARB_texture_storage for immutable textures
- Supports GL_ARB_direct_state_access for VertexArrays, BufferObjects, Textures, etc
- Supports GL_ARB_bindless_texture for bindless textures
- Supports GL_ARB_shader_storage_buffer_object for large blocks in shaders

---

### PostProcessing
  - Tone mapping
  - Gamma correction
  - Bloom
  - Vignette
  - Tone shading
  - FXAA
  - SSAO
---

### Text rendering using FreeType
---

### Content
  
![me](content/materials_scene.png)

![me](content/effects_scene.png)

![me](content/models_scene.png)

![me](content/sponza.png)

![me](content/warlocks.png)

# Build
The project uses C++17 and CMake's find_package for dependencies.

# Dependencies
- glfw3
- glew
- OpenGL
- glm
- assimp
- stb_image
- stb_image_resize
- freetype
