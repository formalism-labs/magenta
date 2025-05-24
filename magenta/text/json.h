
#ifndef _magenta_text_json_
#define _magenta_text_json_

#include <string>

#include "magenta/text/classes.h"

#include "github.nlohmann.json/src/json.hpp"
#include "yaml-cpp/yaml.h"

namespace magenta
{

using JSON = nlohmann::json;

///////////////////////////////////////////////////////////////////////////////////////////////

std::string to_s(const JSON &j);
YAML::Node to_yaml(const JSON &j);

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_text_json_
