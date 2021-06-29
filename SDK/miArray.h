#ifndef _MI_ARRAY_H_
#define _MI_ARRAY_H_

// `clear` must do m_size = 0;
template<typename type>
class miArray
{
	u32     m_allocated;
	void reallocate(u32 new_capacity)
	{
		new_capacity += 4 * (m_allocated / 2);
		auto tmp_size = new_capacity * sizeof(type);
		pointer new_data = static_cast<type*>(miMalloc(tmp_size));
		memset(new_data, 0, tmp_size);

		if (m_data)
		{
			for (u32 i = 0u; i < m_size; ++i)
			{
				new(&new_data[i]) type(m_data[i]);
				(&m_data[i])->~type();
			}
			miFree(m_data);
		}
		m_data = new_data;
		m_allocated = new_capacity;
	}
public:
	typedef type* pointer;
	typedef type& reference;
	typedef const type& const_reference;

	miArray() :m_allocated(0), m_size(0), m_data(0) {}
	~miArray() { free_memory(); }

	u32 capacity() {
		return m_allocated;
	}

	void reserve(u16 new_capacity)
	{
		if (new_capacity > m_allocated)
			reallocate(new_capacity);
	}

	void push_back(const_reference object)
	{
		u32 new_size = m_size + 1u;
		if (new_size > m_allocated)
			reallocate(new_size);
		new(&m_data[m_size]) type(object);
		m_size = new_size;
	}

	void free_memory()
	{
		if (m_data)
		{
			for (u32 i = 0u; i < m_allocated; ++i)
			{
				(&m_data[i])->~type();
			}
			miFree(m_data);

			m_allocated = m_size = 0u;
			m_data = nullptr;
		}
	}

	void clear()
	{
		m_size = 0;
	}

	type* get(u32 index)
	{
		if (index >= m_allocated)
			return nullptr;

		return &m_data[index];
	}

	/*
	struct _pred
	{
	bool operator() (const Map::renderNode& a, const Map::renderNode& b) const
	{
	return a.m_position.y > b.m_position.y;
	}
	};
	m_renderSprites.sort_insertion(_pred());
	*/
	template<class _Pr>
	void sort_insertion(_Pr _pred)
	{
		u32 i, j;
		type t;
		for (i = 1; i < m_size; ++i)
		{
			for (j = i; j > 0 && _pred(m_data[j - 1], m_data[j]); --j)
			{
				t = m_data[j];
				m_data[j] = m_data[j - 1];
				m_data[j - 1] = t;
			}
		}
	}


	pointer m_data;
	u32     m_size;
};


#endif