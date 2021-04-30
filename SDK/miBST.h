#ifndef _MI_BST_H_
#define _MI_BST_H_

template<typename _type>
class miBinarySearchTree
{
	struct _node {
		_node() {
			m_left = m_right = 0;
			m_value = 0;
		//	printf("Constructor\n");
		}
		~_node() {
		//	printf("Destructor\n");
		}

		_type m_data;

		_node* m_left;
		_node* m_right;

		unsigned int m_value;
	};

	_node * m_root;

	void _delete_all(_node* node) {
		if (node->m_right)
			_delete_all(node->m_right);
		if (node->m_left)
			_delete_all(node->m_left);
		delete node;
	}

	void _add(_node * node, unsigned int value, const _type& data) {
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

	bool _get(_node * node, unsigned int value, _type& data) {
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

	void Add(unsigned int value, const _type& data) {
		if (!m_root)
		{
			m_root = new _node;
			m_root->m_data = data;
			m_root->m_value = value;
			return;
		}

		_add(m_root, value, data);
	}

	bool Get(unsigned int value, _type& data) {
		if(m_root)
			return _get(m_root, value, data);
		return false;
	}
};

#endif