#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

namespace sjtu {

template <typename T>
class vector {
private:
	T *data_ = nullptr;
	size_t size_ = 0;
	size_t cap_ = 0;

	static T *allocate(size_t n) {
		if (n == 0) return nullptr;
		void *p = ::operator new(n * sizeof(T));
		return static_cast<T *>(p);
	}
	static void deallocate(T *p) {
		if (p) ::operator delete(static_cast<void *>(p));
	}
	void destroy_range(size_t l, size_t r) {
		for (size_t i = l; i < r; ++i) data_[i].~T();
	}
	void ensure_capacity(size_t need) {
		if (cap_ >= need) return;
		size_t new_cap = cap_ ? cap_ : 1;
		while (new_cap < need) new_cap <<= 1;
		T *nd = allocate(new_cap);
		for (size_t i = 0; i < size_; ++i) new (nd + i) T(data_[i]);
		destroy_range(0, size_);
		deallocate(data_);
		data_ = nd;
		cap_ = new_cap;
	}

public:
	class const_iterator;
	class iterator {
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T *;
		using reference = T &;
		struct _iter_cat {};
		using iterator_category = _iter_cat;
	private:
		vector<T> *owner = nullptr;
		size_t pos = 0;
		iterator(vector<T> *o, size_t p) : owner(o), pos(p) {}
		friend class vector<T>;
		friend class const_iterator;
	public:
		iterator() = default;
		iterator operator+(const int &n) const { return iterator(owner, pos + static_cast<size_t>(n)); }
		iterator operator-(const int &n) const { return iterator(owner, pos - static_cast<size_t>(n)); }
		int operator-(const iterator &rhs) const {
			if (owner != rhs.owner) throw invalid_iterator();
			return static_cast<int>(pos) - static_cast<int>(rhs.pos);
		}
		iterator &operator+=(const int &n) { pos += static_cast<size_t>(n); return *this; }
		iterator &operator-=(const int &n) { pos -= static_cast<size_t>(n); return *this; }
		iterator operator++(int) { iterator tmp = *this; ++pos; return tmp; }
		iterator &operator++() { ++pos; return *this; }
		iterator operator--(int) { iterator tmp = *this; --pos; return tmp; }
		iterator &operator--() { --pos; return *this; }
		T &operator*() const { return owner->data_[pos]; }
		bool operator==(const iterator &rhs) const { return owner == rhs.owner && pos == rhs.pos; }
		bool operator==(const const_iterator &rhs) const { return owner == rhs.owner && pos == rhs.pos; }
		bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
		bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
	};

	class const_iterator {
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T *;
		using reference = T &;
		struct _iter_cat {};
		using iterator_category = _iter_cat;
	private:
		const vector<T> *owner = nullptr;
		size_t pos = 0;
		const_iterator(const vector<T> *o, size_t p) : owner(o), pos(p) {}
		friend class vector<T>;
		friend class iterator;
	public:
		const_iterator() = default;
		const_iterator operator+(const int &n) const { return const_iterator(owner, pos + static_cast<size_t>(n)); }
		const_iterator operator-(const int &n) const { return const_iterator(owner, pos - static_cast<size_t>(n)); }
		int operator-(const const_iterator &rhs) const {
			if (owner != rhs.owner) throw invalid_iterator();
			return static_cast<int>(pos) - static_cast<int>(rhs.pos);
		}
		const_iterator &operator+=(const int &n) { pos += static_cast<size_t>(n); return *this; }
		const_iterator &operator-=(const int &n) { pos -= static_cast<size_t>(n); return *this; }
		const_iterator operator++(int) { const_iterator tmp = *this; ++pos; return tmp; }
		const_iterator &operator++() { ++pos; return *this; }
		const_iterator operator--(int) { const_iterator tmp = *this; --pos; return tmp; }
		const_iterator &operator--() { --pos; return *this; }
		const T &operator*() const { return owner->data_[pos]; }
		bool operator==(const const_iterator &rhs) const { return owner == rhs.owner && pos == rhs.pos; }
		bool operator==(const iterator &rhs) const { return owner == rhs.owner && pos == rhs.pos; }
		bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
		bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
	};

		
	vector() = default;
	vector(const vector &other) : data_(nullptr), size_(0), cap_(0) {
		ensure_capacity(other.size_);
		for (size_t i = 0; i < other.size_; ++i) new (data_ + i) T(other.data_[i]);
		size_ = other.size_;
	}
	~vector() {
		destroy_range(0, size_);
		deallocate(data_);
	}
	vector &operator=(const vector &other) {
		if (this == &other) return *this;
		if (cap_ < other.size_) {
			T *nd = allocate(other.size_);
			for (size_t i = 0; i < other.size_; ++i) new (nd + i) T(other.data_[i]);
			destroy_range(0, size_);
			deallocate(data_);
			data_ = nd;
			cap_ = other.size_;
			size_ = other.size_;
			return *this;
		}
		if (size_ >= other.size_) {
			for (size_t i = 0; i < other.size_; ++i) data_[i] = other.data_[i];
			for (size_t i = other.size_; i < size_; ++i) data_[i].~T();
			size_ = other.size_;
		} else {
			for (size_t i = 0; i < size_; ++i) data_[i] = other.data_[i];
			for (size_t i = size_; i < other.size_; ++i) new (data_ + i) T(other.data_[i]);
			size_ = other.size_;
		}
		return *this;
	}

