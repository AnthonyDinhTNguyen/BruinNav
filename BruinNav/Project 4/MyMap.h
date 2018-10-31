// MyMap.h
#include "provided.h"
#include "support.h"
// Skeleton for the MyMap class template.  You must implement the first six
// member functions.
#ifndef MyMapIn
#define MyMapIn

template<typename KeyType, typename ValueType>
class MyMap
{
public:

	MyMap() { head = nullptr; m_size = 0; }
	~MyMap() { deleteAll(head); }
	void clear() 
	{
		if (head != nullptr)
			deleteAll(head);
		head = nullptr;
	}
	int size() const { return m_size; }
	void associate(const KeyType& key, const ValueType& value)
	{
		if (Search(key,head))//if the association is already there
		{
			ValueType* temp = find(key);//just associate the key with the new value
			*temp = value;
			return;
		}
		else if (head == nullptr)//if the map is empty
		{
			head = new Node(key, value);
			m_size++;
			return;
		}
		else
		{
			Node* cur = head;
			for (;;)//searching for the spot that it belongs
			{
				if (key == cur->first)//doing a normal search of a BST
					return;
				if (key < cur->first)
				{
					if (cur->m_left != nullptr)
						cur = cur->m_left;
					else
					{
						cur->m_left = new Node(key, value);//once we get to the bottom, where the new node beelongs add in the node
						m_size++;
						return;
					}
				}
				else if (key > cur->first)//doing the same on the other side
				{
					if (cur->m_right != nullptr)
						cur = cur->m_right;
					else
					{
						cur->m_right = new Node(key, value);
						m_size++;
						return;
					}
				}
			}
		}
	}

	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const
	{
		Node*ptr = head;
		while (ptr != nullptr)//do a search of the BST
		{
			if (key == ptr->first)//return when you find the value
				return &(ptr->second);
			else if (key < ptr->first)
				ptr = ptr->m_left;
			else
				ptr = ptr->m_right;
		}
		return nullptr;//otherwise return NULL

	}

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:

	struct Node
	{
		Node(KeyType a, ValueType b) :first(a), second(b) { m_left = nullptr; m_right = nullptr; }
		KeyType first;
		ValueType second;
		Node* m_left;
		Node* m_right;
	};
	Node* head;
	void deleteAll(Node* begin)//helper function
	{
		if (begin == nullptr)
			return;
		if (begin->m_left != nullptr)//recursively call delete on each subtree till we get to the bottom
			deleteAll(begin->m_left);
		if (begin->m_right != nullptr)
			deleteAll(begin->m_right);
		delete begin;//then delete the nodes
	}
	int m_size;
	bool Search(KeyType V, Node* ptr) 
	{
		while (ptr != nullptr)//normal BST search algorithm learned in class
		{
			if (V == ptr->first)
				return true;
			else if (V < ptr->first)
				ptr = ptr->m_left;
			else
				ptr = ptr->m_right;
		}
		return false;
	}
};

#endif // !MyMapIn

