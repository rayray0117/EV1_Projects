#pragma once
#include "Engine/Core/EngineBase.hpp"
//////////////////////////////////////////////////////////////////////////
template <typename T>
class Singleton
{
public:
	static T* GetInstance()
	{
		if (nullptr == s_instance)
		{
			s_instance = new /*(s_buffer)*/ T();
		}

		return s_instance;
	}

public:
	static T* s_instance;
	//static size_t s_buffer[sizeof(T)];
};
//////////////////////////////////////////////////////////////////////////
template<typename T>
T* Singleton<T>::s_instance;
/*/
template<typename T>
T* Singleton<T>::s_buffer[sizeof(T)];
//*/