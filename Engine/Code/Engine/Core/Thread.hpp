#pragma once
#include "Engine/Core/EngineBase.hpp"

//Only include the thread safe queue for those that want it. Will also include CriticalSection
#ifdef THREAD_SAFE_QUEUE
#include <queue>
#include "Engine/Core/CriticalSection.hpp"
#endif // THREAD_SAFE_QUEUE

#ifdef THREAD_SAFE_VECTOR
#include <vector>
#include "Engine/Core/CriticalSection.hpp"
#endif // THREAD_SAFE_VECTOR

//////////////////////////////////////////////////////////////////////////
#define INVALID_THREAD_HANDLE 0

typedef void(*thread_cb)(void*);
//////////////////////////////////////////////////////////////////////////
#include "Engine/Core/TemplateArgs.hpp"
//////////////////////////////////////////////////////////////////////////
namespace Thread
{
	typedef void* handle;

	handle Create(thread_cb cb, void* data);

	template <typename CB, typename ...ARGS>
	handle Create(CB entry_point, ARGS ...args)
	{
		pass_data_t<CB, ARGS...> *pass = new pass_data_t<CB, ARGS...>(entry_point, args...);
		return Create(ForwardArguments<CB, ARGS...>, (void*)pass);
	}

	//--------------------------------
	void Sleep(uint ms);
	void Yield_(); // Only has _ to help Visual Assist know it is mine

	//Release hold on this thread [one must be called per create]
	void Detach(handle th);
	void Join(handle th);

	// Test code
	void Demo(char const *path, uint byte_size);


//////////////////////////////////////////////////////////////////////////
#ifdef THREAD_SAFE_QUEUE
	//Thread Safe Queue
	template <typename T>
	class Queue
	{
	public:
		Queue() {}
		~Queue()
		{
			int a = 10;
			a = a;
			// No I don't know why this is here at all.
		}

		void push(const T &v)
		{
			SCOPE_LOCK(m_lock);
			m_queue.push(v);
		}

		bool empty()
		{
			SCOPE_LOCK(m_lock);
			bool result = m_queue.empty();

			return result;
		}

		bool pop(T *out)
		{
			SCOPE_LOCK(m_lock);

			if (m_queue.empty()) {
				return false;
			}
			else {
				*out = m_queue.front();
				m_queue.pop();
				return true;
			}
		}

		T front()
		{
			SCOPE_LOCK(m_lock);
			T result = m_queue.front();

			return result;
		}

	private:
		std::queue<T> m_queue;
		CriticalSection m_lock;
	};
#endif // THREAD_SAFE_QUEUE

#ifdef THREAD_SAFE_VECTOR
	template <typename T>
	class vector
	{
	public:
		vector() {}
		~vector()
		{
			int a = 10;
			a = a;
			// No I don't know why this is here at all.
		}


		void push(const T& v)
		{
			SCOPE_LOCK(m_lock);
			m_vector.push_back(v);
}

		bool empty()
		{
			SCOPE_LOCK(m_lock);
			bool result = m_vector.empty();

			return result;
		}

		bool pop(T *out)
		{
			SCOPE_LOCK(m_lock);

			if (m_vector.empty()) 
			{
				return false;
			}
			else 
			{
				*out = m_vector.back();
				m_vector.pop_back();
				return true;
			}
		}

		T back()
		{
			SCOPE_LOCK(m_lock);
			T result = m_vector.back();

			return result;
		}
	private:
		std::vector<T> m_vector;
		CriticalSection  m_lock;
	};
#endif //THREAD_SAFE_VECTOR

	template <typename T>
	class stack
	{
		struct node
		{
			T data;
			node next;
		};

	public:
		stack() :top(nullptr) {}

		inline bool empty()
		{
			return nullptr == top;
		}

		void push(T value)
		{
			node* n = new node();
			n->data = value;
			n->next = nullptr;
		}
	public:
		node* top;
	};
}







