#pragma once

namespace custom {

	template<class Container>
	class BaseIterator
	{
	private:
		template<class Type>
		struct IterationData {
			Type* _IterBegin	= nullptr;
			Type* _IterEnd		= nullptr;

			IterationData() = default;

			~IterationData() {
				_IterBegin	= nullptr;
				_IterEnd	= nullptr;
			}
		};

	public:
		using ValueType = typename Container::ValueType;
		using IterType	= typename Container::IterType;
		using Data		= IterationData<IterType>;

		IterType* _Ptr			= nullptr;
		Data* _IterationData	= nullptr;

	public:

		explicit BaseIterator(IterType* ptr, Data* data)
			:_Ptr(ptr), _IterationData(data) { }

		virtual ~BaseIterator() {
			_Ptr			= nullptr;
			_IterationData	= nullptr;
		}
	};

} // END custom::