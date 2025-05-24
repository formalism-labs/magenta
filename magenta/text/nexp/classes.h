
#if defined(WIN32) && defined(MAGENTA_DLL)
#	ifdef IMPLEMENT_MAGENTA_TEXT_NESTEXP_2
#		define _class class __declspec(dllexport)
#	else
#		define _class class __declspec(dllimport)
#	endif
#else
#	define _class class
#endif

namespace magenta
{

_class Atom;
_class Node;

_class Nodes;
namespace _Nodes_
{
	_class Iterator;
}

_class Nexp;
	
}

#undef _class