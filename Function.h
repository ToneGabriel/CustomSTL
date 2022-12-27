#pragma once

template<class Type, class... Args>
class Function
{
private:
	using RetType = Type;


public:
	Function() noexcept {}

	template<class RetType>
	Function(RetType&& func);


};

