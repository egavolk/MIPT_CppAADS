#pragma once

template <typename... Types>
class Tuple;

template <typename FirstType, typename... Types>
class Tuple<FirstType, Types...> {
public:
	typedef Tuple <Types...> MyBaseClass;
	typedef Tuple<FirstType, Types...> MyClass;

	Tuple() 
			: base_(new MyBaseClass())
			, data_() {

	}

	explicit Tuple(const FirstType& firstArg, const Tuple<Types...>& tuple)
			: data_(firstArg)
			, base_(new MyBaseClass(tuple))	{

	}

	explicit Tuple(FirstType&& firstArg, Tuple<Types...>&& tuple)
		: data_(firstArg)
		, base_(new MyBaseClass(tuple)) {

	}

	explicit Tuple(const FirstType& firstArg, const Types&... args)
			: data_(firstArg)
			, base_(new MyBaseClass(args...)) {

	}

	explicit Tuple(FirstType&& firstArg, Types&&... args)
			: data_(std::move(firstArg))
			, base_(new MyBaseClass(std::move(args)...)) {
		firstArg = FirstType();
	}

	template < typename ...Types >
	Tuple(FirstType&& first, Types&&... others)
		: data_(std::forward<FirstType>(first))
		, base_(new MyBaseClass(std::forward<Types>(others)...)) {

	}

	Tuple(const MyClass& other)
			: data_(other.data_)
			, base_(new MyBaseClass(*(other.base_))) {
		
	}

	Tuple(MyClass&& other)
			: data_(other.data_)
			, base_(other.base_) {
		other.data_ = FirstType();
		other.base_ = nullptr;
	}
	
	
	
	MyClass& operator=(const MyClass& other) {
		if (this != &other)	{
			delete base_;

			base_ = new MyBaseClass(*other.base_);
			data_ = other.data_;

			return *this;
		}
	}
	
	MyClass& operator=(MyClass&& other) {
		delete base_;

		base_ = other.base_;
		data_ = std::move(other.data_);

		other.base_ = nullptr;
		other.data_ = FirstType();

		return *this;
	}
	
	~Tuple() {
		delete base_;
	}

	void swap(MyClass& other) {
		FirstType cur(std::move(data_));
		data_ = std::move(other.data_);
		other.data_ = std::move(cur);
		std::swap(base_, other.base_);
	}

	FirstType& GetData() {
		return data_;
	}

	MyBaseClass* GetBase() {
		return base_;
	}

//private:
	FirstType data_;
	MyBaseClass* base_;
};

template <>
class Tuple<> {
public:
	Tuple() {}

	Tuple(const Tuple<>& other) {}

	Tuple<>& operator=(const Tuple<>& other) { return *this; }

	int& GetData() {
		return data_;
	}

	void* GetBase() {
		return base_;
	}

//private:
	int data_ = 0;
	int* base_ = nullptr;
};

template <typename...Types>
bool operator<(const Tuple<Types...>& a, const Tuple<Types...>& b) {
	if (!sizeof...(Types))
		return false;
	if (a.data_ < b.data_)
		return true;
	if (a.data_ > b.data_)
		return false;
	return ((*(a.base_)) < (*(b.base_)));
}

template <typename...Types>
bool operator>(const Tuple<Types...>& a, const Tuple<Types...>& b) {
	return b < a;
}

template <typename...Types>
bool operator==(const Tuple<Types...>& a, const Tuple<Types...>& b) {
	return !(b < a) && !(a < b);
}

template <typename...Types>
bool operator<=(const Tuple<Types...>& a, const Tuple<Types...>& b) {
	return ((a < b) || (a == b));
}

template <typename...Types>
bool operator>=(const Tuple<Types...>& a, const Tuple<Types...>& b) {
	return ((b < a) || (a == b));
}

template <typename...Types>
bool operator!=(const Tuple<Types...>& a, const Tuple<Types...>& b) {
	return (!(a == b));
}

template <typename... Types>
auto makeTuple(Types&&... args) {
	return Tuple<std::decay_t<Types>...>(std::forward<Types>(args)...);
}

template<size_t I, typename... Types, class = std::enable_if_t<I != 0>>
	constexpr decltype(auto) get(const Tuple<Types...>& tuple) {
	return get<I - 1>(*tuple.GetBase());
}

template<size_t I, typename... Types, class = std::enable_if_t<I != 0>>
	constexpr decltype(auto) get(Tuple<Types...>&& tuple) {
	return get<I - 1>(std::move*tuple.GetBase());
}

template<size_t I, typename... Types, class = std::enable_if_t<I != 0>>
constexpr decltype(auto) get(Tuple<Types...>& tuple) {
	return get<I - 1>(*tuple.GetBase());
}

