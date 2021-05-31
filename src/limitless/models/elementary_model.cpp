#include <limitless/models/elementary_model.hpp>

using namespace Limitless;

ElementaryModel::ElementaryModel(std::string name)
    : AbstractModel({}, std::move(name)) {
}
