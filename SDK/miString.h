#ifndef _MI_STRING_H_
#define _MI_STRING_H_

#include <cstdlib>
#include <cstdio>
#include <cstring>


const size_t miString_wordSize = 16;

// work with this only inside one module
// application must change text through plugin
template<typename char_type>
class miString_base
{
	typedef char_type* pointer;
	typedef const char_type* const_pointer;
	typedef char_type& reference;
	typedef const char_type&  const_reference;
	typedef miString_base this_type;
	typedef const miString_base& this_const_reference;

	pointer m_data;
	size_t     m_allocated;
	size_t     m_size;

	void reallocate(size_t new_allocated)
	{
		char_type * new_data = (char_type*)miMalloc(new_allocated * sizeof(char_type));
		if (m_data)
		{
			std::memcpy(new_data, m_data, m_size * sizeof(char_type));
			miFree( m_data );
		}
		else
		{
			std::memset(new_data, 0, new_allocated);
		}
		m_data = new_data;
		m_allocated = new_allocated;
	}

	template<typename other_type>
	size_t getlen(const other_type* str)
	{
		unsigned int len = 0u;
		if (str[0] != 0)
		{
			const other_type* p = &str[0u];
			while ((size_t)*p++)
				len++;
		}
		return len;
	}

	template<typename dst_type, typename src_type>
	void copy(dst_type * dst, src_type* src) const
	{
		while ((size_t)*src)
		{
			*dst = static_cast<dst_type>(*src);
			dst++;
			src++;
		}
	}


public:
	typedef char_type value_type;

	miString_base()
		:m_data(nullptr), m_allocated(miString_wordSize), m_size(0)
	{
		reallocate(m_allocated);
	}

	template<typename other_type>
	miString_base(const other_type * str)
		: m_data(nullptr), m_allocated(miString_wordSize), m_size(0)
	{
		reallocate(m_allocated);
		assign(str);
	}

	miString_base(this_const_reference str)
		:  m_data(nullptr), m_allocated(miString_wordSize), m_size(0)
	{
		reallocate(m_allocated);
		assign(str);
	}

	miString_base(this_type&& str)
		: m_data(nullptr), m_allocated(miString_wordSize), m_size(0)
	{
		reallocate(m_allocated);
		assign(str);
	}

	miString_base(char_type c)
		:  m_data(nullptr), m_allocated(miString_wordSize), m_size(0)
	{
		size_t new_size = 1u;
		reallocate((new_size + 1u) + miString_wordSize);
		m_data[0u] = c;
		m_size = new_size;
		m_data[m_size] = static_cast<char_type>(0x0);
	}

	~miString_base(){
		if (m_data)
			miFree( m_data );
	}

	void reserve(size_t size){
		if (size > m_allocated)
		{
			reallocate(size);
			m_data[m_size] = 0;
		}
	}

	template<typename other_type>
	void assign(other_type str){
		m_size = 0u;
		m_data[m_size] = static_cast<char_type>(0x0);
		if (str)
			append(str);
	}

	void assign(this_const_reference str){
		m_size = 0u;
		m_data[m_size] = static_cast<char_type>(0x0);
		append(str);
	}

	template<typename other_type>
	void append(const other_type * str){
		size_t new_size = getlen(str) + m_size;

		if ((new_size + 1u) > m_allocated)
			reallocate((new_size + 1u) + (1 + (u32)(m_size * 0.5f)));

		copy(&m_data[m_size], str);

		m_size = new_size;
		m_data[m_size] = static_cast<char_type>(0x0);
	}

	void append(this_const_reference str){
		append(str.data());
	}

	void push_back(char_type c) {
		size_t new_size = m_size + 1u;
		if ((new_size + 1u) > m_allocated)
			reallocate((new_size + 1u) + (1 + (u32)(m_size * 0.5f)));
		m_data[m_size] = c;
		m_size = new_size;
		m_data[m_size] = 0;
	}
	void append(char_type c) {
		size_t new_size = m_size + 1u;
		if ((new_size + 1u) > m_allocated)
			reallocate((new_size + 1u) + (1 + (u32)(m_size * 0.5f)));
		m_data[m_size] = c;
		m_size = new_size;
		m_data[m_size] = 0;
	}

	void append(int c)	{
		char buf[32u];
		::std::sprintf(buf, "%i", c);
		append(buf);
	}

