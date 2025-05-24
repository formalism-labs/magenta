
#ifndef _magenta_geometry_matrix_
#define _magenta_geometry_matrix_

#include <math.h>
#include <memory.h>
//#include <ostream.h>

namespace magenta
{

////////////////////////////////////////////////////////////////////////////////

template <int N, class T = float>
class SquareMatrix
{
protected:
	T _matrix[N][N];
	
public:
	SquareMatrix() {}
	
	SquareMatrix(const SquareMatrix<N,T> &mat)
	{
		memcpy(_matrix, &mat._matrix[0][0], sizeof(_matrix));
	}
	
	const SquareMatrix &operator=(const SquareMatrix<N, T> &mat)
	{
		memcpy(_matrix, &mat._matrix[0][0], sizeof(_matrix));
		return mat;
	}
	
	T &operator()(int i, int j) { return _matrix[i][j]; }

	void print(char title[])
	{
		printf("%s:\n", title);
		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
				printf("%f\t", _matrix[i][j]);
			printf("\n");
		}	
	}
};

//------------------------------------------------------------------------------

class PerspectiveProjectionMatrix : public SquareMatrix<4, float>
{
public:
	PerspectiveProjectionMatrix(float l, float r, float t, float b, float n, float f)
	{
		_matrix[0][0] = (2.0f * n) / (r - l);
		_matrix[0][1] = 0.0f;
		_matrix[0][2] = 0.0f;
		_matrix[0][3] = 0.0f;
		_matrix[1][0] = 0.0f;
		_matrix[1][1] = (2.0f * n) / (t - b);
		_matrix[1][2] = 0.0f;
		_matrix[1][3] = 0.0f;
		_matrix[2][0] = (r + l) / (r - l);
		_matrix[2][1] = (t + b) / (t - b);
		_matrix[2][2] = -(f + n) / (f - n);
		_matrix[2][3] = -1.0f;
		_matrix[3][0] = 0.0f;
		_matrix[3][1] = 0.0f;
		_matrix[3][2] = -2.0f * (f * n) / (f - n);
		_matrix[3][3] = 0.0f;
	}
};

////////////////////////////////////////////////////////////////////////////////

template <class T>
class Matrix
{
	int _n, _m;
	T *_mat;

public:
	Matrix(int n, int m) : _n(n), _m(m), _mat(new T[n *m])
	{
	}

	Matrix(int n, int m, T init) : _n(n), _m(m), _mat(new T[n *m])
	{
		for (int i = 0; i < n * m; ++i)
			_mat[i] = init;
	}

	T &operator()(int i, int j) { return _mat[j + _m * i]; }
	const T &operator()(int i, int j) const { return _mat[j + _m * i]; }
/*
	void print(ostream &out) const
	{
		for (int i = 0; i < _n; ++i)
		{
			for (int j = 0; j < _m; ++j)
				out << (*this)(i, j) << "\t";
			out << endl;
		}
	}
*/
};

////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_geometry_matrix_

