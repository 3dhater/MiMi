#ifndef _MI_BST_H_
#define _MI_BST_H_

template<typename _type>
class miBinarySearchTree
{
	struct _node {
		_node() {
			m_left = m_right = 0;
			m_value = 0;
		}
		~_node() {
		}

		_type m_data;

		_node* m_left;
		_node* m_right;

		uint64_t m_value;
	};

	_node * m_root;

	template<typename _array>
	void _get(_array* __array, _node* node) {
		__array->push_back(node->m_data);

		if (node->m_right)
			_get(__array, node->m_right);
		if (node->m_left)
			_get(__array, node->m_left);
	}

	void _delete_all(_node* node) {
		if (node->m_right)
			_delete_all(node->m_right);
		if (node->m_left)
			_delete_all(node->m_left);
		delete node;
	}

	void _add(_node * node, uint64_t value, const _type& data) {
		if (node->m_value == value)
		{
			node->m_data = data;
			return;
		}
		if (value > node->m_value)
		{
			if (!node->m_right)
			{
				node->m_right = new _node;
				node->m_right->m_data = data;
				node->m_right->m_value = value;
			}
			else
			{
				_add(node->m_right, value, data);
			}
		}
		else
		{
			if (!node->m_left)
			{
				node->m_left = new _node;
				node->m_left->m_data = data;
				node->m_left->m_value = value;
			}
			else
			{
				_add(node->m_left, value, data);
			}
		}
	}

	bool _get(_node * node, uint64_t value, _type& data) {
		if (node->m_value == value)
		{
			data = node->m_data;
			return true;
		}

		if (value > node->m_value)
		{
			if (node->m_right)
			{
				return _get(node->m_right, value, data);
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (node->m_left)
			{
				return _get(node->m_left, value, data);
			}
			else
			{
				return false;
			}
		}
		return false;
	}

	uint64_t MurmurHash64A(const void * key, int len, unsigned int seed)
	{
		const uint64_t m = 0xc6a4a7935bd1e995;
		const int r = 47;
		uint64_t h = seed ^ (len * m);
		const uint64_t * data = (const uint64_t *)key;
		const uint64_t * end = data + (len / 8);
		while (data != end)
		{
			uint64_t k = *data++;
			k *= m;
			k ^= k >> r;
			k *= m;
			h ^= k;
			h *= m;
		}
		const unsigned char * data2 = (const unsigned char*)data;
		switch (len & 7)
		{
		case 7: h ^= uint64_t(data2[6]) << 48;
		case 6: h ^= uint64_t(data2[5]) << 40;
		case 5: h ^= uint64_t(data2[4]) << 32;
		case 4: h ^= uint64_t(data2[3]) << 24;
		case 3: h ^= uint64_t(data2[2]) << 16;
		case 2: h ^= uint64_t(data2[1]) << 8;
		case 1: h ^= uint64_t(data2[0]);
			h *= m;
		};
		h ^= h >> r;
		h *= m;
		h ^= h >> r;
		return h;
	}

public:
	miBinarySearchTree() {
		m_root = 0;
	}
	~miBinarySearchTree() {
		Clear();
	}

	void Clear() {
		if(m_root)
			_delete_all(m_root);
	}

	void Add(uint64_t value, const _type& data) {
		value = MurmurHash64A(&value, sizeof(uint64_t), 1);

		if (!m_root)
		{
			m_root = new _node;
			m_root->m_data = data;
			m_root->m_value = value;
			return;
		}

		_add(m_root, value, data);
	}

	bool Get(uint64_t value, _type& data) {
		value = MurmurHash64A(&value, sizeof(uint64_t), 1);
		
		if(m_root)
			return _get(m_root, value, data);
		return false;
	}

	template<typename _array>
	void Get(_array* __array) {
		if (m_root)
			_get(__array, m_root);
	}
};

#endif