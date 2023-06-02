#include <gtest/gtest.h>

#include <limitless/core/context.hpp>
#include <limitless/core/state_buffer.hpp>
#include <limitless/core/state_query.hpp>

TEST(BufferTest, StateBufferFunctions) {
    using namespace Limitless;

    Context context {"test context", {1, 1}, {{WindowHint::Visible, false}}};

    // bind, bindAs, bindBase, bindBaseAs works as expected
    // * state is right when its used
    // * state is right when move constructor is invoked
    // * state is right when buffer was resized
    // * state is right when buffer was cloned
    // * state is right when buffer was deleted
    // * and map method returned success

    // test every above scenario with complicated state (binding points in particular)
    {
        const auto BUFFER_SIZE = 1024;
        StateBuffer buffer {Buffer::Type::Array, BUFFER_SIZE, nullptr, Buffer::Usage::StaticDraw,  Buffer::MutableAccess::None};
        StateQuery query;

        buffer.bind();

        ASSERT_EQ(buffer.getId(), context.getBufferTargets().at(Buffer::Type::Array));
        ASSERT_EQ(context.getBufferTargets().at(Buffer::Type::Array), query.geti(QueryState::ArrayBufferBinding));
    }

    {
        const auto BUFFER_SIZE = 1024;
        StateBuffer buffer {Buffer::Type::Array, BUFFER_SIZE, nullptr, Buffer::Storage::Static,  Buffer::ImmutableAccess::None};
        StateQuery query;

        buffer.bind();
        buffer.resize(2048);
        buffer.bind();

        ASSERT_EQ(buffer.getId(), context.getBufferTargets().at(Buffer::Type::Array));
        ASSERT_EQ(context.getBufferTargets().at(Buffer::Type::Array), query.geti(QueryState::ArrayBufferBinding));
    }
}