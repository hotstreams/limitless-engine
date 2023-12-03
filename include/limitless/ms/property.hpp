#pragma once

namespace Limitless::ms {
    /**
     *  Describes property of the material
     *
     *  check properties order in material.glsl
     */
    enum class Property {
        /**
         *  Surface color
         *
         *  linear space
         *
         *  vec4 - [0, 1]
         *
         *  rgb - color, a - alpha
         */
        Color,

        /**
         *  Surface emissive color to simulate light emission
         *
         *  (should be used in HDR pipeline)
         *
         *  linear space
         *
         *  vec3 - [0, n]
         *
         *  rgb - color
         */
        EmissiveColor,

        /**
         *  Surface color from texture
         *
         *  vec4 - [0, 1]
         *
         *  if Color is present, gets multiplied
         */
        Diffuse,

        /**
         * Surface normal map from texture
         *
         * vec3 - [0, 1]
         *
         * used by normal mapping
         */
        Normal,

        /**
         *  EmissiveMask describes whether some part of the surface emits light
         *
         *  vec3 - [0, 1]
         *
         *  '0' is used for not emissive texel
         *  '1' is used for emissive texel
         *
         *  if EmissiveColor is present, gets multiplied
         */
        EmissiveMask,

        /**
         *  BlendMask describes whether rendering fragment should be discarded and not rendered
         *
         *  float - [0, 1]
         *
         *  if value is 0, fragment is discarded
         */
        BlendMask,

        /**
         *  MetallicTexture describes metalness of the surface from texture
         *
         *  float - [0, 1]
         *
         *  gets priority over Metallic
         */
        MetallicTexture,

        /**
         *  RoughnessTexture describes roughness of the surface from texture
         *
         *  float - [0, 1]
         *
         *  gets priority over Roughness
         */
        RoughnessTexture,

        /**
         *  AmbientOcclusionTexture describes ambient shadowing of the surface from texture
         *
         *  float - [0, 1]
         */
        AmbientOcclusionTexture,

        /**
         *  ORM - Occlusion, Roughness, Metallic
         *
         *  vec3 - [0, 1]
         *
         *  describes 3 properties in one backed texture
         *
         *  used channels as follows:
         *
         *      Red: Ambient Occlusion
         *      Green: Roughness
         *      Blue: Metallic
         *
         *  gets highest priority over other properties
         */
        ORM,

        /**
         *  Metallic value of the surface
         *
         *  float - [0, 1]
         *
         *  lowest priority
         */
        Metallic,

        /**
         *  Roughness value of the surface
         *
         *  float - [0, 1]
         *
         *  lowest priority
         */
        Roughness,

        /**
         *  Index of Refraction of transparent surface
         *
         *  float - [1, n]
         *
         *  Used only when Blending is set to 'NOT' Opaque
         */
        IoR,

        /**
         *  Absorption factor of refractive objects
         *
         *  float - [1, n]
         *
         *  Used only when Blending is set to 'NOT' Opaque
         */
        Absorption,

        /**
         *  Thickness of the thin layer of refractive objects
         *
         *  float - [0, n]
         */
        MicroThickness,

        /**
         *  Thickness of the solid volume of refractive objects
         *
         *  float - [0, n]
         */
        Thickness,

        /**
         *  Reflectance describes the strength of reflections
         *
         *  float - [0, 1]
         */
        Reflectance,

        /**
         *  Transmission describes how transparent material is
         *
         *  float - [0, 1]
         *
         *  Used only when Blending is set to 'NOT' Opaque
         */
        Transmission,
    };
}