#pragma once

#include <iostream>

namespace spk
{
	template <typename TType>
	class SafePointer
	{
	public:
		SafePointer(TType* p_ptr = nullptr) : _ptr(p_ptr) {}

		~SafePointer() = default;
		SafePointer(const SafePointer&) = default;
		SafePointer& operator=(const SafePointer&) = default;

		operator TType* () const
		{
			return (_ptr);
		}

		TType& operator*() const { return (*_ptr); }
		TType* operator->() const { return (_ptr); }
		TType* get() const { return (_ptr); }

		bool operator == (SafePointer<TType> p_other) const
		{
			return (p_other.get() == _ptr);
		}

		bool operator != (SafePointer<TType> p_other) const
		{
			return (p_other.get() != _ptr);
		}

		bool operator == (TType* p_other) const
		{
			return (p_other == _ptr);
		}

		bool operator != (TType* p_other) const 
		{
			return (p_other != _ptr);
		}

		friend std::ostream& operator << (std::ostream& p_os, const spk::SafePointer<TType>& p_ptr)
		{
			p_os << p_ptr.get();
			return (p_os);
		}

		friend std::wostream& operator << (std::wostream& p_os, const spk::SafePointer<TType>& p_ptr)
		{
			p_os << p_ptr.get();
			return (p_os);
		}

	private:
		TType* _ptr;
	};
}