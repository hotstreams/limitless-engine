#pragma once

namespace Limitless {
    /**
     * Describes OpenGL Uniform Type
     */
    enum class UniformType {
        /**
        * Type for value
        */
        Value,

        /**
         * Type for texture sampler
         *
         * TODO: Add different sampler types + explicit image samplers with extension
         */
        Sampler,

        /**
         * Type for time
         *
         * It is different from value-float because it should be updated on each invocation
         */
        Time
    };
}
