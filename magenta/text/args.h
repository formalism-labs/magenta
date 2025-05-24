
#ifndef _magenta_lang_args_
#define _magenta_lang_args_

#include <list>
#include <string>

#include "magenta/text/defs.h"

namespace magenta
{

using magenta::text;

///////////////////////////////////////////////////////////////////////////////////////////////

class Arguments
{
	typedef Arguments This;

protected:
	std::list<text> _args;

public:
	template <class It, class Args, class Base>
	class AbstractIterator
	{
		friend class Arguments;

		Args &_args;
		Base _iter;

	protected:
		AbstractIterator(Args &args, const Base &j) : _args(args), _iter(j) {}

	public:
		AbstractIterator(Args &args) : _args(args), _iter(args._args.begin()) {}
		AbstractIterator(const AbstractIterator &i) : _args(i._args), _iter(i._iter) {}

		AbstractIterator &operator++() { _iter.operator++(); return *this; }
		It operator++(int) { It i = *this; _iter.operator++(); return i; }
		AbstractIterator &operator--() { _iter.operator--(); return *this; }
		It operator--(int) { It i = *this; _iter.operator--(); return i; }

		bool operator!() const { return _iter == _args._args.end(); }

		class ConstIterator at_offset(int n) const;
		class Iterator at_offset(int n);

		It at(int n) const { return at_offset(n); }
		It operator+(int n) const { return at_offset(n); }
		It operator-(int n) const { return at_offset(-n); }

		int position() const { return std::distance(_args._args.begin(), _iter); }
		int operator-(const It &it) const { return std::distance(it._iter, _iter); }
	};

	class Iterator : public AbstractIterator<Iterator, Arguments, std::list<text>::iterator>
	{
		friend class Arguments;
		typedef AbstractIterator<Iterator, Arguments, std::list<text>::iterator> Super;

	protected:
		Iterator(Arguments &args, const std::list<text>::iterator &j) : Super(args, j) {}

	public:
		Iterator(Arguments &args) : Super(args, args._args.begin()) {}
		Iterator(const Iterator &i) : Super(i._args, i._iter) {}

		Iterator &operator++() { Super::operator++(); return *this; }
		Iterator &operator--() { Super::operator--(); return *this; }

		text &operator[](int i) { return *at_offset(i); }
		text *operator->() { return &*_iter; }
		text &operator*() { return *_iter; }

		Iterator at_offset(int n);

		void remove(int n = 1);
	};

	class ConstIterator : public AbstractIterator<ConstIterator, const Arguments, std::list<text>::const_iterator>
	{
		friend class Arguments;
		typedef AbstractIterator<ConstIterator, const Arguments, std::list<text>::const_iterator> Super;
			
	protected:
		ConstIterator(const Arguments &args, const std::list<text>::const_iterator &j) : Super(args, j) {}

	public:
		ConstIterator(const Arguments &args) : Super(args, args._args.begin()) {}
		ConstIterator(const ConstIterator &i) : Super(i._args, i._iter) {}
		ConstIterator(const Iterator &i) : Super(i._args, i._iter) {}

		ConstIterator &operator++() { Super::operator++(); return *this; }
		ConstIterator &operator--() { Super::operator--(); return *this; }

		const text &operator[](int i) const { return *at_offset(i); }
		const text *operator->() const { return &*_iter; }
		const text &operator*() const { return *_iter; }

		ConstIterator at_offset(int n) const;
	};

	typedef Iterator I;
	typedef ConstIterator CI;

protected:
	Arguments(std::list<text> &args) : _args(args) {}

public:
	Arguments() {}
	Arguments(int argc, char *argv[]) { append(argc, argv); }

	text &at(int i);
	const text &at(int i) const { return const_cast<Arguments*>(this)->at(i); }
	text &operator[](int i) { return at(i); }
	const text &operator[](int i) const { return at(i); }

	Iterator begin() { return Iterator(*this, _args.begin()); }
	ConstIterator begin() const { return ConstIterator(*this, _args.begin()); }

	Iterator end() { return Iterator(*this, _args.end()); }
	ConstIterator end() const { return ConstIterator(*this, _args.end()); }

	void append(int argc, char *argv[]);
	virtual void append(const text &s);
	This &operator+=(const text &s) { append(s); return *this; }
	This &operator<<(const text &s) { append(s); return *this; }

	text toString() const;
	operator text() const { return toString(); }

	size_t count() const { return _args.size(); }

	Iterator find(const text &opt, bool strict = true);
	Iterator find(const text &opt, Iterator i, bool strict = true);
	ConstIterator find(const text &opt, bool strict = true) const;
	ConstIterator find(const text &opt, ConstIterator i, bool strict = true) const;

	Arguments pop_front(size_t n = 1);
	Arguments pop_back(size_t n = 1);

	char **argv() const;
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_lang_args_
