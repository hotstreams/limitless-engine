#pragma once

#include <limitless/core/buffer/buffer_binding_point.hpp>
#include <limitless/core/buffer/indexed_buffer.hpp>
#include <limitless/core/capabilities.hpp>
#include <limitless/core/polygon_mode.hpp>
#include <limitless/core/pixel_store.hpp>
#include <limitless/core/stencil_op.hpp>
#include <limitless/core/depth_func.hpp>
#include <limitless/core/blending.hpp>
#include <limitless/core/cullface.hpp>
#include <limitless/core/buffer/buffer.hpp>
#include <limitless/core/clear.hpp>

#include <unordered_map>
#include <glm/glm.hpp>
#include <mutex>
#include <map>
#include <array>

namespace Limitless {
    class Context;

    /**
     * ContextState class store currently bound OpenGL state
     *
     * It is primarily used for caching purposes to reduce command count submitted to driver
     * Also it is used for optimizing object binding process
     */

    class ContextState {
    protected:
        /**
         * Contains whether some capability is enabled
         */
        std::unordered_map<Capabilities, bool> capability_map;

        /**
         * Contains <target>:<last bound buffer id>
         *
         * Specifies which buffer is currently bound to which not indexed buffer target
         *
         * Some buffers binding target has only 1 slot. If you bind new buffer, previous link gets broken
         * Other buffers are indexed, it means that you can bind more then 1 buffer to this target at specified index
         */
        std::unordered_map<Buffer::Type, GLuint> buffer_target;

        /**
         * Contains <binding point>:<last bound buffer id>
         *
         * Specifies which buffer is currently bound to which indexed buffer target and index
         *
         * BindingPoint contains <target> and <index> as key for identifying slot
         *
         * Each GPU has different number of binding points. Check StateLimits class
         */
        std::map<BindingPoint, GLuint> buffer_point;

        /**
         * Contains <texture_unit, last bound texture id>
         *
         * Specifies which texture is currently bound to which texture unit
         *
         * Each GPU has different number of texture unit slots. Check StateLimits class
         */
        std::map<GLuint, GLuint> texture_bound;

        /**
         * Currently active texture unit
         *
         * Used internally by glBindTexture function
         */
        GLuint active_texture {};

        /**
         * Contains <texture_handle, resident>
         *
         * Specifies whether texture handle is resident
         *
         * Used by bindless texture extension
         */
        std::map<GLuint64, bool> texture_resident;

        /**
         * Viewport transformation
         *
         * uvec.xy is lower-left corner origin
         * uvec.zw are width and height
         */
        glm::uvec4 viewport {};

        /**
         * Clear color used by glClear()
         */
        glm::vec4 clear_color {};

        /**
         * Blend color used in 'per-sample processing blending' pipeline stage
         */
        glm::vec4 blend_color {0.0f};

        /**
         * Stencil operations
         */
        std::array<StencilOp, 3> stencil_op {StencilOp::Keep};

        /**
         * Stencil mask
         */
        int32_t stencil_mask {0xFF};

        /**
         * Stencil function
         */
        StencilFunc stencil_func {StencilFunc::Always};

        /**
         * Stencil func ref
         */
        int32_t stencil_func_ref {0};

        /**
         * Stencil func mask
         */
        int32_t stencil_func_mask {0xFF};

        /**
         * Depth function
         */
        DepthFunc depth_func {DepthFunc::Less};

        /**
         * Depth mask
         */
        DepthMask depth_mask {DepthMask::True};

        /**
         * Cull face
         */
        CullFace cull_face {CullFace::Back};

        /**
         * Cull face culling order
         */
        FrontFace front_face {FrontFace::CCW};

        /**
         * Cull face side
         */
        CullFace polygon_face {CullFace::FrontBack};

        /**
         * Polygon mode
         */
        PolygonMode polygon_mode {PolygonMode::Fill};

        /**
         * Blending state
         */
        BlendFactor src_factor {BlendFactor::None};
        BlendFactor dst_factor {BlendFactor::Zero};

        /**
         * Scissor viewport transformation
         *
         * uvec.xy is origin
         * uvec.zw is size
         */
        glm::uvec4 scissor_viewport {};

        /**
         * Line width
         *
         * Highly specific by GPU. Better not to use
         */
        float line_width {1.0f};

        /**
         * Shader object id
         */
        GLuint shader_id {};

        /**
         * Vertex array object id
         */
        GLuint vertex_array_id {};

        /**
         * Framebuffer object id
         */
        GLuint framebuffer_id {};

        /**
         * Pixel store packing parameters
         */
        PixelStore pixel_pack {};
        GLint pixel_param {};

        /**
         * TODO: I dont remember that it is but looks pretty important!
         */
        IndexedBuffer indexed_buffers;

        /**
         * ContextState constructor
         *
         * Initializes current state to default values
         *
         * State is not fetched from GPU!
         */

        ContextState() = default;

        /**
         * Initializes available slots based on specific to GPU limits
         */
        void init() noexcept;

        /**
         * Current states of whole application
         *
         * Binds ContextState to GLFWWindow state
         *
         * Used to fetch current ContextState class pointer based on glfwGetCurrentState (this call is almost free)
         */
        static inline std::unordered_map<GLFWwindow*, ContextState*> state_map;
        /**
         * Mutex to protect concurrent map access
         */
        static inline std::mutex mutex;

        /**
         * Swaps ContextState pointers in the map to link to the right pointer of GLFWWindow state
         *
         * Used by move semantics
         */
        static void swap_state_map(Context& lhs, Context& rhs) noexcept;

        /**
         * Registers/Unregisters context in the static state map
         *
         * @param window - windows to register/unregister
         */
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
        void setViewPort(glm::uvec4 new_viewport) noexcept;
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
        const auto& getBlendColor() const noexcept { return blend_color; }
        auto getScissorTest() const noexcept { return scissor_viewport; }
        const auto& getCapabilities() const noexcept { return capability_map; }
        const auto& getBufferTargets() const noexcept { return buffer_target; }
        const auto& getLineWidth() const noexcept { return line_width; }
//        const auto& getPointSize() const noexcept { return point_size; }

        auto getActiveTexture() const noexcept { return active_texture; }
        const auto& getTextureBound() const noexcept { return texture_bound; }
        const auto& getBufferPoints() const noexcept { return buffer_point; }

        auto getShaderId() const noexcept { return shader_id; }
        auto getVertexArrayId() const noexcept { return vertex_array_id; }
    };
}
