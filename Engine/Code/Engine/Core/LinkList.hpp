#pragma once
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
// A circular doubly linked list container
//////////////////////////////////////////////////////////////////////////
template <typename T>
struct LinkNode
{
	T data;
	LinkNode<T>* sibling_left = nullptr;
	LinkNode<T>* sibling_right = nullptr;

	~LinkNode();
	const bool isValid() const { return (sibling_right != nullptr && sibling_left != nullptr); }
};

template <typename T>
LinkNode<T>::~LinkNode()
{
	sibling_left->sibling_right = sibling_right;
	sibling_right->sibling_left = sibling_left;

	sibling_left = nullptr;
	sibling_right = nullptr;
}
//////////////////////////////////////////////////////////////////////////
template <typename T>
class LinkList
{
public:
	LinkList() {}
	~LinkList() 
	{
		SAFE_DELETE(m_head);
	}

	void add(T item);
	void remove(T item);
	T* find(T item);
	bool contains(T item);
	inline bool empty() const { return nullptr == m_head; }
public:
	LinkNode<T>* m_head = nullptr;
	uint length = 0;
private:
	LinkNode<T>* findNode(T item);
};
//////////////////////////////////////////////////////////////////////////
template <typename T>
bool LinkList<T>::contains(T item)
{
	return (nullptr == find(item));
}

template <typename T>
T* LinkList<T>::find(T item)
{
	LinkNode<T>* found = findNode(item);

	if (nullptr == found)
		return nullptr;

	return found->data;
}

template <typename T>
LinkNode<T>* LinkList<T>::findNode(T item)
{
	if (empty())
		return nullptr;

	LinkNode<T>* current = m_head;
	if (current->data == item)
		return current;


	current = current->sibling_right;
	while (current != m_head)
	{
		if (current->data == item)
			return current;

		current = current->sibling_right;
	}

	return nullptr;
}

template <typename T>
void LinkList<T>::remove(T item)
{
	if (empty() || length == 0)
		return;
	LinkNode<T>* nodeToRemove = findNode(item);
	if (nullptr == nodeToRemove)
		return;
	DebuggerPrintf("Removing.\n");
	//*/
	if (nodeToRemove == m_head)
	{
		DebuggerPrintf("Removing the head.\n");
		m_head = m_head->sibling_right;
		if (nodeToRemove == m_head)
		{
			m_head = nullptr;
		}
	}
	//*/
	SAFE_DELETE(nodeToRemove);
	//*/
	if (m_head == nullptr || !m_head->isValid())
	{
		DebuggerPrintf("Setting head to nullptr.\n");
		m_head = nullptr;
	}
	//*/
	--length;
}

template <typename T>
void LinkList<T>::add(T item)
{
	LinkNode<T>* newNode = new LinkNode<T>();
	newNode->data = item;
	
	++length;
	if (nullptr == m_head)
	{
		DebuggerPrintf("Add is new head.\n");
		m_head = newNode;
		m_head->sibling_left = m_head;
		m_head->sibling_right = m_head;
		return;
	}

	DebuggerPrintf("Adding #%i.\n");
	LinkNode<T>* left = m_head->sibling_left;
	left->sibling_right = newNode;
	newNode->sibling_left = left;

	m_head->sibling_left = newNode;
	newNode->sibling_right = m_head;
}
