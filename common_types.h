#pragma once
#include <string>
#include <stdexcept>
#include <unordered_map>

typedef char int8;
typedef short int int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef float float32;
typedef double float64;
typedef long double float128;


#if ENCODING == 8
typedef std::basic_string<char, std::char_traits<char>, std::allocator<char>> String;
typedef char charT;
#define T(STRING) STRING

#elif ENCODING == 16
//typedef std::basic_string<char16_t, std::char_traits<char16_t>, std::allocator<char16_t>> String;
typedef std::u16string String;
typedef char16_t charT;
#define T(STRING) u##STRING

#elif ENCODING == 32
typedef std::basic_string<char32_t, std::char_traits<char32_t>, std::allocator<char32_t>> String;
typedef char32_t charT;
#define T(STRING) U##STRING

#else
#error Incorrect encoding size specified
#endif

template <typename _T>
struct Array {
	const static size_t INITIAL_CAPACITY = 16;

	_T* content;
	size_t max_index;
	size_t capacity;

	Array() {
		content = new _T[INITIAL_CAPACITY];
		max_index = -1;
		capacity = INITIAL_CAPACITY;
	}

	Array(size_t initial_capacity) {
		content = new _T[initial_capacity];
		max_index = -1;
		capacity = initial_capacity;
	}

	~Array() {
		delete[] content;
	}

	void init() {
		content = new _T[INITIAL_CAPACITY];
		max_index = -1;
		capacity = INITIAL_CAPACITY;
	}

	void init(size_t initial_capacity) {
		content = new _T[initial_capacity];
		max_index = -1;
		capacity = initial_capacity;
	}

	inline Array* add(_T element) {
		if (++max_index != capacity)
		{
			content[max_index] = element;

			return this;
		}
		else
		{
			_T* ptr = new _T[capacity <<= 1];
			memcpy(ptr, content, max_index + 1);
			delete[] content;
			content = ptr;
			content[max_index] = element;
			return this;
		}
	}

	inline void slice_r(size_t index) {
		for (size_t i = max_index - index; i <= max_index; ++i)
			content[i].~_T();
		max_index -= index;
	}


	inline _T get_s(size_t index) {	//Safe version
		if (index > max_index)
			throw std::out_of_range();
		return content[index];
	}

	inline _T get(size_t index) {
		return content[index];
	}

	inline _T get_r(size_t index) {
		return content[max_index - index];
	}

	inline _T& at(size_t index) {
		return content[index];
	}

	inline _T& at_r(size_t index) {
		return content[max_index - index];
	}

	inline void set(size_t index, _T element) {
		content[index] = element;
	}

	inline void set_r(size_t index, _T element) {
		content[max_index - index] = element;
	}


	inline _T& operator=(_T& type) {
		this->content = type.content;
		this->capacity = type.capacity;
		this->max_index = type.max_index;
		return this;
	}

	inline _T& operator[](size_t index) {
		return content[index];
	}
};

template<typename Key, typename Value>
using Table = std::unordered_map<Key, Value>;