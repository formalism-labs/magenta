
#ifndef _magenta_lang_cxx_attributes_
#define _magenta_lang_cxx_attributes_

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

#define MAGENTA_CXX_SIMPLE_ATTRIBUTE(type, name, capname) \
	protected: \
		type _##name; \
	public: \
		virtual type name() const { return _##name; } \
		virtual void set##capname(type x) { _##name = x; }

//---------------------------------------------------------------------------------------------

#define MAGENTA_CXX_SIMPLE_CONST_ATTRIBUTE(type, name, capname) \
	protected: \
		type _##name; \
		virtual void set##capname(type x) { _##name = x; } \
	public: \
		virtual type name() const { return _##name; }

///////////////////////////////////////////////////////////////////////////////////////////////

#define MAGENTA_CXX_ATTRIBUTE(type, name) \
	protected: \
		type _##name; \
	public: \
		virtual const type &name() const { return _##name; } \
		virtual type &name() { return _##name; }

//---------------------------------------------------------------------------------------------

#define MAGENTA_CXX_CONST_ATTRIBUTE(type, name) \
	protected: \
		type __##name; \
		virtual type &_##name() { return __##name; } \
	public: \
		virtual const type &name() const { return __##name; }

///////////////////////////////////////////////////////////////////////////////////////////////

#define MAGENTA_CXX_REF_ATTRIBUTE(type, name, capname) \
	protected: \
		type _##name; \
	public: \
		virtual const type name() const { return _##name; } \
		virtual type name() { return _##name; } \
		virtual void set##capname(type x) { _##name = x; }

//---------------------------------------------------------------------------------------------

#define MAGENTA_CXX_CONST_REF_ATTRIBUTE(type, name, capname) \
	protected: \
		const type _##name; \
	public: \
		virtual const type name() const { return _##name; } \
		virtual void set##capname(const type x) { _##name = x; }

//---------------------------------------------------------------------------------------------

#define MAGENTA_CXX_REF_CONST_ATTRIBUTE(type, name, capname) \
	protected: \
		type _##name; \
		virtual type name() { return _##name; } \
		virtual void set##capname(const type x) { _##name = x; } \
	public: \
		virtual const type name() const { return _##name; }

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_lang_cxx_attributes_
