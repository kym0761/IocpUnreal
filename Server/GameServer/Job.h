#pragma once


//// C++11 방식의 apply(apply는 C++17버전부터 존재함)
//template<int... Remains>
//struct seq
//{};
//
//template<int N, int... Remains>
//struct gen_seq : gen_seq<N - 1, N - 1, Remains...>
//{};
//
//template<int... Remains>
//struct gen_seq<0, Remains...> : seq<Remains...>
//{};
//
//template<typename Ret, typename... Args>
//void xapply(Ret(*func)(Args...), std::tuple<Args...>& tup)
//{
//	xapply_helper(func, gen_seq<sizeof...(Args)>(), tup);
//}
//
//template<typename F, typename... Args, int... ls>
//void xapply_helper(F func, seq<ls...>, std::tuple<Args...>& tup)
//{
//	(func)(std::get<ls>(tup)...);
//}
//
//template<typename T, typename Ret, typename... Args>
//void xapply(T* obj, Ret(T::* func)(Args...), std::tuple<Args...>& tup)
//{
//	xapply_helper(obj, func, gen_seq<sizeof...(Args)>(), tup);
//}
//
//template<typename T, typename F, typename... Args, int... ls>
//void xapply_helper(T* obj, F func, seq<ls...>, std::tuple<Args...>& tup)
//{
//	(obj->*func)(std::get<ls>(tup)...);
//}
//
//// 함수자 (Functor) 방식
//
//class IJob
//{
//public:
//	virtual void Execute() { }
//};
//
//
//
//template<typename Ret, typename... Args>
//class FuncJob : public IJob //함수를 직접 받음
//{
//	using FuncType = Ret(*)(Args...); //void StaticFunc* (Args...); 스러운 방식으로 동작할 것
//
//public:
//	FuncJob(FuncType func, Args... args) : Func(func), Tuple(args...)
//	{
//
//	}
//
//	virtual void Execute() override
//	{
//		std::apply(Func, Tuple); // C++17
//		//xapply(Func, Tuple);
//	}
//
//private:
//	FuncType Func;
//	std::tuple<Args...> Tuple; //Args... 를 받을때 사용해야함.
//};
//
//template<typename T, typename Ret, typename... Args>
//class MemberJob : public IJob //멤버(인스턴스)의 함수를 받아 사용
//{
//	using FuncType = Ret(T::*)(Args...); //void FClassName::SomeFunc(Args...) 스러운 느낌으로 동작할 것
//
//public:
//	MemberJob(T* obj, FuncType func, Args... args) 
//		: Obj(obj), Func(func), Tuple(args...)
//	{
//
//	}
//
//	virtual void Execute() override
//	{
//		//std::apply(Func, Tuple); // C++17
//		xapply(Obj, Func, Tuple);
//	}
//
//private:
//	T* Obj;
//	FuncType	Func;
//	std::tuple<Args...> Tuple;
//};
//
//
//
////~~