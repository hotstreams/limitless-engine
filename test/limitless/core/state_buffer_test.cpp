#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"

#include <limitless/core/context.hpp>
#include <limitless/core/state_buffer.hpp>

using namespace Limitless;
using namespace LimitlessTest;

class BufferChecker {
public:
    static GLint getBufferUsage(const Buffer& buffer) {
        GLint data;
        glGetBufferParameteriv((GLenum)buffer.getType(), GL_BUFFER_USAGE, &data);
        return data;
    }

    static GLint getBufferStorageFlags(const Buffer& buffer) {
        GLint data;
        glGetBufferParameteriv((GLenum)buffer.getType(), GL_BUFFER_STORAGE_FLAGS, &data);
        return data;
    }

    static GLint getBufferSize(const Buffer& buffer) {
        GLint data;
        glGetBufferParameteriv((GLenum)buffer.getType(), GL_BUFFER_SIZE, &data);
        return data;
    }

    static GLint64 getBufferMappedOffset(const Buffer& buffer) {
        GLint64  data;
        glGetBufferParameteri64v((GLenum)buffer.getType(), GL_BUFFER_MAP_OFFSET, &data);
        return data;
    }

    static GLint64 getBufferMappedLength(const Buffer& buffer) {
        GLint64  data;
        glGetBufferParameteri64v((GLenum)buffer.getType(), GL_BUFFER_MAP_LENGTH, &data);
        return data;
    }

    static bool isBufferMapped(const Buffer& buffer) {
        GLint data;
        glGetBufferParameteriv((GLenum)buffer.getType(), GL_BUFFER_MAPPED, &data);
        return data;
    }

    static bool isBufferImmutable(const Buffer& buffer) {
        GLint data;
        glGetBufferParameteriv((GLenum)buffer.getType(), GL_BUFFER_IMMUTABLE_STORAGE, &data);
        return data;
    }

    static GLint getBufferAccessFlags(const Buffer& buffer) {
        GLint data;
        glGetBufferParameteriv((GLenum)buffer.getType(), GL_BUFFER_ACCESS_FLAGS, &data);
        return data;
    }

    static GLint getBufferAccess(const Buffer& buffer) {
        GLint data;
        glGetBufferParameteriv((GLenum)buffer.getType(), GL_BUFFER_ACCESS, &data);
        return data;
    }
};

