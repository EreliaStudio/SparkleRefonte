#pragma once

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

		TType& operator*() const { return (*_ptr); }
		TType* operator->() const { return (_ptr); }
		TType* get() const { return (_ptr); }

		bool operator == (const TType* p_other) const
		{
			return (p_other == _ptr);
		}

		bool operator != (const TType* p_other) const
		{
			return (p_other != _ptr);
		}

	private:
		TType* _ptr;
	};
}