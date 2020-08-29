#ifndef LIST_H_
#define LIST_H_

#include "utils.h"

template <class T>
class List {
protected:
	class Node {
	public:
		Node(T _data, Node* _next = nullptr, Node* _prev = nullptr) : data(_data), next(_next), prev(_prev) {
		}
		T data;
		Node * next;
		Node * prev;
	};

	Node* head;
	Node* tail;
	unsigned n;

	void copy(const List& other);
public:
	List();
	List(const List<T>& other);
	List<T>& operator=(const List<T>& other);
	~List();
	void push_front(T data) volatile;
	void push_back(T data) volatile;
	T pop_front() volatile;
	T pop_back() volatile;

	unsigned size() const volatile;
	unsigned empty() const volatile;
	void clear() volatile;
	void print() const;


	class Iterator {
	private:
		Node* ptr;
	public:
		Iterator(Node* _ptr = nullptr) : ptr(_ptr) {}
		Iterator& operator++() { 	  // Prefix increment operator.
			ptr = ptr->next;
			return *this;
		}
		Iterator operator++(int) {	  // Postfix increment operator.
			Iterator tmp(ptr);
			ptr = ptr->next;
			return tmp;
		}

		T& operator*() {
		    return ptr->data;
		}
		const T& operator*() const {
			return ptr->data;
		}

		int exists() const {
			if (ptr == nullptr)
				return 0;
			else
				return 1;
		}
		void f();
		friend class List<T>;
	};

	Iterator begin() volatile {
		return Iterator(head);
	}
	void remove(Iterator it) volatile {
		Node* ptr = it.ptr;
		if (n == 1) {
			head = tail = nullptr;
		}
		else
			if (ptr == head) {
				head = head->next;
				head->prev = nullptr;
			}
			else
				if (ptr == tail) {
					tail = tail->prev;
					tail->next = nullptr;
				}
				else {
					ptr->prev->next = ptr->next;
					ptr->next->prev = ptr->prev;

				}

			delete ptr;

		n--;
	}
};

template <class T>
List<T>::List() : n(0), head(nullptr), tail(nullptr) {}
template <class T>
List<T>::~List() {
	clear();
}

template <class T>
void List<T>::push_front(T data) volatile {
	if (n == 0) {
		head = tail = new Node(data);
	}
	else {
		head = new Node(data, head);
		head->next->prev = head;
	}
	n++;
}
template <class T>
void List<T>::push_back(T data) volatile {
	if (n == 0) {
		head = tail = new Node(data);
	}
	else {
		tail = new Node(data, nullptr, tail);
		tail->prev->next = tail;
	}
	n++;
}
template <class T>
T List<T>::pop_front() volatile {

	Node* old = head;
	T x = head->data;
	head = head->next;
	n--;
	if (n == 0)
		tail = nullptr;
	else
		head->prev = nullptr;

	delete old;

	return x;
}
template <class T>
T List<T>::pop_back() volatile {

	Node* old = tail;
	T x = tail->data;
	tail = tail->prev;
	n--;
	if (n == 0)
		head = nullptr;
	else
		tail->next = nullptr;

	delete old;
	return x;
}

template <class T>
unsigned List<T>::size() const volatile {
	return n;
}
template <class T>
unsigned List<T>::empty() const volatile {
	if (n==0)						// essentially same as size() method
		return 1;
	else
		return 0;
}
template <class T>
void List<T>::clear() volatile {
	for (int i = 0; i < n; i++) {
		Node* old = head;
		delete old;
		head = head->next;
	}
	head = tail = nullptr;
	n = 0;
}

template <class T>
void List<T>::copy(const List<T>& other) {
	n = other.n;
	head = nullptr;
	tail = nullptr;

	Node* tmp = other.head;
	Node* prev = nullptr;

	if (tmp != nullptr) {
		prev = head = tail = new Node(tmp->data, nullptr, nullptr);
		tmp = tmp-> next;
	}
	while (tmp != nullptr) {
		tail = new Node(tmp->data, nullptr, prev);
		prev->next = tail;
		prev = tail;
		tmp = tmp->next;
	}
}
/* Copy Constructor */
template <class T>
List<T>::List(const List<T>& other) {
	copy(other);
}

/* Copy operator= */
template <class T>
List<T>& List<T>::operator=(const List<T>& other) {
	if (this != &other) {
		this->clear();
		this->copy(other);
	}
	return *this;
}


/*
#include <iostream.h>
template <class T>
void List<T>::print() const {
	Node* tmp = head;

	while (tmp) {

		cout << tmp->data << endl;

		tmp=tmp->next;
	}

}
*/


#endif
