#ifndef _MI_MEMORY_H_
#define _MI_MEMORY_H_

template<typename _type>
class miDefaultAllocator
{
public:
	miDefaultAllocator(int) {}
	~miDefaultAllocator() {}

	_type* Allocate() {
		//return new _type();
		_type* _o = (_type*)miMalloc(sizeof(_type));
		new(_o)_type();
		return _o;
	}

	void Deallocate(_type* o) {
		assert(o);
		//delete o;
		o->~_type();
		miFree(o);
	}
};

template<typename _type>
class miPoolAllocator
{
	size_t m_freeCount;
	size_t m_poolSize;
	unsigned char* m_pool;
	void*			m_firstFree;

	miPoolAllocator():m_poolSize(0), m_pool(0), m_freeCount(0),m_firstFree(0){}
public:

	miPoolAllocator(size_t pool_size)
	{
		miPoolAllocator();

		assert(pool_size);
		m_poolSize = pool_size;

		m_pool = (unsigned char*)miMalloc(pool_size * sizeof(_type));
		unsigned char* p = m_pool;
		m_firstFree = p;
		m_freeCount = m_poolSize;
		int count = m_poolSize;
		while (--count) 
		{
			*(void**)p = (p + sizeof(_type));
			p += sizeof(_type);
		}
		*(void**)p = 0;
	}
	~miPoolAllocator() {
		if (m_pool) miFree(m_pool);
	}

	_type* Allocate() {
		assert(m_freeCount>0);
		_type* object = (_type*)m_firstFree;
		m_firstFree = *(void**)m_firstFree;
		--m_freeCount;
		new(object)_type();
		return object;
	}

	void Deallocate(_type* o) {
		assert(o);
		*(void**)o = m_firstFree;
		m_firstFree = o;
		++m_freeCount;
	}
};


#endif