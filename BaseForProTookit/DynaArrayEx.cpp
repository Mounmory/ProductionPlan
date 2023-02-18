#include "stdafx.h"
#include "DynaArrayEx.h"
#include <string>
#include <iostream>


template<typename _Ty>
std::ostream& operator<<(std::ostream &os, const DynaArray2DEx<_Ty>& rhs)
{
	os <<"DynaArray2DEx Type:"<< typeid(_Ty).name() <<
		"\t" << "nRow:" << rhs.m_rows << "\t" << "nCol:" << rhs.m_cols << std::endl;
	for (size_t i = 0; i < rhs.m_rows; ++i)
	{
		for (size_t j = 0; j < rhs.m_cols; ++j)
		{
			os << rhs.m_array[rhs.m_cols * i + j] << "\t";
		}
		os << std::endl;
	}
	return os;
}

template<typename _Ty>
DynaArray2DEx<_Ty>::DynaArray2DEx():m_rows(0),m_cols(0),m_array(nullptr)
{

}

template<typename _Ty>
DynaArray2DEx<_Ty>::DynaArray2DEx(const size_t nRow, const size_t nCol) : m_rows(nRow), m_cols(nCol), m_array(nullptr)
{
	m_array = new _Ty[m_rows * m_rows];
}

template<typename _Ty>
DynaArray2DEx<_Ty>::DynaArray2DEx(const DynaArray2DEx<_Ty> &rhs):m_rows(0),m_cols(0), m_array(nullptr)
{
	*this = rhs;
}

template<typename _Ty>
DynaArray2DEx<_Ty>::DynaArray2DEx(DynaArray2DEx<_Ty> && rhs) noexcept:
	m_rows(std::exchange(rhs.m_rows,0)),
	m_cols(std::exchange(rhs.m_cols,0)),
	m_array(std::exchange(rhs.m_array,nullptr)) 
{
}

template<typename _Ty>
DynaArray2DEx<_Ty>::~DynaArray2DEx(void)
{
	destroyArray();
}

template<typename _Ty>
size_t DynaArray2DEx<_Ty>::getRowCount() const
{
	return m_rows;
}

template<typename _Ty>
size_t DynaArray2DEx<_Ty>::getColCount() const
{
	return m_cols;
}

template<typename _Ty>
void DynaArray2DEx<_Ty>::reSizeArray(const size_t nRow, const size_t nCol)
{
	if ((m_rows * m_cols) != (nRow * nCol))
	{
		//new分配的时候，一般会分配除了它本身大小以外的内存头和内存尾，一般形式为：[内存头][可用数据][内存尾]。
		//而你的ptr指针返回的是[可用数据]部分的起始地址。大部分编译器在用户分配的size为0的情况下，会自动分配1字节的可用内存，可以用delete删除
		destroyArray();
		m_rows = nRow;
		m_cols = nCol;
		m_array = new _Ty[m_rows * m_cols];
	}
}

template<>
void DynaArray2DEx<std::string>::zeroArray()
{
	for (size_t i = 0; i < m_rows * m_cols; ++i)
	{
		m_array[i] = "";
	}
}

template<>
void DynaArray2DEx<CString>::zeroArray()
{
	for (size_t i = 0; i < m_rows * m_cols; ++i)
	{
		m_array[i] = "";
	}
}

template<typename _Ty>
void DynaArray2DEx<_Ty>::zeroArray()
{
	memset(m_array, 0, m_rows * m_cols * sizeof(_Ty));
}

template<typename _Ty>
bool DynaArray2DEx<_Ty>::isMemoryCopy() 
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
void DynaArray2DEx<_Ty>::destroyArray()
{
	if (0 == m_rows && 0 == m_cols && nullptr == m_array)
	{
		return;
	}

	delete[] m_array;
	m_array = nullptr;
}


template<typename _Ty>
_Ty* DynaArray2DEx<_Ty>::operator[](const size_t index)
{
	//if (index >= m_rows * m_cols)//越界了
	//{
	//	throw std::out_of_range("Error! operator[], index lager than row count!");
	//}
	return &m_array[index * m_cols];
}

template<typename _Ty>
const _Ty* DynaArray2DEx<_Ty>::operator[] (const size_t index) const
{
	//if (index >= m_rows * m_cols)//越界了
	//{
	//	throw std::out_of_range("Error! operator[], index lager than row count!");
	//}
	return &m_array[index * m_cols];
}

template<typename _Ty>
DynaArray2DEx<_Ty>& DynaArray2DEx<_Ty>::operator=(const _Ty& value)
{
	if (true == isMemoryCopy())
	{
		for (size_t i = 0; i < m_rows; ++i)
		{
			if (0 == i)
			{
				for (size_t j = 0; j < m_cols; ++j)
				{
					m_array[j] = value;
				}
			}
			else
			{
				memcpy(&m_array[i * m_cols], &m_array[0], sizeof(_Ty) * m_cols);
			}
		}
	}
	else 
	{
		for (size_t i = 0; i < m_rows * m_cols; ++i)
		{
			m_array[i] = value;
		}
	}
	return *this;
}

template<typename _Ty>
DynaArray2DEx<_Ty>& DynaArray2DEx<_Ty>::operator=(const DynaArray2DEx<_Ty> &rhs)
{
	if (m_rows != rhs.m_rows || m_cols != rhs.m_cols || m_array != rhs.m_array)//非自身赋值
	{

		reSizeArray(rhs.m_rows, rhs.m_cols);

		if (true == isMemoryCopy())
		{
			memcpy(m_array, rhs.m_array, m_rows * m_cols * sizeof(_Ty));
		}
		else 
		{
			for (size_t i = 0; i < m_rows * m_cols; ++i)
			{
				m_array[i] = rhs.m_array[i];
			}
		}
	}
	return *this;
}

template<typename _Ty>
DynaArray2DEx<_Ty>& DynaArray2DEx<_Ty>::operator=(DynaArray2DEx<_Ty>&& rhs) noexcept
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
bool DynaArray2DEx<_Ty>::operator==(const DynaArray2DEx<_Ty> &rhs)
{
	if (this->m_cols != rhs.m_cols || this->m_rows != rhs.m_rows)
	{
		return false;
	}
	for (int i = 0; i < m_rows * m_cols; ++i)
	{
		if (this->m_array[i] != rhs.m_array[i])
		{
			return false;
		}
	}
	return true;
}

template<typename _Ty>
bool DynaArray2DEx<_Ty>::operator!=(const DynaArray2DEx<_Ty>& rhs) 
{
	return !(*this == rhs);
}
