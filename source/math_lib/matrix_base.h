#ifndef SELECTOR_MATH_LIB_MATRIX_BASE_H_
#define SELECTOR_MATH_LIB_MATRIX_BASE_H_

#include <common/assertions.h>
#include <common/profiler.h>

#include "radians.h"
#include "vec_base.h"

#include <cmath>
#include <cstring>

namespace selector
{
template<int NumCols, int NumRows, class Rep>
class MatrixBase
{
public:
	using MatrixType = MatrixBase<NumCols, NumRows, Rep>;
	using ColumnType = VecBase<NumRows, Rep>;
	using RowType = VecBase<NumCols, Rep>;
	static const size_t kNumCols = NumCols;
	static const size_t kNumRows = NumRows;
	static const size_t kSizeInBytes = kNumCols * kNumRows * sizeof(Rep);

	MatrixBase()
	{
		START_PROFILE;
		for (auto r = 0; r < NumRows; ++r)
		{
			for (auto c = 0; c < NumCols; ++c)
			{
				m_elems[r * NumCols + c] = 0;
			}
		}
	}

	explicit MatrixBase(const Rep &diagonal)
	{
		START_PROFILE;
		for (auto r = 0; r < NumRows; ++r)
		{
			for (auto c = 0; c < NumCols; ++c)
			{
				m_elems[r * NumCols + c] = (r == c ? diagonal : 0);
			}
		}
	}

	explicit MatrixBase(const MatrixBase<NumCols - 1, NumRows - 1, Rep> &m)
	{
		START_PROFILE;
		for (auto r = 0; r < NumRows - 1; ++r)
		{
			for (auto c = 0; c < NumCols - 1; ++c)
			{
				m_elems[r * NumCols + c] = m.Value(c, r);
			}
		}
		for (auto r = 0; r < NumRows; ++r)
		{
			m_elems[r * NumCols + NumCols - 1] = 0;
		}
		for (auto c = 0; c < NumCols; ++c)
		{
			m_elems[(NumRows - 1) * NumCols + c] = 0;
		}
		m_elems[NumRows * NumCols - 1] = 1;
	}

	explicit MatrixBase(const MatrixBase<NumCols + 1, NumRows + 1, Rep> &m)
	{
		START_PROFILE;
		for (auto r = 0; r < NumRows; ++r)
		{
			for (auto c = 0; c < NumCols; ++c)
			{
				m_elems[r * NumCols + c] = m.Value(c, r);
			}
		}
	}

	ColumnType Col(const size_t &col) const
	{
		START_PROFILE;
		CRITICAL_ASSERT(col < NumCols);

		ColumnType c;
		for (auto i = 0u; i < NumRows; ++i)
		{
			c[i] = m_elems[col + NumCols * i];
		}

		return c;
	}

	void SetCol(const size_t &colIndex, const ColumnType &col)
	{
		START_PROFILE;
		CRITICAL_ASSERT(colIndex < NumCols);
		for (auto i = 0u; i < NumRows; ++i)
		{
			m_elems[colIndex + NumCols * i] = col[i];
		}
	}

	RowType Row(const size_t &row) const
	{
		START_PROFILE;
		CRITICAL_ASSERT(row < NumRows);

		RowType r;
		for (auto i = 0u; i < NumCols; ++i)
		{
			r[i] = m_elems[i + NumRows * row];
		}
		return r;
	}

	void SetRow(const size_t &rowIndex, const RowType &row)
	{
		START_PROFILE;
		CRITICAL_ASSERT(rowIndex < NumRows);

		for (auto i = 0u; i < NumCols; ++i)
		{
			m_elems[i + NumRows * rowIndex] = row[i];
		}
	}

	Rep &Value(const size_t &col, const size_t &row)
	{
		START_PROFILE;
		CRITICAL_ASSERT(col < NumCols);
		CRITICAL_ASSERT(row < NumRows);

		return m_elems[col + row * NumCols];
	}

	const Rep &Value(const size_t &col, const size_t &row) const
	{
		START_PROFILE;
		CRITICAL_ASSERT(col < NumCols);
		CRITICAL_ASSERT(row < NumRows);

		return m_elems[col + row * NumCols];
	}

	bool operator==(const MatrixBase<NumCols, NumRows, Rep> &m) const
	{
		return std::memcmp(m_elems, m.m_elems, NumCols * NumRows * sizeof(Rep)) == 0;
	}

