
#include <iostream>
#include <fstream>
#include <strstream>

#include "text/defs.h"

#include "json.h"

namespace magenta
{

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

string to_s(const JSON &j)
{
	strstream ss;
	ss << j.dump(4) << std::ends;
	return ss.str();
}

//---------------------------------------------------------------------------------------------

YAML::Node to_yaml(const JSON &j)
{
	YAML::Node y;
	if (j.is_object())
	{
		for (auto it = j.begin(); it != j.end(); ++it)
		{
			auto k = it.key();
			auto v = it.value();
			y[k] = to_yaml(v);
		}
	}
	else if (j.is_array())
	{
		for (auto &e : j)
			y.push_back(to_yaml(e));
	}
	else if (j.is_string())
	{
		y = j.get<string>();
	}
	else if (j.is_boolean())
	{
		y = j.get<bool>() ? "yes" : "no";
	}
	else if (j.is_null())
	{
		y = YAML::Null;
	}
	else
	{
		strstream ss;
		ss << j << std::ends;
		y = ss.str();
	}
	return y;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
