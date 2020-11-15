#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <core/texture_visitor.hpp>
#include <core/context_debug.hpp>
#include <util/bytewriter.hpp>
#include <util/bytereader.hpp>
#include <util/serializer.hpp>
#include <glm/glm.hpp>
#include <assets.hpp>
#include <core/texture.hpp>
#include <texture_loader.hpp>

namespace GraphicsEngine {
    class Texture;
    class ShaderProgram;
    class Uniform;

    enum class UniformType { Value, Sampler, Time };
    enum class UniformValueType { Invalid, Float, Int, Uint, Vec2, Vec3, Vec4, Mat3, Mat4 };

	template <typename Reader>
	std::unique_ptr<Uniform> deserializeUniformValue(Reader&& reader);

    class Uniform {
    protected:
        std::string name;
        UniformType type;
        UniformValueType value_type;
        bool changed;

        virtual ByteWriter& serialize(ByteWriter& writer) const {
        	return writer << name << type << value_type;
        }
    public:
    	explicit Uniform(ByteReader& reader) : changed{false} {
    		reader >> name >> type >> value_type;
    	}

        Uniform(std::string name, UniformType type, UniformValueType value_type) noexcept;
        virtual ~Uniform() = default;

        [[nodiscard]] auto getType() const noexcept { return type; }
        [[nodiscard]] auto getValueType() const noexcept { return value_type; }
        [[nodiscard]] const auto& getName() const noexcept { return name; }
        [[nodiscard]] virtual bool& getChanged() noexcept { return changed; }

        [[nodiscard]] virtual Uniform* clone() noexcept = 0;
        virtual void set(const ShaderProgram& shader) = 0;

        friend ByteWriter& operator<<(ByteWriter& writer, const Uniform& u) {
            return u.serialize(writer);
        }
    };

    template<typename T>
    class UniformValue : public Uniform {
    protected:
        T value;

        [[nodiscard]] constexpr UniformValueType getUniformValueType() const;
        UniformValue(const std::string& name, UniformType type, const T& value);

        ByteWriter& serializeFields(ByteWriter& writer) const;
        ByteWriter& serialize(ByteWriter& writer) const override;
    public:
    	explicit UniformValue(ByteReader& reader);

        UniformValue(const std::string& name, const T& value) noexcept;
        ~UniformValue() override = default;

        [[nodiscard]] const auto& getValue() const noexcept { return value; }
        void setValue(const T& val) noexcept;

        [[nodiscard]] UniformValue* clone() noexcept override;
        void set(const ShaderProgram& shader) override;
    };

    class UniformSampler : public UniformValue<int> {
    private:
        std::shared_ptr<Texture> sampler;
        GLuint sampler_id {};
    protected:
    	ByteWriter& serialize(ByteWriter& writer) const override;
    public:
        explicit UniformSampler(ByteReader& reader);

        UniformSampler(const std::string& name, std::shared_ptr<Texture> sampler) noexcept;
        ~UniformSampler() override = default;

        [[nodiscard]] const auto& getSampler() const noexcept { return sampler; }
        void setSampler(const std::shared_ptr<Texture>& texture) noexcept;

        [[nodiscard]] UniformSampler* clone() noexcept override;
        void set(const ShaderProgram& shader) override;
    };

    class UniformTime : public UniformValue<float> {
    private:
        std::chrono::time_point<std::chrono::steady_clock> start;
    protected:
	    ByteWriter& serialize(ByteWriter& writer) const override {
	    	writer << UniformType::Time;
	    	UniformValue::serializeFields(writer);
	    	return writer;
	    }
    public:
    	explicit UniformTime(ByteReader& reader) : UniformValue{reader} {}

        explicit UniformTime(const std::string& name) noexcept;
        ~UniformTime() override = default;

        void update() noexcept;

        [[nodiscard]] bool& getChanged() noexcept override { changed = true; return changed; }

        [[nodiscard]] UniformTime* clone() noexcept override;
        void set(const ShaderProgram& shader) override;
    };

    std::string getUniformDeclaration(const Uniform& uniform) noexcept;

	std::unique_ptr<Uniform> deserializeUniformValue(ByteReader& reader);

	std::unique_ptr<Uniform> deserializeUniform(ByteReader& reader);
}
