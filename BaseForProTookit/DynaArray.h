#pragma once

#ifndef DYNARRAY_API
#define DYNARRAY_API __declspec(dllexport)

template<class _Ty>
class DYNARRAY_API DynaArray2D {
public:
	DynaArray2D();
	DynaArray2D(const size_t nRow, const size_t nCol);
	/*explicit */DynaArray2D(const DynaArray2D<_Ty> &rhs);
	DynaArray2D(DynaArray2D<_Ty>&& rhs) noexcept;//�ƶ����캯��
	~DynaArray2D();

	size_t getRowCount() const;
	size_t getColCount() const;
	void reSizeArray(const size_t nRow, const size_t nCol);
	void zeroArray();

	_Ty* operator[](const size_t index);
	const _Ty* operator[] (const size_t index) const;
	DynaArray2D<_Ty>& operator=(const _Ty& value);
	DynaArray2D<_Ty>& operator=(const DynaArray2D<_Ty> &rhs);
	DynaArray2D<_Ty>& operator=(DynaArray2D<_Ty>&& rhs) noexcept;

	template<typename _Ty>
	friend std::ostream& operator<< (std::ostream &os, const DynaArray2D<_Ty>& rhs);
	//����д��������ʽ���ڡ�<<������ϡ�<>����ʾ����һ������ģ��
	//friend std::ostream& operator<< <>(std::ostream &os, const DynaArray2D<_Ty>& rhs);
	
	bool operator==(const DynaArray2D<_Ty> &rhs);
	bool operator!=(const DynaArray2D<_Ty> &rhs);
private:
	bool isMemoryCopy();
	void destroyArray();
private:
	size_t m_rows;
	size_t m_cols;
	_Ty** m_array;
};

//����ʵ�������ͼ������
#define INSTANTIATE_TYPE_CLASS(type) \
template class DYNARRAY_API DynaArray2D<type>; \
template std::ostream DYNARRAY_API &operator<<(std::ostream &os, const DynaArray2D<type>& rhs);

INSTANTIATE_TYPE_CLASS(bool)
INSTANTIATE_TYPE_CLASS(char)
INSTANTIATE_TYPE_CLASS(unsigned char)
INSTANTIATE_TYPE_CLASS(short)
INSTANTIATE_TYPE_CLASS(int)
INSTANTIATE_TYPE_CLASS(unsigned short)
INSTANTIATE_TYPE_CLASS(unsigned int)
INSTANTIATE_TYPE_CLASS(float)
INSTANTIATE_TYPE_CLASS(double)
INSTANTIATE_TYPE_CLASS(long)
INSTANTIATE_TYPE_CLASS(long long)

//INSTANTIATE_TYPE_CLASS(char*)//��Ҫ�ػ�
INSTANTIATE_TYPE_CLASS(std::string)
INSTANTIATE_TYPE_CLASS(CString)

#endif