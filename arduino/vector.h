#pragma once

namespace std
{
	template <typename T>
	class vector
	{
	public:
		/*
		* Iterator class
		*/
		typedef T* iterator;

		/*
		* Initialization
		*/
		vector() : _data(nullptr), _size(0), _elem(0) {}

		~vector()
		{
			delete[] _data;
		}

		vector& operator= (const vector &other)
		{
			if (_size < other._elem && !expand_mem(other._elem))
				return vector();
			memcpy(_data, other._data, _elem*sizeof(T));
		}

		bool assign(size_t count, const T &value)
		{
			if (_size < count && !expand_mem(count))
				return false;
			for (_elem = 0; _elem < count; _elem++)
				_data[_elem] = value;
			return true;
		}

		/*
		* Element access
		*/
		T& operator[](size_t pos)
		{
			return _data[pos];
		}

		T& front()
		{
			return _data[0];
		}

		T& back()
		{
			return _data[_elem-1];
		}

		const T* data() const
		{
			return _data;
		}

		/*
		* Iterators
		*/
		iterator begin()
		{
			return &_data[0];
		}

		iterator end()
		{
			return &_data[_elem];
		}

		/*
		* Capacity
		*/
		bool empty()
		{
			return (_elem == 0);
		}

		size_t size()
		{
			return _elem;
		}

		void shrink_to_fit()
		{
			if (_elem > 0) {
				_size = _elem;
				T* tmp = new T[_elem];
				memcpy(tmp, _data, _elem*sizeof(T));
				delete[] _data;
				_data = tmp;
			}
			else {
				_size = 0;
				delete[] _data;
			}
		}

		/*
		* Modifiers
		*/
		void clear()
		{
			_elem = 0;
		}

		iterator insert(iterator pos, const T &value)
		{
			size_t index = static_cast<size_t>(pos-_data);
			if (_elem == _size && !expand_mem())
				return false;

			memmove(&_data[index+1], &_data[index], (_elem-index)*sizeof(T));
			_data[index] = value;
			_elem++;
			return pos;
		}

		iterator erase(iterator pos)
		{
			size_t index = static_cast<size_t>(pos-_data);
			memmove(pos, pos+1, (--_elem-index)*sizeof(T));
			return pos;
		}

		iterator erase(iterator begin, iterator end)
		{
			size_t index = static_cast<size_t>(begin-_data);
			size_t amount = static_cast<size_t>(end-begin);
			memmove(begin, begin+(end-begin), (_elem-index-amount)*sizeof(T));
			_elem -= static_cast<size_t>(end-begin);
			return begin;
		}

		bool push_back(const T &value)
		{
			if (_size == _elem && !expand_mem())
				return false;
			_data[_elem++] = value;
			return true;
		}

		void pop_back()
		{
			--_elem;
		}

	private:
		bool expand_mem()
		{
			if (!_size) {
				_size = 2;
				_data = new T[_size];
			}
			else {
				_size *= 2;
				T* tmp = new T[_size];
				if (!tmp)
					return false;
				memcpy(tmp, _data, _elem*sizeof(T));
				delete[] _data;
				_data = tmp;
			}
			return (_data != nullptr);
		}

		bool expand_mem(size_t room)
		{
			if (!_size) {
				_size = room;
				_data = new T[_size];
			}
			else {
				do { _size *= 2; } while (_size < room);
				T* tmp = new T[_size];
				if (!tmp)
					return false;
				memcpy(tmp, _data, _elem*sizeof(T));
				delete[] _data;
				_data = tmp;
			}
			return (_data != nullptr);
		}

		T* _data;
		size_t _size, _elem;
	};
}