	bool operator!=(const MatrixBase<NumCols, NumRows, Rep> &m) const { return !(*this == m); }

private:
	Rep m_elems[NumCols * NumRows];
};  // class MatrixBase

template<int NumCols, int NumRows, class Rep>
std::ostream &operator<<(std::ostream &o, const MatrixBase<NumCols, NumRows, Rep> &mat)
{
	for (auto r = 0u; r < NumRows; ++r)
	{
		o << mat.Row(r) << std::endl;
	}
	return o;
}

// Common matrix types
using Mat2x2F = MatrixBase<2, 2, float>;
using Mat3x3F = MatrixBase<3, 3, float>;
using Mat4x4F = MatrixBase<4, 4, float>;

template<typename T>
MatrixBase<4, 4, T> translate_matrix(const T &x, const T &y, const T &z)
{
	MatrixBase<4, 4, T> mat;
	mat.SetRow(0, typename MatrixBase<4, 4, T>::ColumnType(1, 0, 0, x));
	mat.SetRow(1, typename MatrixBase<4, 4, T>::ColumnType(0, 1, 0, y));
	mat.SetRow(2, typename MatrixBase<4, 4, T>::ColumnType(0, 0, 1, z));
	mat.SetRow(3, typename MatrixBase<4, 4, T>::ColumnType(0, 0, 0, 1));

	return mat;
}

template<typename T>
MatrixBase<4, 4, T> translate_matrix(VecBase<3, T> vec)
{
	return translate_matrix(vec.X(), vec.Y(), vec.Z());
}

template<typename T>
MatrixBase<4, 4, T> scale_matrix(const T &x, const T &y, const T &z)
{
	MatrixBase<4, 4, T> mat;
	mat.SetRow(0, typename MatrixBase<4, 4, T>::ColumnType(x, 0, 0, 0));
	mat.SetRow(1, typename MatrixBase<4, 4, T>::ColumnType(0, y, 0, 0));
	mat.SetRow(2, typename MatrixBase<4, 4, T>::ColumnType(0, 0, z, 0));
	mat.SetRow(3, typename MatrixBase<4, 4, T>::ColumnType(0, 0, 0, 1));

	return mat;
}

template<typename T>
MatrixBase<4, 4, T> scale_matrix(VecBase<3, T> vec)
{
	return scale_matrix(vec.X(), vec.Y(), vec.Z());
}

template<typename T>
MatrixBase<4, 4, T> rotate_x_matrix(const Radians<T> &radians)
{
	T s = std::sin(static_cast<T>(radians));
	T c = std::cos(static_cast<T>(radians));

	MatrixBase<4, 4, T> mat;
	mat.SetRow(0, typename MatrixBase<4, 4, T>::ColumnType(1, 0, 0, 0));
	mat.SetRow(1, typename MatrixBase<4, 4, T>::ColumnType(0, c, -s, 0));
	mat.SetRow(2, typename MatrixBase<4, 4, T>::ColumnType(0, s, c, 0));
	mat.SetRow(3, typename MatrixBase<4, 4, T>::ColumnType(0, 0, 0, 1));

	return mat;
}

template<typename T>
MatrixBase<4, 4, T> rotate_y_matrix(const Radians<T> &radians)
{
	T s = std::sin(static_cast<T>(radians));
	T c = std::cos(static_cast<T>(radians));

	MatrixBase<4, 4, T> mat;
	mat.SetRow(0, typename MatrixBase<4, 4, T>::ColumnType(c, 0, s, 0));
	mat.SetRow(1, typename MatrixBase<4, 4, T>::ColumnType(0, 1, 0, 0));
	mat.SetRow(2, typename MatrixBase<4, 4, T>::ColumnType(-s, 0, c, 0));
	mat.SetRow(3, typename MatrixBase<4, 4, T>::ColumnType(0, 0, 0, 1));

	return mat;
}

template<typename T>
MatrixBase<4, 4, T> rotate_z_matrix(const Radians<T> &radians)
{
	T s = std::sin(static_cast<T>(radians));
	T c = std::cos(static_cast<T>(radians));

	MatrixBase<4, 4, T> mat;
	mat.SetRow(0, typename MatrixBase<4, 4, T>::ColumnType(c, -s, 0, 0));
	mat.SetRow(1, typename MatrixBase<4, 4, T>::ColumnType(s, c, 0, 0));
	mat.SetRow(2, typename MatrixBase<4, 4, T>::ColumnType(0, 0, 1, 0));
	mat.SetRow(3, typename MatrixBase<4, 4, T>::ColumnType(0, 0, 0, 1));

	return mat;
}

}  // namespace selector

#include "matrix_arithmetic.h"

#endif
