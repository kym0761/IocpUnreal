#pragma once

#include "Types.h"
#include "Allocator.h"

#include <array>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

using namespace std;

template<typename Type, uint32 Size>
using Array = array<Type, Size>;

template<typename Type>
using Vector = vector<Type, STL_Allocator<Type>>;

template<typename Type>
using List = list<Type, STL_Allocator<Type>>;

template<typename Key,typename Type, typename Pred = less<Key>>
using Map = map<Key, Type, Pred, STL_Allocator<pair<const Key, Type>>>;

template<typename Key, typename Pred = less<Key>>
using Set = set<Key, Pred, STL_Allocator<Key>>;

template<typename Type>
using Deque = deque<Type, STL_Allocator<Type>>;


template<typename Type , typename Container = Deque<Type>>
using Queue = queue<Type, Container>;

template<typename Type, typename Container = Deque<Type>>
using Stack = stack<Type, Container>;

template<typename Type, typename Container = Vector<Type>, typename Pred = less<typename Container::value_type>>
using PriorityQueue = priority_queue<Type, Container, Pred>;

template<typename Key, typename Type, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using HashMap = unordered_map<Key, Type, Hasher, KeyEq, STL_Allocator<pair<const Key, Type>>>;

template<typename Key, typename Hasher = hash<Key>, typename KeyEq = equal_to<Key>>
using HashSet = unordered_map<Key, Hasher, KeyEq, STL_Allocator<Key>>;

using String = basic_string<wchar_t, char_traits<wchar_t>, STL_Allocator<wchar_t>>;

//using String = basic_string<char, char_traits<char>, STL_Allocator<char>>;
//using WString = basic_string<wchar_t, char_traits<wchar_t>, STL_Allocator<wchar_t>>;

