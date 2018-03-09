#pragma once
#include <tuple>
#include <utility>
//////////////////////////////////////////////////////////////////////////
template <typename CB, typename ...ARGS>
struct pass_data_t
{
	CB cb;
	std::tuple<ARGS...> args;

	pass_data_t(CB cb, ARGS ...args)
		: cb(cb)
		, args(args...) {}
};

template <typename CB, typename TUPLE, size_t ...INDICES>
void ForwardArgumentsWithIndices(CB cb, TUPLE &args, std::integer_sequence<size_t, INDICES...>)
{
	cb(std::get<INDICES>(args)...);
}

template <typename CB, typename ...ARGS>
void ForwardArguments(void *ptr)
{
	pass_data_t<CB, ARGS...> *args = (pass_data_t<CB, ARGS...>*) ptr;
	ForwardArgumentsWithIndices(args->cb, args->args, std::make_index_sequence<sizeof...(ARGS)>());
	delete args;
}