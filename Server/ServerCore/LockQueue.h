#pragma once

//Job을 넣을 때, 혹은 Job을 꺼낼때만 Lock을 걸면 실제로 사용할 때마다 Lock을 걸지 않아도 된다.

template<typename T>
class FLockQueue
{
public:
	void Push(T item)
	{
		WRITE_LOCK;
		Items.push(item);
	}

	T Pop()
	{
		WRITE_LOCK;
		if (Items.empty())
			return T();

		T ret = Items.front();
		Items.pop();
		return ret;
	}

	void PopAll(OUT vector<T>& items)
	{
		WRITE_LOCK;
		while (T item = Pop())
			items.push_back(item);
	}

	void Clear()
	{
		WRITE_LOCK;
		Items = queue<T>();
	}

private:
	USE_LOCK;
	queue<T> Items;
};