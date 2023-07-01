#include <limitless/log.hpp>

using namespace Limitless;

void Log::init() {
    unlink("log.txt");

    plog::init(plog::info, &console_log).addAppender(&file_log);
}

plog::Record& plog::operator<<(Record& stream, const glm::uvec2& v)
{
    return stream << "(" << v.x << ", " << v.y << ")";
}

plog::Record& plog::operator<<(Record& stream, const glm::vec2& v)
{
    return stream << "(" << v.x << ", " << v.y << ")";
}

plog::Record& plog::operator<<(Record& stream, const glm::vec3& v)
{
    return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

plog::Record& plog::operator<<(Record& stream, const glm::vec4& v)
{
    return stream << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}

plog::Record& plog::operator<<(Record& stream, const glm::mat4& m)
{
    return stream << "[" << m[0][0] << ", " << m[1][0] << ", " << m[2][0] << ", " << m[3][0] << ", " << std::endl
                  << " " << m[0][1] << ", " << m[1][1] << ", " << m[2][1] << ", " << m[3][1] << ", " << std::endl
                  << " " << m[0][2] << ", " << m[1][2] << ", " << m[2][2] << ", " << m[3][2] << ", " << std::endl
                  << " " << m[0][3] << ", " << m[1][3] << ", " << m[2][3] << ", " << m[3][3] << ", " << std::endl;
}
