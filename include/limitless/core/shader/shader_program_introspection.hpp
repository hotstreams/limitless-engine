#pragma once

#include <limitless/core/buffer/indexed_buffer.hpp>

#include <vector>

namespace Limitless {
    class ShaderProgramIntrospection {
    private:
        static std::unordered_map<std::string, GLint> uniformLocations_withProgramInterfaceQuery(GLuint program_id);
    public:



        static std::unordered_map<std::string, GLint> getUniformLocations(GLuint program_id) {
            //TODO: check extension presents
            // or do in other way
            return uniformLocations_withProgramInterfaceQuery(program_id);
        }

        static std::vector<IndexedBufferData> getIndexedBufferBounds(GLuint program_id);
    };
}