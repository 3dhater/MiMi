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

	miListNode<_type>* find(const _type& data)
	{
		if (!m_head)
			return 0;

		auto node = m_head;
		auto last = m_head->m_left;
		while (true)
		{
			if (node->m_data == data)
				return node;

			if (node == last)
				break;
			node = node->m_right;
		}
		return 0;
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
	
	void erase(miListNode<_type>* node) {
		node->m_left->m_right = node->m_right;
		node->m_right->m_left = node->m_left;
		node->~miListNode();
		miFree(node);
		if (node == m_head)
			m_head = 0;
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

template<typename _type1, typename _type2>
struct miListNode2
{
	miListNode2() :m_left(0), m_right(0) {}
	~miListNode2() {}
	_type1 m_data1;
	_type2 m_data2;
	miListNode2* m_left;
	miListNode2* m_right;
};

template<typename _type1, typename _type2>
class miList2
{
	miList2(const miList2& other) {};
	miList2(miList2&& other) {};
public:
	miList2() :m_head(0) {}
	~miList2() {clear();}
	void clear(){
		if (!m_head)
			return;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = m_head->m_right;
			m_head->~miListNode2();
			miFree(m_head);
			if (m_head == last)
				break;
			m_head = next;
		}
		m_head = nullptr;
	}

	void insert_after(const _type1& after_this, const _type1& data1, const _type2& data2) {
		miListNode2<_type1, _type2>* node = (miListNode2<_type1, _type2>*)miMalloc(sizeof(miListNode2<_type1, _type2>));
		new(node)miListNode2<_type1, _type2>();

		node->m_data1 = data1;
		node->m_data2 = data2;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			auto c = m_head;
			auto l = c->m_left;
			while (true)
			{
				if (c->m_data1 == after_this)
				{
					auto r = c->m_right;

					node->m_left = c;
					node->m_right = r;

					c->m_right = node;
					r->m_left = node;

					return;
				}

				if (c == l)
					break;
				c = c->m_right;
			}

			auto last = m_head->m_left;
			last->m_right = node;
			node->m_left = last;
			node->m_right = m_head;
			m_head->m_left = node;
		}
	}

	void insert_before(const _type1& after_this, const _type1& data1, const _type2& data2) {
		miListNode2<_type1, _type2>* node = (miListNode2<_type1, _type2>*)miMalloc(sizeof(miListNode2<_type1, _type2>));
		new(node)miListNode2<_type1, _type2>();

		node->m_data1 = data1;
		node->m_data2 = data2;

		if (!m_head)
		{
			m_head = node;
			m_head->m_right = m_head;
			m_head->m_left = m_head;
		}
		else
		{
			auto c = m_head;
			auto l = c->m_left;
			while (true)
			{
				if (c->m_data1 == after_this)
				{
					auto l = c->m_left;

					node->m_left = l;
					node->m_right = c;

					c->m_left = node;
					l->m_right = node;

					return;
				}

				if (c == l)
					break;
				c = c->m_right;
			}

			auto last = m_head->m_left;
			last->m_right = node;
			node->m_left = last;
			node->m_right = m_head;
			m_head->m_left = node;
		}
	}

	void push_back(const _type1& data1, const _type2& data2){
		miListNode2<_type1, _type2>* node = (miListNode2<_type1, _type2>*)miMalloc(sizeof(miListNode2<_type1, _type2>));
		new(node)miListNode2<_type1, _type2>();

		node->m_data1 = data1;
		node->m_data2 = data2;

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

	void pop_front()
	{
		if (!m_head)
			return;
		auto next = m_head->m_right;
		auto last = m_head->m_left;
		m_head->~miListNode2();
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

	bool erase_first(const _type1& object) {
		if (!m_head)
			return false;

		auto node = m_head;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = node->m_right;
			if (node->m_data1 == object)
			{
				if (node == m_head)
					pop_front();
				else
				{
					node->m_left->m_right = node->m_right;
					node->m_right->m_left = node->m_left;

					node->~miListNode2();
					miFree(node);

					//// ???
					//if (node == m_head)
					//	m_head = 0;
				}
				return true;
			}
			if (node == last)
				break;
			node = next;
		}
		return false;
	}

	void replace(const _type1& oldObject, const _type1& newObject, const _type2& newObject2) {
		if (!m_head)
			return;

		auto node = m_head;
		auto last = m_head->m_left;
		while (true)
		{
			auto next = node->m_right;
			if (node->m_data1 == oldObject)
			{
				node->m_data1 = newObject;
				node->m_data2 = newObject2;
				return;
			}
			if (node == last)
				break;
			node = next;
		}
	}

	miListNode2<_type1, _type2>* m_head;
};

#endif