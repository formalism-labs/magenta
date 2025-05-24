
namespace magenta_math_1
{

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
NumericVector<T> NumericVector<T>::rotate90() const
{
	if (d !=2)
		throw "NumericVector::rotate90: dimension must be two";
	return This(-v[1],v[0]);
}

template <class T>
NumericVector<T> NumericVector<T>::rotate(double fi) const
{
	if (d !=2)
		throw "NumericVector::rotate: dimension must be two";
	double sinfi = sin(fi);
	double cosfi = cos(fi);
	return NumericVector<T>(T(v[0]*cosfi-v[1]*sinfi), T(v[0]*sinfi+v[1]*cosfi));
}

template <class T>
NumericVector<T>& NumericVector<T>::operator+=(const This &vec)
{
	check_dimensions(vec);
	int n = d;
	while (n--)
		v[n] += vec.v[n];
	return *this;
}

template <class T>
NumericVector<T>& NumericVector<T>::operator-=(const This &vec)
{ 
	check_dimensions(vec);
	int n = d;
	while (n--) 
		v[n] -= vec.v[n];
	return *this;
}

template <class T>
NumericVector<T> NumericVector<T>::operator+(const This& vec) const
{ 
	check_dimensions(vec);
	int n = d;
	This result(n);
	while (n--) 
		result.v[n] = v[n]+vec.v[n];
	return result;
}

template <class T>
NumericVector<T> NumericVector<T>::operator-(const This& vec) const
{ 
	check_dimensions(vec);
	int n = d;
	This result(n);
	while (n--) 
		result.v[n] = v[n]-vec.v[n];
	return result;
}

template <class T>
NumericVector<T> NumericVector<T>::operator-() const  // unary minus
{ 
	int n = d;
	This result(n);
	while (n--) 
		result.v[n] = -v[n];
	return result;
}


template <class T>
NumericVector<T> NumericVector<T>::operator*(T x) const
{ 
	int n = d;
	This result(n);
	while (n--) 
		result.v[n] = v[n] * x;
	return result;
}

template <class T>
NumericVector<T> NumericVector<T>::operator/(T x) const
{ 
	int n = d;
	This result(n);
	while (n--) 
		result.v[n] = v[n] / x;
	return result;
}

template <class T>
T NumericVector<T>::operator*(const This& vec) const
{ 
	check_dimensions(vec);
	T result=0;
	int n = d;
	while (n--) 
		result += v[n]*vec.v[n];
	return result;
}

template <class T>
T NumericVector<T>::sqr_length() const { return *this * *this; }

template <class T>
T NumericVector<T>::length() const { return sqrt(sqr_length()); }

template <class T>
T NumericVector<T>::angle(const This &y)  const
{ 
	const This& x = *this;

	double L = x.length() * y.length();

	if (L == 0)
		throw "angle: zero argument";

	double cosin = (x*y)/L;

	if (cosin < -1) cosin = -1;
	if (cosin >  1) cosin =  1;

	return acos(cosin);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta_math_1
