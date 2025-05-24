
#include <iostream>
#include <fstream>
#include <strstream>
#include <iomanip>

#include "text/defs.h"

#include "yaml.h"
#include "json.h"

namespace magenta
{

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////

string to_s(const YAML::Node &y)
{
	strstream ss;
	ss << std::setw(4) << y << std::ends;
	return ss.str();
}

//---------------------------------------------------------------------------------------------

JSON to_json(const YAML::Node &y)
{
	JSON j;
	if (y.IsMap())
	{
		for (auto it = y.begin(); it != y.end(); ++it)
		{
			auto k = it->first.as<string>();
			auto v = it->second;
			j[k] = to_json(v);
		}
	}
	else if (y.IsSequence())
	{
		for (auto &e : y)
			j.push_back(to_json(e));
	}
	else if (y.IsScalar())
	{
		auto s = y.as<string>();
		auto n = ston(s);
		if (n.has_value())
		{
			if (n.type() == typeid(long))
				j = any_cast<long>(n);
			else if (n.type() == typeid(double))
				j = any_cast<double>(n);
			else
				j = s;
		}
		else if (s == "true" || s == "yes")
			j = true;
		else if (s == "false" || s == "no")
			j = false;
		else
		{
			j = s;
		}
	}
	else if (y.IsNull())
	{
		j = nullptr;
	}
	else
	{
		strstream ss;
		ss << y << std::ends;
		j = ss.str();
	}

	return j;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
