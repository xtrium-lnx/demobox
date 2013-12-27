#pragma once

#include <functional>
#include <map>
#include <vector>

namespace xtd
{
	template<typename O, typename R, typename ...A>
	std::function<R(A...)> slot(O* o, R(O::*f)(A...))
	{
		return [=](A... args) { return (o->*f)(args...); };
	}

	template<typename T, typename... R>
	class signal
	{
	private:
		std::vector< std::function<void(T, R...)> > m_slots;

	public:
		void emit(T t, R... r)
		{
			for (unsigned int i = 0; i < m_slots.size(); i++)
				m_slots[i](t, r...);
		}

		void operator()(T t, R... r)
		{
			emit(t, r...);
		}

		void connect(const std::function<void(T, R...)>& s)
		{
			m_slots.push_back(s);
		}

		void disconnectAll()
		{
			m_slots.clear();
		}
	};

	//! Implementation of a Qt-less Signal with no argument
	template<>
	class signal<void>
	{
	private:
		std::vector< std::function<void()> > m_slots;

	public:
		void emit()
		{
			for (auto it = m_slots.begin(); it != m_slots.end(); it++)
				(*it)();
		}

		void operator()()
		{
			emit();
		}

		void connect(const std::function<void()>& s)
		{
			m_slots.push_back(s);
		}

		void disconnectAll()
		{
			m_slots.clear();
		}
	};
}