	void append(size_t c)	{
		char buf[32u];
		::std::sprintf(buf, "%u", c);
		append(buf);
	}

	void append(long long c)	{
		char buf[32u];
		::std::sprintf(buf, "%llu", c);
		append(buf);
	}

	void append(double c)	{
		char buf[32u];
		::std::sprintf(buf, "%.12f", c);
		append(buf);
	}

	void append(float c)	{
		char buf[32u];
		::std::sprintf(buf, "%.4f", c);
		append(buf);
	}
	void append_float(float c) {
		char buf[32u];
		::std::sprintf(buf, "%.7f", c);
		append(buf);
	}

	const_pointer c_str() const { return m_data; }
	pointer data() const { return m_data; }
	const size_t size() const { return m_size; }

	this_type& operator=(this_const_reference str)	{
		assign(str);
		return *this;
	}

	this_type& operator=(this_type&& str)	{
		assign(str);
		return *this;
	}


	template<typename other_type>
	this_type& operator=(other_type * str)	{
		assign(str);
		return *this;
	}

	this_type operator+(const_pointer str)	{
		this_type ret(*this);
		ret.append(str);
		return ret;
	}

	this_type operator+(this_const_reference str)	{
		return operator+(str.data());
	}

	this_type operator+(size_t num)	{
		this_type r(*this);
		r.append(num);
		return r;
	}

	pointer begin() const	{return m_data;}
	pointer end() const	{return (m_data + (m_size));}
	const_reference operator[](size_t i) const	{return m_data[i];}
	reference operator[](size_t i){return m_data[i];}
	void operator+=(int i){append(i);}
	void operator+=(size_t i){append(i);	}
	void operator+=(long long i)	{		append(i);	}
	void operator+=(float i)	{		append(i);	}
	void operator+=(double i)	{		append(i);	}
	void operator+=(char_type c)	{		append(c);	}

	template<typename other_type>
	void operator+=(other_type * str)	{		append(str);	}
	void operator+=(this_const_reference str)	{		append(str);	}
	
	bool operator==(this_const_reference other) const	{
		if (other.size() != m_size)
			return false;

		const size_t sz = other.size();
		const auto * ptr = other.data();
		for (size_t i = 0u; i < sz; ++i)
		{
			if (ptr[i] != m_data[i])
				return false;
		}

		return true;
	}

	bool operator!=(this_const_reference other) const	{
		if (other.size() != m_size)
			return true;

		const size_t sz = other.size();
		const auto * ptr = other.data();
		for (size_t i = 0u; i < sz; ++i)
		{
			if (ptr[i] != m_data[i])
				return true;
		}

		return false;
	}

	void clear()	{
		m_size = 0u;
		m_data[m_size] = 0;
	}

	void pop_back(){
		if (m_size)
		{
			--m_size;
			m_data[m_size] = 0;
		}
	}

	char_type pop_back_return(){
		char_type r = '?';
		if (m_size)
		{
			--m_size;
			r = m_data[m_size];
			m_data[m_size] = 0;
		}
		return r;
	}

	void shrink_to_fit(){
		if (m_size)
		{
			if (m_allocated > (m_size + (1 + (u32)(m_size * 0.5f))))
			{
				reallocate(m_size + 1u);
				m_data[m_size] = static_cast<char_type>(0x0);
			}
		}
	}

	void setSize(size_t size){
		m_size = size;
	}

	void erase(size_t begin, size_t end){
		size_t numCharsToDelete = end - begin + 1u; // delete first char: 0 - 0 + 1
		size_t next = end + 1u;
		for (size_t i = begin; i < m_size; ++i)
		{
			if (next < m_size)
			{
				m_data[i] = m_data[next];
				++next;
			}
		}
		m_size -= numCharsToDelete;
		m_data[m_size] = static_cast<char_type>(0x0);
	}

	void pop_front(){
		erase(0u, 0u);
	}

	bool is_space(size_t index){
		if (m_data[index] == (char_type)' ') return true;
		if (m_data[index] == (char_type)'\r') return true;
		if (m_data[index] == (char_type)'\n') return true;
		if (m_data[index] == (char_type)'\t') return true;
		return false;
	}
};

typedef miString_base<wchar_t> miString;
typedef miString_base<char> miStringA;

#endif