template<size_t I, typename... Types>
constexpr decltype(auto) get(const Tuple<Types...>& tuple, typename std::enable_if_t<I == 0>* = 0) {
	return tuple.GetData();
}

template<size_t I, typename... Types>
constexpr decltype(auto) get(Tuple<Types...>&& tuple, typename std::enable_if_t<I == 0>* = 0) {
	return std::move(tuple.GetData());
}

template<size_t I, typename... Types>
constexpr decltype(auto) get(Tuple<Types...>& tuple, typename std::enable_if_t<I == 0>* = 0) {
	return tuple.GetData();
}


template<typename LookForType, typename FirstType, typename... Types, class = std::enable_if_t<!std::is_same<FirstType, std::decay_t<LookForType>>::value>>
	constexpr decltype(auto) get(Tuple<FirstType, Types...>& tuple) {
	return get<LookForType>(*tuple.GetBase());
}

template<typename LookForType, typename FirstType, typename... Types, class = std::enable_if_t<!std::is_same<FirstType, std::decay_t<LookForType>>::value>>
	constexpr decltype(auto) get(Tuple<FirstType, Types...>&& tuple) {
	return get<LookForType>(*tuple.GetBase());
}

template<typename LookForType, typename FirstType, typename... Types, class = std::enable_if_t<!std::is_same<FirstType, std::decay_t<LookForType>>::value>>
	constexpr decltype(auto) get(const Tuple<FirstType, Types...>& tuple) {
	return get<LookForType>(*tuple.GetBase());
}

template<typename LookForType, typename FirstType, typename...Types>
constexpr decltype(auto) get(Tuple<FirstType, Types...>& tuple, typename std::enable_if_t<(std::is_same<FirstType, std::decay_t<LookForType>>::value)>* = 0) {
	return tuple.GetData();
}

template<typename LookForType, typename FirstType, typename...Types>
constexpr decltype(auto) get(Tuple<FirstType, Types...>&& tuple, typename std::enable_if_t<(std::is_same<FirstType, std::decay_t<LookForType>>::value)>* = 0) {
	return std::move(tuple.GetData());
}

template<typename LookForType, typename FirstType, typename...Types>
constexpr decltype(auto) get(const Tuple<FirstType, Types...>& tuple, typename std::enable_if_t<(std::is_same<FirstType, std::decay_t<LookForType>>::value)>* = 0) {
	return tuple.GetData();
}
/*
template <typename FirstType, typename... Others1, typename... Others2>
auto catTwo(Tuple<FirstType, Others1...>&& tuple1, Tuple<Others2...>&& tuple2) {
	return Tuple<FirstType, Others1..., Others2...>(std::forward<FirstTake>(tuple1.GetData()), catTwo(std::forward<Others1>(*tuple1.GetBase())..., std::forward<Others2>(tuple2)...));
}

template<typename Tuple1, typename Tuple2>
typename JoinTwo<Tuple1, Tuple2>::type catTwo(Tuple1&& tuple1, Tuple2&& tuple2) {

}

template<typename T1, typename T2>
struct JoinTwo {};

template<typename... Types1, typename... Types2>
struct JoinTwo<Tuple<Types1...>, Tuple<Types2...> >
{
	typedef Tuple<Type1..., Types2...> type;
};

template <typename ...Others>
auto catTwo(const Tuple<> &emptyTuple, Tuple<Others...>&& tuple) {
	return std::forward<Others>(tuple)...;
}

template <typename FirstTuple, typename ...Tuples>
auto tupleCat(FirstTuple&& first, Tuples&& ...others) {
	return catTwo(std::forward<FirstTuple>(first), tupleCat(std::forward<Tuples>(others)...));
}

template <typename Tuple>
auto tupleCat(Tuple&& tuple) {
	return std::forward<Tuple>(tuple);
}*/

template <typename FirstType, typename... Others1, typename... Others2>
decltype(auto) catTwo(const Tuple<FirstType, Others1...>& tuple1, const Tuple<Others2...> &tuple2) {
	return Tuple<FirstType, Others1..., Others2...>(tuple1.data_, catTwo(*tuple1.base_, tuple2));
}

template <typename ...Others>
decltype(auto) catTwo(const Tuple<> &emptyTuple, const Tuple<Others...> &tuple) {
	return tuple;
}

template <typename firstTuple, typename ...Tuples>
decltype(auto) tupleCat(const firstTuple &first, const Tuples& ...others) {
	return catTwo(first, tupleCat(others...));
}

template <typename Tuple>
decltype(auto) tupleCat(const Tuple& tuple) {
	return tuple;
}