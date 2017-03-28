#pragma once

namespace std
{
	template <typename T>
	class listitem
	{
	public:
		listitem() : _next(nullptr), _prev(nullptr) {}
		listitem(T data, listitem* ptr, bool next) : listitem(ptr, next)
		{
			_data = data;
		}
		listitem(listitem* ptr, bool next)
		{
			if (next) {
				_next = ptr;
				_prev = nullptr;
			}
			else {
				_prev = ptr;
				_next = nullptr;
			}
		}
		listitem(T data) : _next(nullptr), _prev(nullptr)
		{
			_data = data;
		}
		listitem(const listitem &other)
		{
			this->_next = other->_next;
			this->_prev = other->_prev;
			this->_data = other->_data;
		}
		~listitem() {}

		listitem<T>* _next;
		listitem<T>* _prev;
		T _data;
	};

	template <typename T>
	class list
	{
	public:
		/*
		* Iterator class
		*/
		class iterator
		{
		public:
			iterator() : _it(nullptr) {}
			iterator(const iterator &other) : _it(other._it) {}
			iterator(listitem<T>* item) : _it(item) {}

			iterator& operator++()
			{
				_it = _it->_next;
				return *this;
			}
			iterator operator++(int)
			{
				iterator res(*this);
				++(*this);
				return res;
			}
			iterator& operator--()
			{
				_it = _it->_prev;
				return *this;
			}
			iterator operator--(int)
			{
				iterator res(*this);
				--(*this);
				return res;
			}
			bool operator==(const iterator other) const
			{
				return (_it == other._it);
			}
			bool operator!=(const iterator other) const
			{
				return (_it != other._it);
			}
			T* operator->()
			{
				return &_it->_data;
			}
			T& operator*()
			{
				return _it->_data;
			}
			listitem<T>* getraw()
			{
				return _it;
			}
		private:
			listitem<T>* _it;
		};

		/*
		* Initialization
		*/
		list() : _first(nullptr), _last(nullptr), _empty(nullptr) {}

		~list()
		{
			listitem<T>* next = _first;
			while (next) {
				listitem<T>* buf = next->_next;
				delete next;
				next = buf;
			}
			next = _empty;
			while (next) {
				listitem<T>* buf = next->_next;
				delete next;
				next = buf;
			}
		}

		list& operator=(const list &other)
		{
			clear();
			iterator it = other.begin();
			if (other.size() > 0) {
				listitem<T>* item = _new_item(*it);
				if (!item)
					return false;
				_first = _last = item;
				item->_prev = nullptr;
				item->_next = nullptr;

				for (size_t i = 1; i < other.size(); ++i, ++it) {
					item->_next = _new_item(*it);
					if (!item)
						return false;
					item->_next->_prev = item;
					item = item->_next;
					item->_next = nullptr;
					_last = item;
				}
			}
		}

		bool assign(size_t count, const T &value)
		{
			clear();
			if (count > 0) {
				listitem<T>* item = _new_item(value);
				if (!item)
					return false;
				_first = _last = item;
				item->_prev = nullptr;
				item->_next = nullptr;

				for (size_t i = 1; i < count; i++) {
					item->_next = _new_item(value);
					if (!item)
						return false;
					item->_next->_prev = item;
					item = item->_next;
					item->_next = nullptr;
					_last = item;
				}
			}
			return true;
		}

		/*
		* Element access
		*/
		T& front()
		{
			return _first->_data;
		}

		T& back()
		{
			return _last->_data;
		}

		/*
		* Iterators
		*/
		iterator begin()
		{
			return iterator(_first);
		}

		iterator end()
		{
			if (_last)
				return ++iterator(_last);
			else
				return iterator(_last);
		}

		iterator last()
		{
			return iterator(_last);
		}

		/*
		* Capacity
		*/
		bool empty() const
		{
			return (_first == nullptr);
		}

		size_t size() const
		{
			size_t size = 0;
			listitem<T>* next = _first;
			while (next) {
				next = next->_next;
				size++;
			}
			return size;
		}

