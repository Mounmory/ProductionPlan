#include "stdafx.h"
#include "DynaArray.h"
#include <string>
#include <iostream>


template<typename _Ty>
std::ostream& operator<<(std::ostream &os, const DynaArray2D<_Ty>& rhs)
{
	os << "DynaArray2D Type:" << typeid(_Ty).name() <<
		"\t" << "nRow:" << rhs.m_rows << "\t" << "nCol:" << rhs.m_cols << std::endl;
	for (size_t i = 0; i < rhs.m_rows; ++i)
	{
		for (size_t j = 0; j < rhs.m_cols; ++j)
		{
			os << rhs.m_array[i][j] << "\t";
		}
		os << std::endl;
	}
	return os;
}

template<typename _Ty>
DynaArray2D<_Ty>::DynaArray2D() :m_rows(0), m_cols(0), m_array(nullptr)
{

}

template<typename _Ty>
DynaArray2D<_Ty>::DynaArray2D(const size_t nRow, const size_t nCol) : m_rows(nRow), m_cols(nCol), m_array(nullptr)
{
	m_array = new _Ty*[m_rows];
	for (size_t i = 0; i < m_rows; ++i)
	{
		m_array[i] = new _Ty[m_cols];
	}
}

template<typename _Ty>
DynaArray2D<_Ty>::DynaArray2D(const DynaArray2D<_Ty> &rhs) :m_rows(0), m_cols(0), m_array(nullptr)
{
	*this = rhs;
}

template<typename _Ty>
DynaArray2D<_Ty>::DynaArray2D(DynaArray2D<_Ty> && rhs) noexcept:
m_rows(std::exchange(rhs.m_rows, 0)),
m_cols(std::exchange(rhs.m_cols, 0)),
m_array(std::exchange(rhs.m_array, nullptr))
{
}

template<typename _Ty>
DynaArray2D<_Ty>::~DynaArray2D(void)
{
	destroyArray();
}

template<typename _Ty>
size_t DynaArray2D<_Ty>::getRowCount() const
{
	return m_rows;
}

template<typename _Ty>
size_t DynaArray2D<_Ty>::getColCount() const
{
	return m_cols;
}

template<typename _Ty>
void DynaArray2D<_Ty>::reSizeArray(const size_t nRow, const size_t nCol)
{
	if (this->m_rows != nRow || this->m_cols != nCol)
	{
		//new分配的时候，一般会分配除了它本身大小以外的内存头和内存尾，一般形式为：[内存头][可用数据][内存尾]。
		//而你的ptr指针返回的是[可用数据]部分的起始地址。大部分编译器在用户分配的size为0的情况下，会自动分配1字节的可用内存，可以用delete删除
		destroyArray();
		m_rows = nRow;
		m_cols = nCol;
		m_array = new _Ty*[m_rows];
		for (size_t i = 0; i < m_rows; ++i)
		{
			m_array[i] = new _Ty[m_cols];
		}
	}
}

template<>
void DynaArray2D<std::string>::zeroArray()
{
	for (size_t i = 0; i < m_rows; ++i)
	{
		for (size_t j = 0; j < m_cols; ++j)
		{
			m_array[i][j] = "";
		}
	}
}

template<>
void DynaArray2D<CString>::zeroArray()
{
	for (size_t i = 0; i < m_rows; ++i)
	{
		for (size_t j = 0; j < m_cols; ++j)
		{
			m_array[i][j] = "";
		}
	}
}

template<typename _Ty>
void DynaArray2D<_Ty>::zeroArray()
{
	for (size_t i = 0; i < m_rows; ++i)
	{
		memset(m_array[i], 0, m_cols * sizeof(_Ty));
	}
}

