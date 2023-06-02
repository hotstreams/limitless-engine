#pragma once

#include <limitless/core/context_debug.hpp>
#include <glm/glm.hpp>

namespace Limitless {
    enum class QueryState {
        ActiveTexture = GL_ACTIVE_TEXTURE,
        ArrayBufferBinding = GL_ARRAY_BUFFER_BINDING,
        VertexArrayObject = GL_VERTEX_ARRAY_BINDING,
        ShaderStorageBufferBinding = GL_SHADER_STORAGE_BUFFER_BINDING,
        UniformBufferBinding = GL_UNIFORM_BUFFER_BINDING,
        Blend = GL_BLEND,
        BlendColor = GL_BLEND_COLOR,
        BlendDstAlpha = GL_BLEND_DST_ALPHA,
        BlendDstRGB = GL_BLEND_DST_RGB,
        BlendSrcAlpha = GL_BLEND_SRC_ALPHA,
        BlendSrcRGB = GL_BLEND_SRC_RGB,
        ClearColor = GL_COLOR_CLEAR_VALUE,
        CullFace = GL_CULL_FACE,
        CullFaceMode = GL_CULL_FACE_MODE,
        CurrentProgram = GL_CURRENT_PROGRAM,
        DepthFunc = GL_DEPTH_FUNC,
        DepthTest = GL_DEPTH_TEST,
        DepthMask = GL_DEPTH_WRITEMASK,
        DrawBuffer = GL_DRAW_BUFFER,
        DrawFramebufferBinding = GL_DRAW_FRAMEBUFFER_BINDING,
        ReadFramebufferBinding = GL_READ_FRAMEBUFFER_BINDING,
        ElementArrayBufferBinding = GL_ELEMENT_ARRAY_BUFFER_BINDING,
        LineWidth = GL_LINE_WIDTH,
        // TODO: this is limit
        // move to limits
//        Max3DTextureSize = GL_MAX_3D_TEXTURE_SIZE,
        MaxArrayTextureLayers = GL_MAX_ARRAY_TEXTURE_LAYERS,
        MaxCubeMapTextureSize = GL_MAX_CUBE_MAP_TEXTURE_SIZE,
        MaxDrawBuffers = GL_MAX_DRAW_BUFFERS,
        MaxElementsIndices = GL_MAX_ELEMENTS_INDICES,
        MaxElementsVertices = GL_MAX_ELEMENTS_VERTICES,
        MaxTextureSize = GL_MAX_TEXTURE_SIZE,
        MaxUniformBufferBindings = GL_MAX_UNIFORM_BUFFER_BINDINGS,
        MaxShaderStorageBufferBindings = GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS,
        MaxUniformBlockSize = GL_MAX_UNIFORM_BLOCK_SIZE,
        PointSize = GL_POINT_SIZE,
        ProgramPointSize = GL_PROGRAM_POINT_SIZE,
        ScissorTest = GL_SCISSOR_TEST,
        StencilTest = GL_STENCIL_TEST,
        TextureBinding2D = GL_TEXTURE_BINDING_2D,
        Viewport = GL_VIEWPORT,
        TessLevel = GL_MAX_TESS_GEN_LEVEL
    };

    class StateQuery {
    public:
        GLint geti(QueryState name);
        GLfloat getf(QueryState name);
        GLboolean getb(QueryState name);
        glm::vec4 get4f(QueryState name);
        glm::vec3 get3f(QueryState name);
        glm::uvec4 get4ui(QueryState name);
        GLint getiForIndex(QueryState name, GLuint index);

        GLint getTexi(GLenum target, GLenum pname);
    };
}