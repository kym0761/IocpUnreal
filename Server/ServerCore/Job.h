#pragma once
#include <functional>

using CallbackType = std::function<void()>;

class FJob
{
public:
	FJob(CallbackType&& callback) : Callback(std::move(callback))
	{
	}

	template<typename T, typename Ret, typename... Args>
	FJob(shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		Callback = [owner, memFunc, args...]()
			{
				(owner.get()->*memFunc)(args...);
			};
	}

	void Execute()
	{
		Callback();
	}

private:
	CallbackType Callback;
};