TEST_CASE("Buffer mutable constructor") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Array, 1024, nullptr, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        REQUIRE(BufferChecker::getBufferUsage(buffer) == (GLint)std::get<Buffer::Usage>(buffer.getUsageFlags()));
        REQUIRE(BufferChecker::getBufferSize(buffer) == buffer.getSize());
        REQUIRE(BufferChecker::isBufferImmutable(buffer) == false);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer immutable constructor") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Array, 1024, nullptr, Buffer::Storage::Static, Buffer::ImmutableAccess::None);

        REQUIRE(BufferChecker::getBufferStorageFlags(buffer) == (GLint)std::get<Buffer::Storage>(buffer.getUsageFlags()));
        REQUIRE(BufferChecker::getBufferSize(buffer) == buffer.getSize());
        REQUIRE(BufferChecker::isBufferImmutable(buffer) == true);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer bind function changes state correctly") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Array, 1024, nullptr, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        buffer.bind();

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer bindAs function changes state correctly") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Array, 1024, nullptr, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        buffer.bindAs(Buffer::Type::Uniform);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer bindBase function changes state correctly") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        buffer.bindBase(1);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer bindBaseAs function changes state correctly") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        buffer.bindBase(1);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer bindBufferRange function changes state correctly") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        buffer.bindBufferRange(0, 512);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer bindBufferRangeAs function changes state correctly") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        buffer.bindBufferRangeAs(Buffer::Type::ShaderStorage, 0, 512);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer mutable mapData function changes state correctly") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Usage::StaticDraw, Buffer::MutableAccess::Write);

        unsigned char data[1024] = {1};
        buffer.mapData(data, 1024);

        REQUIRE(BufferChecker::isBufferMapped(buffer) == false);
        REQUIRE(BufferChecker::getBufferMappedOffset(buffer) == 0);

        check_opengl_state();
    }

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Usage::StaticDraw, Buffer::MutableAccess::WriteOrphaning);

        unsigned char data[1024] = {1};
        buffer.mapData(data, 1024);

        REQUIRE(BufferChecker::isBufferMapped(buffer) == false);
        REQUIRE(BufferChecker::getBufferMappedOffset(buffer) == 0);

        check_opengl_state();
    }

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Usage::StaticDraw, Buffer::MutableAccess::WriteUnsync);

        unsigned char data[1024] = {1};
        buffer.mapData(data, 1024);

        REQUIRE(BufferChecker::isBufferMapped(buffer) == false);
        REQUIRE(BufferChecker::getBufferMappedOffset(buffer) == 0);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer immutable mapData function changes state correctly") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        unsigned char data[1024] = {1};

        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, data, Buffer::Storage::Static, Buffer::ImmutableAccess::None);

        REQUIRE(BufferChecker::isBufferMapped(buffer) == false);

        check_opengl_state();
    }

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Storage::DynamicPersistenceWrite, Buffer::ImmutableAccess::WritePersistence);

        unsigned char data[1024] = {1};
        buffer.mapData(data, 1024);

        REQUIRE(BufferChecker::isBufferMapped(buffer) == true);
        REQUIRE(BufferChecker::getBufferMappedOffset(buffer) == 0);

        check_opengl_state();
    }

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Storage::DynamicCoherentWrite, Buffer::ImmutableAccess::WriteCoherent);

        unsigned char data[1024] = {1};
        buffer.mapData(data, 1024);

        REQUIRE(BufferChecker::isBufferMapped(buffer) == true);
        REQUIRE(BufferChecker::getBufferMappedOffset(buffer) == 0);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer bufferSubData function changes state correctly") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        buffer.bufferSubData(0, 10, nullptr);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer clone function changes state correctly for mutable buffer") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        unsigned char data[1024] = {42};
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, data, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        StateBuffer* copy = buffer.clone();

        REQUIRE(buffer.getId() != copy->getId());
        REQUIRE(buffer.getType() == copy->getType());
        REQUIRE(buffer.getSize() == copy->getSize());
        REQUIRE(buffer.getAccess() == copy->getAccess());
        REQUIRE(buffer.getUsageFlags() == copy->getUsageFlags());

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer clone function changes state correctly for immutable buffer") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        unsigned char data[1024] = {42};
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, data, Buffer::Storage::Static, Buffer::ImmutableAccess::None);

        StateBuffer* copy = buffer.clone();

        REQUIRE(buffer.getId() != copy->getId());
        REQUIRE(buffer.getType() == copy->getType());
        REQUIRE(buffer.getSize() == copy->getSize());
        REQUIRE(buffer.getAccess() == copy->getAccess());
        REQUIRE(buffer.getUsageFlags() == copy->getUsageFlags());

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer resize function changes state correctly for mutable buffer") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        unsigned char data[1024] = {42};
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, data, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        buffer.resize(2048);

        REQUIRE(buffer.getSize() == 2048);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer resize function changes state correctly for immutable buffer") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    {
        unsigned char data[1024] = {42};
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, data, Buffer::Storage::Static, Buffer::ImmutableAccess::None);

        buffer.resize(2048);

        REQUIRE(buffer.getSize() == 2048);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer resize function changes state correctly for immutable buffer with immutable buffers bound") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    StateBuffer fake1 = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Storage::Static, Buffer::ImmutableAccess::None);
    StateBuffer fake2 = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Storage::Static, Buffer::ImmutableAccess::None);

    fake1.bindBase(1);
    fake2.bindBaseAs(Buffer::Type::Uniform, 2);
    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Storage::Static, Buffer::ImmutableAccess::None);
        buffer.bindBase(3);
        fake1.bind();

        check_opengl_state();

        buffer.resize(2048);

        REQUIRE(buffer.getSize() == 2048);

        check_opengl_state();
    }

    check_opengl_state();
}

TEST_CASE("Buffer clone function changes state correctly for immutable buffer with immutable buffers bound") {
    Context context = {"Title", {512, 512}, {{WindowHint::Visible, false}}};

    StateBuffer fake1 = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Storage::Static, Buffer::ImmutableAccess::None);
    StateBuffer fake2 = StateBuffer(Buffer::Type::ShaderStorage, 1024, nullptr, Buffer::Storage::Static, Buffer::ImmutableAccess::None);

    fake1.bindBase(1);
    fake2.bindBaseAs(Buffer::Type::Uniform, 2);
    {
        StateBuffer buffer = StateBuffer(Buffer::Type::Uniform, 1024, nullptr, Buffer::Storage::Static, Buffer::ImmutableAccess::None);
        check_opengl_state();
        buffer.bindBase(3);
        fake1.bind();

        check_opengl_state();

        StateBuffer* copy = buffer.clone();

        REQUIRE(buffer.getId() != copy->getId());

        check_opengl_state();
    }

    check_opengl_state();
}