template<typename _Ty>
bool DynaArray2D<_Ty>::isMemoryCopy()
{
	return typeid(_Ty) == typeid(bool) ||
		typeid(_Ty) == typeid(unsigned char) ||
		typeid(_Ty) == typeid(char) ||
		typeid(_Ty) == typeid(short) ||
		typeid(_Ty) == typeid(int) ||
		typeid(_Ty) == typeid(float) ||
		typeid(_Ty) == typeid(double) ||
		typeid(_Ty) == typeid(long) ||
		typeid(_Ty) == typeid(long long);
}

template<typename _Ty>
void DynaArray2D<_Ty>::destroyArray()
{
	if (0 == m_rows && 0 == m_cols && nullptr == m_array)
	{
		return;
	}

	for (size_t i = 0; i < m_rows; ++i)
	{
		delete[] m_array[i];
	}

	delete m_array;
	m_array = nullptr;
}


template<typename _Ty>
_Ty* DynaArray2D<_Ty>::operator[](const size_t index)
{
	//if (index >= m_rows)
	//{
	//	throw std::out_of_range("Error! operator[], index lager than row count!");
	//}
	return m_array[index];
}

template<typename _Ty>
const _Ty* DynaArray2D<_Ty>::operator[] (const size_t index) const
{
	//if (index >= m_rows)
	//{
	//	throw std::out_of_range("Error! operator[], index lager than row count!");
	//}
	return m_array[index];
}

template<typename _Ty>
DynaArray2D<_Ty>& DynaArray2D<_Ty>::operator=(const _Ty& value)
{
	if (true == isMemoryCopy())
	{
		for (size_t i = 0; i < m_rows; ++i)
		{
			if (0 == i)
			{
				for (size_t j = 0; j < m_cols; ++j)
				{
					m_array[i][j] = value;
				}
			}
			else
			{
				memcpy(m_array[i], m_array[0], sizeof(_Ty) * m_cols);
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_rows; ++i)
		{
			for (size_t j = 0; j < m_cols; ++j)
			{
				m_array[i][j] = value;
			}
		}
	}
	return *this;
}

template<typename _Ty>
DynaArray2D<_Ty>& DynaArray2D<_Ty>::operator=(const DynaArray2D<_Ty> &rhs)
{
	if (m_rows != rhs.m_rows || m_cols != rhs.m_cols || m_array != rhs.m_array)//非自身赋值
	{
		if (m_rows != rhs.m_rows || m_cols != rhs.m_cols)
		{
			reSizeArray(rhs.m_rows, rhs.m_cols);
		}
		if (true == isMemoryCopy())
		{
			for (size_t i = 0; i < m_rows; ++i)
			{
				memcpy(m_array[i], rhs.m_array[i], m_cols * sizeof(_Ty));
			}
		}
		else
		{
			for (size_t i = 0; i < m_rows; ++i)
			{
				for (size_t j = 0; j < m_cols; ++j)
				{
					m_array[i][j] = rhs.m_array[i][j];
				}
			}
		}
	}
	return *this;
}

template<typename _Ty>
DynaArray2D<_Ty>& DynaArray2D<_Ty>::operator=(DynaArray2D<_Ty>&& rhs) noexcept
{
	if (this->m_array != rhs.m_array)
	{
		destroyArray();
		m_rows = std::exchange(rhs.m_rows, 0);
		m_cols = std::exchange(rhs.m_cols, 0);
		m_array = std::exchange(rhs.m_array, nullptr);
	}
	return *this;
}

template<typename _Ty>
bool DynaArray2D<_Ty>::operator==(const DynaArray2D<_Ty> &rhs)
{
	if (this->m_cols != rhs.m_cols || this->m_rows != rhs.m_rows)
	{
		return false;
	}
	for (int i = 0; i < this->m_rows; ++i)
	{
		for (int j = 0; j < this->m_cols; ++j)
		{
			if (this->m_array[i][j] != rhs.m_array[i][j])
			{
				return false;
			}
		}
	}
	return true;
}

template<typename _Ty>
bool DynaArray2D<_Ty>::operator!=(const DynaArray2D<_Ty>& rhs)
{
	return !(*this == rhs);
}
