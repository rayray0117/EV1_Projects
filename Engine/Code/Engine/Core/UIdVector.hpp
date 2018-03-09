#pragma once
#include <vector>
//////////////////////////////////////////////////////////////////////////
template <typename T>
class uid_vector
{
public:
	uid_vector() {}
	~uid_vector() {}
	
	reference front() { return m_vector.front(); }
	const_reference front() const { return m_vector.front(); }
	reference back() { return m_vector.back(); }
	const_reference back() const { return m_vector.back(); }
	T* data() noexcept { return m_vector.data(); }
	const T* data() const noexcept { return m_vector.data(); }

	reference operator<T>[](size_type pos) { return m_vector[pos]; }
	const_reference operator<T>[](size_type pos) const { return m_vector[pos]; }

	bool empty() const noexcept { return m_vector.empty(); }
	size_type size() const noexcept { return m_vector.size(); }
	void reserve(size_type new_cap) { m_vector.reserve(new_cap); }

	void resize(size_type count) { m_vector.resize(count); }
	void resize(size_type count, const value_type& value) { m_vector.resize(count,value); }
	size_type capacity() const noexcept { m_vector.capacity(); }
	void clear() noexcept { m_vector.clear(); }

private:
	std::vector<T> m_vector;
	std::stack<size_t> m_freeSpaces;
};