	T &at(const size_t &pos) {
		if (pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}
	const T &at(const size_t &pos) const {
		if (pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}
	T &operator[](const size_t &pos) {
		if (pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}
	const T &operator[](const size_t &pos) const {
		if (pos >= size_) throw index_out_of_bound();
		return data_[pos];
	}
	const T &front() const {
		if (size_ == 0) throw container_is_empty();
		return data_[0];
	}
	const T &back() const {
		if (size_ == 0) throw container_is_empty();
		return data_[size_ - 1];
	}
	iterator begin() { return iterator(this, 0); }
	const_iterator begin() const { return const_iterator(this, 0); }
	const_iterator cbegin() const { return const_iterator(this, 0); }
	iterator end() { return iterator(this, size_); }
	const_iterator end() const { return const_iterator(this, size_); }
	const_iterator cend() const { return const_iterator(this, size_); }
	bool empty() const { return size_ == 0; }
	size_t size() const { return size_; }
	void clear() {
		destroy_range(0, size_);
		size_ = 0;
	}
	iterator insert(iterator pos, const T &value) {
		if (pos.owner != this) throw invalid_iterator();
		size_t idx = pos.pos;
		if (idx > size_) throw invalid_iterator();
		ensure_capacity(size_ + 1);
		if (idx == size_) {
			new (data_ + size_) T(value);
			++size_;
			return iterator(this, idx);
		}
		new (data_ + size_) T(data_[size_ - 1]);
		for (size_t i = size_ - 1; i > idx; --i) data_[i] = data_[i - 1];
		data_[idx] = value;
		++size_;
		return iterator(this, idx);
	}
	iterator insert(const size_t &ind, const T &value) {
		if (ind > size_) throw index_out_of_bound();
		return insert(iterator(this, ind), value);
	}
	iterator erase(iterator pos) {
		if (pos.owner != this) throw invalid_iterator();
		size_t idx = pos.pos;
		if (idx >= size_) throw invalid_iterator();
		for (size_t i = idx; i + 1 < size_; ++i) data_[i] = data_[i + 1];
		data_[size_ - 1].~T();
		--size_;
		return iterator(this, idx < size_ ? idx : size_);
	}
	iterator erase(const size_t &ind) {
		if (ind >= size_) throw index_out_of_bound();
		return erase(iterator(this, ind));
	}
	void push_back(const T &value) {
		ensure_capacity(size_ + 1);
		new (data_ + size_) T(value);
		++size_;
	}
	void pop_back() {
		if (size_ == 0) throw container_is_empty();
		data_[size_ - 1].~T();
		--size_;
	}
};

}  // namespace sjtu

#endif
