#pragma once

namespace Limitless::ms {
    /**
     *  Shading describes intrinsic properties of a surface
     *
     *  defines how lighting is computed
     */
    enum class Shading {
        /**
         *  Unlit model turns off lighting computations
         *
         *  Maybe used to render pre-lit models such as cubemaps, UI or emissive meshes
         */
        Unlit,

        /**
         *  Standard model
         *
         *  Used to describe metallic and non-metallic surfaces
         */
        Lit,

        /**
         *  Custom shading model defined by user
         */
        Custom,
    };
}