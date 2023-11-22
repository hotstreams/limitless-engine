Material shading models:
    unlit
    lit (pbr)
    cloth
    subsurface
    custom

lit pbr
    color
    diffuse albedo
    metallic
    roughness
    reflectance
    ambient occlusion
    normal
    emissive
    ior
    absorption
    transmission


How final surface color is calculated:

1) You should compute MaterialContext
    MaterialContext already contains default material parameters and user invoked code to customize this parameters
2) You should compute ShadingContext from MaterialContext or pass parameters yourself
3) Lighting is calculated from ShadingContext
4) 


transmission/ absorption / energyconserv ?

refraction works only on translucent objects = forward render after deferred


make shaders include-consistent