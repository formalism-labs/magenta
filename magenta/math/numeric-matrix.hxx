
namespace magenta_math_1
{

const double EPSILON = 1e-12;

///////////////////////////////////////////////////////////////////////////////////////////////
// CNumericMatrix
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
NumericVector<T> CNumericMatrix<T>::row(int i) const
{ 
#ifdef MAGENTA_MATH_CHECK_DIMENTIONS
	if (i<0 || i>=d1)  
		throw "matrix: row index out of range";
#endif

	NumericVector<T> result(d2);
	int j = d2;
	while (j--) 
		result[j] = elem(i, j);
	return result;
}

//---------------------------------------------------------------------------------------------

template <class T>
NumericVector<T> CNumericMatrix<T>::col(int i)  const
{ 
#ifdef MAGENTA_MATH_CHECK_DIMENTIONS
	if (i<0 || i>=d2) 
		throw "matrix: col index out of range";
#endif

	NumericVector<T> result(d1);
	int j = d1;
	while (j--) 
		result[j] = elem(j,i);
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void CNumericMatrix<T>::add(const This& mat, This &result) const
{ 
	int i,j;
	check_dimensions(mat);
	for(i=0;i<d1;i++)
		for(j=0;j<d2;j++)
			result.setElem(i,j, elem(i,j) + mat.elem(i,j));
}

//---------------------------------------------------------------------------------------------

template <class T>
void CNumericMatrix<T>::subtract(const This& mat, This &result) const
{ 
	int i,j;
	check_dimensions(mat);
	for(i=0;i<d1;i++)
		for(j=0;j<d2;j++)
			result.setElem(i,j, elem(i,j) - mat.elem(i,j));
}

//---------------------------------------------------------------------------------------------

template <class T>
void CNumericMatrix<T>::negate(This &result)  const
{ 
	int i,j;
	for(i=0;i<d1;i++)
		for(j=0;j<d2;j++)
			result.setElem(i,j, -elem(i,j));
}

//---------------------------------------------------------------------------------------------

template <class T>
void CNumericMatrix<T>::multiply(T f, This &result) const
{ 
	int i,j;
	for(i=0;i<d1;i++)
		for(j=0;j<d2;j++)
			result.setElem(i,j, elem(i,j) *f);
}

//---------------------------------------------------------------------------------------------

template <class T>
void CNumericMatrix<T>::multiply(const This& mat, This &result) const
{
	if (d2!=mat.d1)
		throw "matrix multiplication: incompatible matrix types";
  
	int i, j, k;
	for (i = 0; i < d1; ++i)
		for (j = 0; j < mat.d2; ++j)
		{
			T e = 0;
			for (k = 0; k < d2; ++k)
				e += elem(i, k) * mat.elem(k, j);
			result.setElem(i, j, e);
		}
}

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T CNumericMatrix<T>::det() const
{
	if (d1!=d2)  
		throw "NumericMatrix<T>::det: matrix not quadratic";

	int n = d1;
	NumericMatrix<T> M(n,1);
	int flips;

	T** A = triang(M,flips);
	if (!A)  
		return 0;

	T Det = 1;
	int i;
	for(i=0;i<n;i++) 
		Det *= A[i][i];
	for(i=0;i<n;i++) 
		delete[] A[i];
	delete[] A;

	return (flips % 2) ? -Det : Det;
}

//---------------------------------------------------------------------------------------------

template <class T>
T** CNumericMatrix<T>::triang(const CNumericMatrix &M, int &flips) const
{
	T **p, **q;
	T *l, *r, *s;

	T pivot_el,tmp;

	int i,j, col, row;

	int n = d1;
	int d = M.d2;
	int m = n+d;

	T** A = new T*[n];

	p = A;

	for(i=0;i<n;i++) 
	{ 
		*p = new T[m];
		l = *p++;
		for(j=0;j<n;j++) 
			*l++ = elem(i,j);
		for(j=0;j<d;j++) 
			*l++ = M.elem(i,j);
	}

	flips = 0;

	for (col=0, row=0; row<n; row++, col++)
	{
		// search for row j with maximal absolute entry in current col
		j = row;
		for (i=row+1; i<n; i++)
		if (fabs(A[j][col]) < fabs(A[i][col])) j = i;

		if (row < j /* && j < n */) 
		{ 
			T* p = A[j];
			A[j] = A[row];
			A[row] = p;
			flips++;
		}

		tmp = A[row][col];
		q  = &A[row];

		if (fabs(tmp) < EPSILON) // matrix has not full rank
		{ 
			p = A;
			for (i = 0; i < n; ++i) 
				delete A[i];
			delete[] A;
			return 0;
		}

		for (p = &A[n-1]; p != q; --p)
		{ 
			l = *p+col;
			s = *p+m;	
			r = *q+col;

			if (*l != 0.0)
			{ 
				pivot_el = *l / tmp;
				while (l < s)
					*l++ -= *r++ * pivot_el;
			}
		}
	}

	return A;
}

//---------------------------------------------------------------------------------------------

template <class T>
void CNumericMatrix<T>::inv(This &result) const
{
	if (d1!=d2)  
		throw "NumericMatrix<T>::inv: matrix not quadratic";
	int n = d1;
	IdentityMatrix<T> I(n);
	solve(I, result);
}

//---------------------------------------------------------------------------------------------

template <class T>
void CNumericMatrix<T>::solve(const This& M, This &result) const
{
	if (d1 != d2 || d1 != M.d1)
		throw  "Solve: wrong dimensions";

	T **p, ** q;
	T *l, *r, *s;

	int      n = d1;
	int      d = M.d2;
	int      m = n+d;
	int      row, col,i;

	T** A = triang(M,i);

	if (!A) 
		throw "NumericMatrix<T>::solve: matrix has not full rank.";

	for (col = n-1, p = &A[n-1]; col>=0; p--, col--)
	{ 
		s = *p+m;
		T tmp = (*p)[col];

		for(l=*p+n; l < s; l++) 
			*l /=tmp;

		for(q = A; q != p; q++)
		{ 
			tmp = (*q)[col];
			l = *q+n;
			r = *p+n;
			while(r < s)  
				*l++ -= *r++ * tmp;
		}

	} 

//	NumericMatrix<T> result(n,d);

	for(row=0; row<n; row++)
	{ 
		l = A[row]+n;
		for(col=0; col<d; col++) 
			result.setElem(row,col, *l++);
		delete[] A[row];
	}

	delete[] A;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// NumericMatrix
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
RefNumericVector<T> NumericMatrix<T>::row(int i) const
{ 
#ifdef MAGENTA_MATH_CHECK_DIMENTIONS
	if (i<0 || i>=d1)  
		throw "matrix: row index out of range";
#endif
	return RefNumericVector<T>(&v[i * d2], d2);
}

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
NumericMatrix<T> &NumericMatrix<T>::operator=(const This &mat) 
{
	Matrix<T>::operator=(mat);
	return *this;
}

//---------------------------------------------------------------------------------------------
/*
template <class T>
CNumericMatrix<T> &CNumericMatrix<T>::operator+=(const This &mat) 
{ 
	int i, j;
	check_dimensions(mat);
	for(i = 0; i < d1; i++)
		for(j = 0; j < d2; j++)
			setElem(i,j elem(i, j) +  mat.elem(i,j));
	return *this;
}

//---------------------------------------------------------------------------------------------

template <class T>
CNumericMatrix<T> &CNumericMatrix<T>::operator-=(const This &mat) 
{ 
	int i,j;
	check_dimensions(mat);
	for(i=0;i<d1;i++)
		for(j=0;j<d2;j++)
			setElem(i, j, elem(i,j) - mat.elem(i,j));
	return *this;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void NumericMatrix<T>::set(T value)
{
	int i, j;
	for (i = 0; i < d1; ++i)
		for (j = 0; j < d2; ++j)
			setElem(i, j,  value);
}

//---------------------------------------------------------------------------------------------

template <class T>
void NumericMatrix<T>::setIdentity()
{
	int i, j;
	for (i = 0; i < d1; ++i)
		for (j = 0; j < d2; ++j)
			setElem(i, j, i == j ? 1 : 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Sparse NumericMatix
///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
SparseNumericMatrix<T>::SparseNumericMatrix(int n, int m)
{
	d1 = n;
	d2 = m;
}

//---------------------------------------------------------------------------------------------

template <class T>
SparseNumericMatrix<T>::SparseNumericMatrix(int n, int m, int hashSize) :
	_table(hashSize)
{
	d1 = n;
	d2 = m;
}

//---------------------------------------------------------------------------------------------

template <class T>
SparseNumericMatrix<T>::SparseNumericMatrix(int n, int m, T* D)
{
	d1 = n;
	d2 = m;
	T x;
	for (int i = 0; i < d1; ++i)
		for (int j = 0; j < d2; ++j)
			if ((x = *D++))
				setElem(i, j, x);
}

//---------------------------------------------------------------------------------------------

template <class T>
SparseNumericMatrix<T>::SparseNumericMatrix(const CConstMatrix<T> &m)
{
	*this = m;
}

//---------------------------------------------------------------------------------------------

template <class T>
SparseNumericMatrix<T>::SparseNumericMatrix(const NumericVector<T> &v)
{
	d1 = vec.d;
	d2 = 1;
	for (int i = 0; i < d1; ++i)
		if (vec[i])
			setElem(i, 0, vec[i]);
}

//---------------------------------------------------------------------------------------------

template <class T>
SparseNumericMatrix<T> &SparseNumericMatrix<T>::operator=(const CConstMatrix<T>&M)
{
	_table.clear();
	d1 = m.d1;
	d2 = m.d2;
	T x;
	for (int i = 0; i < d1; ++i)
		for (j = 0; j < d2; ++j)
		{
			x =  m(i, j);
			if (x)
				setElem(i, j, x);
		}
}

///////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T SparseNumericMatrix<T>::elem(int i, int j) const
{
	Table::const_iterator k = _table.find(int64_t(i) << 32 | j);
	if (k == _table.end())
		return 0;
	return k->second;
}

//---------------------------------------------------------------------------------------------

template <class T>
T &SparseNumericMatrix<T>::elem(int i, int j)
{
	int64_t k = int64_t(i) << 32 | j;
	Table::iterator it = _table.find(k);
	if (it == _table.end())
	{
		Table::value_type &v = *(_table.insert(Table::value_type(k, 0)).first);
		return v.second;
	}
	Table::value_type &v = *it;
	return v.second;
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta_math_1
