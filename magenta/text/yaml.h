
#ifndef _magenta_text_yaml_
#define _magenta_text_yaml_

#include "magenta/text/classes.h"

#include "yaml-cpp/yaml.h"
#include "github.nlohmann.json/src/json.hpp"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

string to_s(const YAML::Node &y);
JSON to_json(const YAML::Node &y);

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_text_yaml_
