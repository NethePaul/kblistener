#pragma once
#include<memory>
template<class T>
class rpqueue {
	T*data = 0;
	int n = 0;
	int index = 0;
	int size = 0;
public:
	rpqueue(const rpqueue&) = delete;
	rpqueue(rpqueue&&r) { memcpy_s(this, sizeof(*this), &r, sizeof(r)); r.data = 0; r.index = 0; r.n = 0; }
	rpqueue(int size, int n, T*data): data(data), n(n), size(size), index(0) {};
	rpqueue(const T & p):data(new T[1]), index(0), size(1), n(1) {
		data[0] = p;
	
	};
	~rpqueue() { if (data)delete data; };

	const T&pop() {
		auto&r = data[index++];
		if (index >= size) { index = 0; n--; }
		return r;
	};
	bool empty() { return !n; };
};