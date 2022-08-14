#pragma once
template<class T,int capacity>
class CircularBuffer
{
	
	int index=0;
	int msize = 0;
public:
	T data[capacity]{};

	void size()const { return msize; };
	void push(const T&);
	void pop(int i = 1);
	const T&get(int i = 1)const;
	void set(const T&val, int i = 1);
	bool cpr(const T*cprTo)const;
	bool cpr(const T*cprTo, int length)const;
};

template<class T, int capacity>
inline void CircularBuffer<T, capacity>::push(const T &p)
{
	data[index] = p;
	index++;
	index %= capacity;
	msize++;
	if (msize >= capacity)msize = capacity;
}

template<class T, int capacity>
inline void CircularBuffer<T, capacity>::pop(int i)
{
	if (msize<i)return;
	msize-=i;
	index-=i;
	if (index < 0)index = capacity + index;
	data[index] = 0;
}

template<class T, int capacity>
inline const T & CircularBuffer<T, capacity>::get(int i)const
{
	if (msize < i)return {};
	auto in=index-i;
	if (in < 0)in = capacity + in;
	return data[in];
}
template<class T, int capacity>
inline void CircularBuffer<T, capacity>::set(const T&val, int i)
{
	if (msize < i)return;
	auto in = index - i;
	if (in < 0)in = capacity + in;
	data[in]=val;
}
template<class T, int capacity>
bool CircularBuffer<T, capacity>::cpr(const T * cprTo)const
{
	int i = 0;
	for (; cprTo[i]; i++);
	return cpr(cprTo, i);
}

template<class T, int capacity>
bool CircularBuffer<T, capacity>::cpr(const T * cprTo, int length)const
{
	if (length > msize)return false;
	for (int i = 0; i < length; i++)
		if (get(length - i) != cprTo[i])return false;
	return true;
}