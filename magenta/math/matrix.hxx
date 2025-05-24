
namespace magenta_math_1
{

///////////////////////////////////////////////////////////////////////////////////////////////
// CConstMatrix
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void CConstMatrix<T>::check_dimensions(const This &mat) const
{ 
	if (d1 != mat.d1 || d2 != mat.d2)
		throw "incompatible matrix types.";
}

//---------------------------------------------------------------------------------------------

template <class T>
bool CConstMatrix<T>::operator==(const This &x) const
{ 
	if (d1 != x.d1 || d2 != x.d2) 
		return false;

	for (int i = 0; i < d1; i++)
		for (int j = 0; j < d2; j++)
			if (elem(i, j) != x.elem(i, j)) 
				return false;

	return true;
}

//---------------------------------------------------------------------------------------------

template <class T>
Vector<T> CConstMatrix<T>::row(int i)  const
{ 
#ifdef MAGENTA_MATH_CHECK_DIMENTIONS
	if (i < 0 || i >= d1) 
		throw "matrix: row index out of range";
#endif

	Vector<T> result(d2);
	int j = d2;
	while (j--) 
		result.v[j] = elem(i, j);
	return result;
}

//---------------------------------------------------------------------------------------------

template <class T>
Vector<T> CConstMatrix<T>::col(int i)  const
{ 
#ifdef MAGENTA_MATH_CHECK_DIMENTIONS
	if (i < 0 || i >= d2) 
		throw "matrix: col index out of range";
#endif

	Vector<T> result(d1);
	int j = d1;
	while (j--) 
		result.v[j] = elem(j,i);
	return result;
}

//---------------------------------------------------------------------------------------------

template <class T>
CConstMatrix<T>::operator Vector<T>() const
{ 
#ifdef MAGENTA_MATH_CHECK_DIMENTIONS
	if (d2 != 1) 
		throw "error: cannot make vector from matrix";
#endif

	return col(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T CConstMatrix<T>::operator()(int i, int j) const
{ 
#ifdef MAGENTA_MATH_CHECK_DIMENTIONS
	if (i<0 || i>=d1)  
		throw "matrix: row index out of range";
	if (j<0 || j>=d2)  
		throw "matrix: col index out of range";
#endif

  return elem(i,j);
}

///////////////////////////////////////////////////////////////////////////////////////////////
/*
template <class T>
void CConstMatrix<T>::trans(This &result) const
{ 
	for(int i = 0; i < d2; i++)
		for(int j = 0; j < d1; j++)
			result.setElem(i, j, elem(j,i));
	return result;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void CConstMatrix<T>::print(std::ostream &out) const
{
	using std::endl;
	for (int i = 0; i < d1; ++i)
	{ 
		out << "[";
		for(int j = 0; j < d2; j++) 
			out << (j ? "\t" : "") << elem(i,j);
		out << "]" << endl;
	}
}
     
//---------------------------------------------------------------------------------------------

template <class T>
void CConstMatrix<T>::write(std::ostream &out) const
{ 
	using std::endl;
	out << M.d1 << '\t' << M.d2 << endl;
	for(int i = 0; i < M.d1; i++)
	{ 
		for(int j = 0; j < M.d2; j++) 
			os << (j ? "\t" : "") << M(i,j);
		out << endl; 
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CMatrix
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T &CMatrix<T>::operator()(int i, int j)
{ 
#ifdef MAGENTA_MATH_CHECK_DIMENTIONS
	if (i < 0 || i >= d1)  
		throw "matrix: row index out of range";
	if (j < 0 || j >= d2)  
		throw "matrix: col index out of range";
#endif

	return elem(i,j);
}

//---------------------------------------------------------------------------------------------

template <class T>
void CConstMatrix<T>::read(std::istream &in)
{
	int dim1, dim2;
	in >> dim1 >> dim1;
	if (d1 != dim1 || d1 != dim2)
		throw "matrix: invalid dimensions";

	for (int i = 0; i < d1; ++i)
		for (int j = 0; j < d2; ++j)
		{
			T e;
			in >> e;
			setElem(i, j, e);
		}
}	

///////////////////////////////////////////////////////////////////////////////////////////////
// Matrix
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void Matrix<T>::ctor(int dim1, int dim2, const T *w)
{
#ifdef MAGENTA_MATH_CHECK_DIMENTIONS
	if (dim1 <= 0 || dim2 <= 0) 
		throw "Matrix: invalid dimension"; 
#endif

	d1 = dim1;
	d2 = dim2;
	int n = d1*d2;
	v = new T[n];
	if (!w)
		return;
	for (int i = 0; i < n; ++i) 
		v[i] = w[i]; 
}

//---------------------------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix(int dim1, int dim2)
{
	ctor(dim1, dim2);
}

//---------------------------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix(const Matrix& p)  
{ 
	ctor(p.d1, p.d2, p.v);
}

//---------------------------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix(int dim1, int dim2, const T *D)
{
	ctor(dim1, dim2, D);
}

//---------------------------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix(int dim1, int dim2, T D)
{ 
	ctor(dim1, dim2);
	int n = d1*d2;
	for(int i = 0; i < n; ++i) 
		v[i] = D;
}

//---------------------------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix(const Vector<T> &w)
{
	ctor(w.dim(), 1);
	for (int i = 0; i < d1; ++i)
		v[i] = w[i];
}

//---------------------------------------------------------------------------------------------

template <class T>
Matrix<T>::~Matrix()  
{ 
	delete[] v;
}

//---------------------------------------------------------------------------------------------

template <class T>
Matrix<T> &Matrix<T>::operator=(const Matrix& mat)
{ 
	if (d1 != mat.d1 || d2 != mat.d2)
	{ 
		delete[] v;
		ctor(mat.d1, mat.d2);
	}

	int n = d1*d2;
	for (int i = 0; i < n; ++i)
		v[i] = mat.v[i];

	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////

/*
template <class T>
void Matrix<T>::flip_rows(int i,int j)
{ 
	Vector* p = v[i];
	v[i] = v[j];
	v[j] = p;
}
*/

//---------------------------------------------------------------------------------------------

template <class T>
RefVector<T> Matrix<T>::row(int i)
{ 
#ifdef MAGENTA_MATH_CHECK_DIMENTIONS
	if (i<0 || i>=d1)  
		throw "matrix: row index out of range";
#endif
	return RefVector(&v[i * d2], d2);
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta_math_1
