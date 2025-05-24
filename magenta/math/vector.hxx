
namespace magenta_math_1
{

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
Vector<T>::Vector() 
{
	d = 0; 
	v = 0;
}

//---------------------------------------------------------------------------------------------

template <class T>
Vector<T>::Vector(int n) 
{ 
	if (n<0) 
		 throw "vector: negative dimension"; 
	d = n; 
	v = 0;
	if (!d)
		return;
	v = new T[d];
	for(int i=0; i<d; i++) 
		v[i] = 0;
}

//---------------------------------------------------------------------------------------------

template <class T>
Vector<T>::Vector(const Vector &p)
{
	d = p.d; 
	v = 0;
	if (!d)
		return;
	v = new T[d];
	for(int i=0; i<d; i++) 
		v[i] = p.v[i];
}

//---------------------------------------------------------------------------------------------

template <class T>
Vector<T>::Vector(const STLVector &w)
{
	d = w.size();
	v = new T[d];
	for (int i = 0; i < d; ++i)
		v[i] = w[i];
}

//---------------------------------------------------------------------------------------------

template <class T>
Vector<T>::Vector(T x, T y) 
{ 
	v = new T[2];
	d = 2;
	v[0] = x;
	v[1] = y;
}

//---------------------------------------------------------------------------------------------

template <class T>
Vector<T>::Vector(T x, T y, T z) 
{
	v = new T[3];
	d = 3;
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

//---------------------------------------------------------------------------------------------

template <class T>
Vector<T>::~Vector() 
{
	if (v) 
		delete[] v; 
}

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void Vector<T>::check_dimensions(const Vector& vec) const
{
	if (d!=vec.d)
		throw "vector arguments have different dimensions";
 }

//---------------------------------------------------------------------------------------------

template <class T>
T  Vector<T>::coord(int i) const
{ 
	if (i<0 || i>=d)  
		throw "vector: index out of range";
	return v[i]; 
}

template <class T>
T& Vector<T>::coord(int i)
{ 
	if (i<0 || i>=d)  
		throw "vector: index out of range";
	return v[i]; 
}

//---------------------------------------------------------------------------------------------

template <class T>
Vector<T>& Vector<T>::operator=(const Vector& vec)
{ 
	if (d != vec.d)
	{ 
		if (v)
		delete[] v;

		d = vec.d;
		if (d > 0)
			v = new T[d];
		else
			v = 0;
	}

	for(int i=0; i<d; i++) 
		v[i] = vec.v[i];

	return *this;
}

//---------------------------------------------------------------------------------------------

template <class T>
bool Vector<T>::operator==(const Vector& vec)  const
{ 
	if (vec.d != d) 
		return false;
	int i = 0;
	while ((i<d) && (v[i]==vec.v[i])) 
		i++;
	return i==d;
 }

///////////////////////////////////////////////////////////////////////////////////////////////

/*
template <class T>
void Vector<T>::read(istream& is) 
{ 
	for(int i=0;i<d;i++) is  >> v[i]; 
}

//---------------------------------------------------------------------------------------------

void Vector<T>::print(ostream& os) 
{ 
	os << "(";
	for(int i=0;i<d;i++) 
		os << string(" %6.2f",v[i]);
	os << ")";
}

//---------------------------------------------------------------------------------------------

template <class T>
ostream& operator<<(ostream& os, const Vector& v)
{ 
	os << v.d << " ";
	for (int i=0;i<v.d;i++) 
		os << " " << v[i];
  return os;
}

//---------------------------------------------------------------------------------------------

template <class T>
istream& operator>>(istream& is, Vector& v) 
{
	int d;
	is >> d;
		Vector w(d);
	for (int i=0;i<d;i++) 
		is >> w[i];
	v = w;
	return is; 
} 

*/

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta_math_1
