#pragma once

#include <limitless/core/context_debug.hpp>
#include <limitless/core/indexed_buffer.hpp>
#include <limitless/core/buffer.hpp>
#include <unordered_map>
#include <glm/glm.hpp>
#include <mutex>
#include <map>

namespace Limitless {
    struct BindingPoint {
        Buffer::Type target;
        GLuint point;
    };
    bool operator<(const BindingPoint& a, const BindingPoint& b) noexcept;

    enum class Clear {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT,
        ColorDepth = Color | Depth,
        ColorDepthStencil = ColorDepth | Stencil
    };

    enum class DepthFunc {
        Never = GL_NEVER,
        Less = GL_LESS,
        Equal = GL_EQUAL,
        Lequal = GL_LEQUAL,
        Greater = GL_GREATER,
        Notequal = GL_NOTEQUAL,
        Gequal = GL_GEQUAL,
        Always = GL_ALWAYS
    };

    enum class DepthMask {
        True = GL_TRUE,
        False = GL_FALSE
    };

    // enable or disable server-side GL capabilities
    enum class Capabilities {
        DepthTest = GL_DEPTH_TEST,
        Blending = GL_BLEND,
        ProgramPointSize = GL_PROGRAM_POINT_SIZE,
        ScissorTest = GL_SCISSOR_TEST,
        StencilTest = GL_STENCIL_TEST,
        CullFace = GL_CULL_FACE
    };

    enum class BlendFactor {
        None,
        Zero = GL_ZERO,
        One = GL_ONE,
        DstColor = GL_DST_COLOR,
        SrcColor = GL_SRC_COLOR,
        SrcAlpha = GL_SRC_ALPHA,
        DstAplha = GL_DST_ALPHA,
        OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
        BlendColor = GL_CONSTANT_COLOR
    };

    enum class CullFace {
        Front = GL_FRONT,
        Back = GL_BACK,
        FrontBack = GL_FRONT_AND_BACK
    };

    enum class FrontFace {
        CW = GL_CW,
        CCW = GL_CCW
    };

    enum class PolygonMode {
        Point = GL_POINT,
        Line = GL_LINE,
        Fill = GL_FILL,
    };

    enum class StencilOp {
    	Keep = GL_KEEP,
    	Zero = GL_ZERO,
    	Replace = GL_REPLACE,
    	Incr = GL_INCR,
    	IncrWrap = GL_INCR_WRAP,
    	Decr = GL_DECR,
    	DecrWrap = GL_DECR_WRAP,
    	Invert = GL_INVERT
    };

    enum class StencilFunc {
    	Never = GL_NEVER,
    	Less = GL_LESS,
    	Lequal = GL_LEQUAL,
    	Greater = GL_GREATER,
    	Gequal = GL_GEQUAL,
    	Nequal = GL_NOTEQUAL,
    	Always = GL_ALWAYS
    };

    enum class PixelStore {
        PackAlignment = GL_PACK_ALIGNMENT,
        UnpackAlignment = GL_UNPACK_ALIGNMENT
    };

    class Context;

    class ContextState {
    protected:
        std::unordered_map<Capabilities, bool> capability_map;
        glm::uvec2 viewport {};
        glm::vec4 clear_color {};

        std::array<StencilOp, 3> stencil_op {StencilOp::Keep};
        int32_t stencil_mask {0xFF};
        StencilFunc stencil_func {StencilFunc::Always};
        int32_t stencil_func_ref {0};
        int32_t stencil_func_mask {0xFF};

        DepthFunc depth_func {DepthFunc::Less};
        DepthMask depth_mask {DepthMask::True};
        CullFace cull_face {CullFace::Back};
        FrontFace front_face {FrontFace::CCW};
        CullFace polygon_face {CullFace::FrontBack};
        PolygonMode polygon_mode {PolygonMode::Fill};
        BlendFactor src_factor {BlendFactor::None}, dst_factor {BlendFactor::Zero};
        glm::vec4 blending_color {0.0f};
        glm::uvec2 scissor_origin {};
        glm::uvec2 scissor_size {};
        float line_width {};

