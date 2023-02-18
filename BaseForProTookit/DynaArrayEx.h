#pragma once

#ifndef DYNARRAY_API_EX
#define DYNARRAY_API_EX __declspec(dllexport)

template<class _Ty>
class DYNARRAY_API_EX DynaArray2DEx {
public:
	DynaArray2DEx();
	DynaArray2DEx(const size_t nRow, const size_t nCol);
	/*explicit */DynaArray2DEx(const DynaArray2DEx<_Ty> &rhs);
	DynaArray2DEx(DynaArray2DEx<_Ty>&& rhs) noexcept;//移动构造函数
	~DynaArray2DEx();

	size_t getRowCount() const;
	size_t getColCount() const;
	void reSizeArray(const size_t nRow, const size_t nCol);
	void zeroArray();

	_Ty* operator[](const size_t index);
	const _Ty* operator[] (const size_t index) const;
	DynaArray2DEx<_Ty>& operator=(const _Ty& value);
	DynaArray2DEx<_Ty>& operator=(const DynaArray2DEx<_Ty> &rhs);
	DynaArray2DEx<_Ty>& operator=(DynaArray2DEx<_Ty>&& rhs) noexcept;

	template<typename _Ty>
	friend std::ostream& operator<< (std::ostream &os, const DynaArray2DEx<_Ty>& rhs);
	//或者写成下面形式，在“<<”后加上“<>”表示这是一个函数模板
	//friend std::ostream& operator<< <>(std::ostream &os, const DynaArray2DEx<_Ty>& rhs);
	
	bool operator==(const DynaArray2DEx<_Ty> &rhs);
	bool operator!=(const DynaArray2DEx<_Ty> &rhs);
private:
	bool isMemoryCopy();
	void destroyArray();
private:
	size_t m_rows;
	size_t m_cols;
	_Ty* m_array;
};

//构建实例化类型及输出流
#define INSTANTIATE_TYPE_CLASS_EX(type) \
template class DYNARRAY_API_EX DynaArray2DEx<type>; \
template std::ostream DYNARRAY_API_EX &operator<<(std::ostream &os, const DynaArray2DEx<type>& rhs);

//template class DYNARRAY_API_EX DynaArray2DEx<bool>; 
//template std::ostream DYNARRAY_API_EX &operator<<(std::ostream &os, const DynaArray2DEx<bool>& rhs);

INSTANTIATE_TYPE_CLASS_EX(bool)
INSTANTIATE_TYPE_CLASS_EX(char)
INSTANTIATE_TYPE_CLASS_EX(unsigned char)
INSTANTIATE_TYPE_CLASS_EX(short)
INSTANTIATE_TYPE_CLASS_EX(int)
INSTANTIATE_TYPE_CLASS_EX(unsigned short)
INSTANTIATE_TYPE_CLASS_EX(unsigned int)
INSTANTIATE_TYPE_CLASS_EX(float)
INSTANTIATE_TYPE_CLASS_EX(double)
INSTANTIATE_TYPE_CLASS_EX(long)
INSTANTIATE_TYPE_CLASS_EX(long long)

//INSTANTIATE_TYPE_CLASS_EX(char*)//需要特化
INSTANTIATE_TYPE_CLASS_EX(std::string)
INSTANTIATE_TYPE_CLASS_EX(CString)

#endif