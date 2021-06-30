#ifndef _MI_LIST_H_
#define _MI_LIST_H_

template<typename _type>
struct miListNode
{
	miListNode() :m_left(0), m_right(0){}
	~miListNode() {}
	_type m_data;
	miListNode* m_left;
	miListNode* m_right;
};

template<typename _type>
class miList
{
	miList(const miList& other) {};
	miList(miList&& other) {};
public:
	miList():m_head(0){}
	~miList() {
		clear();
	}

	void clear()
	{
		if (!m_head)
			return;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = m_head->m_right;
			//m_allocator.destruct(m_head);
			//m_allocator.deallocate(m_head);
			///m_onDeallocate(m_head);
			//delete m_head;
			m_head->~miListNode();
			miFree(m_head);

			if (m_head == last)
				break;
			m_head = next;
		}
		m_head = nullptr;
	}

	void push_back(const _type& data)
	{
		//miListNode<_type>* node = m_allocator.allocate(1);
		//m_allocator.construct(node, miListNode<_type>());
		///miListNode<_type>* node = m_onAllocate();
		//miListNode<_type>* node = new miListNode<_type>;
		miListNode<_type>* node = (miListNode<_type>*)miMalloc(sizeof(miListNode<_type>));
		new(node)miListNode<_type>();

		node->m_data = data;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			auto last = m_head->m_left;
			last->m_right = node;
			node->m_left = last;
			node->m_right = m_head;
			m_head->m_left = node;
		}
	}

	void push_front(const _type& data)
	{
		//yyListNode<T>* node = m_allocator.allocate(1);
		//m_allocator.construct(node, yyListNode<T>());
		///miListNode<_type>* node = m_onAllocate();
		//miListNode<_type>* node = new miListNode<_type>;
		miListNode<_type>* node = (miListNode<_type>*)miMalloc(sizeof(miListNode<_type>));
		new(node)miListNode<_type>();

		node->m_data = data;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			node->m_right = m_head;
			node->m_left = m_head->m_left;
			m_head->m_left->m_right = node;
			m_head->m_left = node;
			m_head = node;
		}
	}

	void pop_front()
	{
		if (!m_head)
			return;
		auto next = m_head->m_right;
		auto last = m_head->m_left;
		//m_allocator.destruct(m_head);
		//m_allocator.deallocate(m_head);
		///m_onDeallocate(m_head);
		m_head->~miListNode();
		miFree(m_head);

		if (next == m_head)
		{
			m_head = nullptr;
			return;
		}
		m_head = next;
		next->m_left = last;
		last->m_right = next;
	}

	bool erase_first(const _type& object) {
		if (!m_head)
			return false;

		auto node = m_head;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = node->m_right;
			if (node->m_data == object)
			{
				if (node == m_head)
					pop_front();
				else
				{
					node->m_left->m_right = node->m_right;
					node->m_right->m_left = node->m_left;
					//m_allocator.destruct(node);
					//m_allocator.deallocate(node);
					///m_onDeallocate(node);
					node->~miListNode();
					miFree(node);

					// ???
					if (node == m_head)
						m_head = 0;
				}
				return true;
			}
			if (node == last)
				break;
			node = next;
		}
		return false;
	}

	void replace(const _type& oldObject, const _type& newObject) {
		if (!m_head)
			return;

		auto node = m_head;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = node->m_right;
			if (node->m_data == oldObject)
			{
				node->m_data = newObject;
				return;
			}
			if (node == last)
				break;
			node = next;
		}
	}

	miListNode<_type>* m_head;
	
	//miListNode<_type>*(*m_onAllocate)();
	//void(*m_onDeallocate)(miListNode<_type>*);
	//_allocator_type<_type> m_allocator;
};

#endif