        GLuint shader_id {};
        GLuint vertex_array_id {};
        GLuint framebuffer_id {};

        // contains [target, last buffer id]
        std::unordered_map<Buffer::Type, GLuint> buffer_target;
        // contains [binding point, last_buffer_id]
        std::map<BindingPoint, GLuint> buffer_point;

        IndexedBuffer indexed_buffers;

        GLuint active_texture {};
        // contains [texture_image_unit, texture_id]
        std::map<GLuint, GLuint> texture_bound;
        // contains [texture_handle, resident]
        std::map<GLuint64, bool> texture_resident;

        // pixel store
        PixelStore pixel_pack {};
        GLint pixel_param {};

        ContextState() = default;
        void init() noexcept;

        static inline std::unordered_map<GLFWwindow*, ContextState*> state_map;
        static inline std::mutex mutex;
        void swapStateMap(Context& lhs, Context& rhs) noexcept;
        void unregisterState(GLFWwindow* window) noexcept;
        void registerState(GLFWwindow* window) noexcept;

        friend class StateBuffer;
        friend class NamedBuffer;
        friend class ShaderProgram;
        friend class VertexArray;
        friend class StateTexture;
        friend class NamedTexture;
        friend class BindlessTexture;
        friend class TextureBinder;
        friend class Framebuffer;
        friend class DefaultFramebuffer;
    public:
        virtual ~ContextState() = default;

        ContextState(const ContextState&) = delete;
        ContextState& operator=(const ContextState&) = delete;

        ContextState(ContextState&&) noexcept = default;
        ContextState& operator=(ContextState&&) noexcept = default;

        static bool hasState(GLFWwindow* window) noexcept;
        static ContextState* getState(GLFWwindow* window) noexcept;

        void setViewPort(glm::uvec2 size) noexcept;
        void setViewPort(glm::uvec2 position, glm::uvec2 size) noexcept;
        void clearColor(const glm::vec4& color) noexcept;
        void setDepthFunc(DepthFunc func) noexcept;
        void setDepthMask(DepthMask mask) noexcept;
        void setCullFace(CullFace mode) noexcept;
        void setFrontFace(FrontFace mode) noexcept;
        void setPolygonMode(CullFace face, PolygonMode mode) noexcept;
        void setBlendFunc(BlendFactor src, BlendFactor dst) noexcept;
        void setBlendColor(const glm::vec4& color) noexcept;
        void setScissorTest(glm::uvec2 origin, glm::uvec2 size) noexcept;
        void clear(Clear bits) noexcept;
        void setLineWidth(float width) noexcept;
        void disable(Capabilities func) noexcept;
        void enable(Capabilities func) noexcept;
        void setStencilFunc(StencilFunc func, int32_t ref, int32_t mask) noexcept;
        void setStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass) noexcept;
        void setStencilMask(int32_t mask) noexcept;
        void setPixelStore(PixelStore name, GLint param) noexcept;

        auto& getIndexedBuffers() noexcept { return indexed_buffers; }

        const auto& getViewPort() const noexcept { return viewport; }
        const auto& getClearColor() const noexcept { return clear_color; }
        const auto& getDepthFunc() const noexcept { return depth_func; }
        const auto& getDepthMask() const noexcept { return depth_mask; }
        const auto& getCullFace() const noexcept { return cull_face; }
        const auto& getFrontFace() const noexcept { return front_face; }
        const auto& getPolygonMode() const noexcept { return polygon_mode; }
        auto getBlendFunc() const noexcept { return std::pair{src_factor, dst_factor}; }
        const auto& getBlendColor() const noexcept { return blending_color; }
        auto getScissorTest() const noexcept { return std::pair{scissor_origin, scissor_size}; }
        const auto& getCapabilities() const noexcept { return capability_map; }
        const auto& getBufferTargets() const noexcept { return buffer_target; }

        auto getActiveTexture() const noexcept { return active_texture; }
        const auto& getTextureBound() const noexcept { return texture_bound; }

        auto getShaderId() const noexcept { return shader_id; }
        auto getVertexArrayId() const noexcept { return vertex_array_id; }
    };
}