		/*
		* Modifiers
		*/
		void clear()
		{
			if (_first != _last)
				erase(begin(), end());
			else if (_first) {
				erase(end());
			}
		}

		iterator insert(iterator pos, const T &value)
		{
			// Allocate memory
			listitem<T>* item = _new_item(value);
			if (!item)
				return begin();

			// Insert the new item
			listitem<T>* ptr = pos.getraw();
			if (ptr->_next && ptr->_prev) {
				ptr->_prev->_next = item;
				item->_prev = ptr->_prev;
				item->_next = ptr;
				ptr->_prev = item;
			}
			else if (ptr->_next) {
				_first->_prev = item;
				item->_prev = nullptr;
				item->_next = _first;
				_first = item;
			}
			else if (ptr->_prev) {
				ptr->_prev->_next = item;
				item->_prev = ptr->_prev;
				item->_next = ptr;
				ptr->_prev = item;
			}
			else {
				_first = _last = item;
				item->_prev = nullptr;
				item->_next = nullptr;
			}
			return iterator(item);
		}

		iterator erase(iterator pos)
		{
			iterator it;

			// Remove the item from the list
			listitem<T>* ptr = pos.getraw();
			if (ptr->_next && ptr->_prev) {
				ptr->_prev->_next = ptr->_next;
				ptr->_next->_prev = ptr->_prev;
				it = ptr->_next;
			}
			else if (ptr->_next) {
				ptr->_next->_prev = nullptr;
				it = ptr->_next;
				_first = ptr->_next;
			}
			else if (ptr->_prev) {
				ptr->_prev->_next = nullptr;
				it = ptr->_prev;
				_last = ptr->_prev;
			}
			else {
				_first = _last = nullptr;
			}

			// Add the item to the empty items list
			_add_to_empty(ptr);

			return it;
		}

		iterator erase(iterator begin, iterator end)
		{
			iterator ret;
			for (iterator it = begin; it != end;)
				ret = erase(it++);
			return ret;
		}

		bool push_back(const T &value)
		{
			// Allocate memory
			listitem<T>* item = _new_item(value);
			if (!item)
				return false;
			if (_first) {
				_last->_next = item;
				item->_prev = _last;
			}
			else {
				_first = item;
				item->_prev = nullptr;
			}
			item->_next = nullptr;
			_last = item;
			return true;
		}

		void pop_back()
		{
			auto tmp = _last;
			tmp->_prev->_next = nullptr;
			_last = tmp->_prev;
			_add_to_empty(tmp);
		}

		bool push_front(const T &value)
		{
			// Allocate memory
			listitem<T>* item = _new_item(value);
			if (!item)
				return false;
			if (_first) {
				_first->_prev = item;
				item->_next = _first;
			}
			else {
				item->_next = nullptr;
				_last = item;
			}
			item->_prev = nullptr;
			_first = item;
			return true;
		}

		void pop_front()
		{
			auto tmp = _first;
			tmp->_next->_prev = nullptr;
			_first = tmp->_next;
			_add_to_empty(tmp);
		}

	private:
		listitem<T>* _new_item(const T &value)
		{
			// Allocate new memory if necessary
			listitem<T>* item;
			if (_empty) {
				item = _empty;
				if (_empty->_next) {
					_empty = _empty->_next;
					_empty->_prev = nullptr;
				}
				else
					_empty = nullptr;
				item->_data = value;
				item->_prev = nullptr;
				item->_next = nullptr;
			}
			else {
				item = new listitem<T>(value);
			}
			return item;
		}

		void _add_to_empty(listitem<T>* ptr)
		{
			if (_empty)
				ptr->_next = _empty;
			else
				ptr->_next = nullptr;
			ptr->_prev = nullptr;
			_empty = ptr;
		}

		listitem<T>* _first;
		listitem<T>* _last;
		listitem<T>* _empty;
	};
}