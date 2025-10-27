#ifndef SHAPELIST_H
#define SHAPELIST_H


// minimal move helper (replacement for std::move)
template<typename U>
inline U&& _move(U& u) noexcept { return static_cast<U&&>(u); }

template<typename T>
class ShapeList {
private:
    struct Node {
        T value;
        Node* next;
        Node(const T& v) : value(v), next(nullptr) {}
        Node(T&& v) : value(_move(v)), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    using size_type = unsigned long;
    size_type count;

    void clear_nodes() {
        while (head) {
            Node* n = head;
            head = head->next;
            delete n;
        }
        tail = nullptr;
        count = 0;
    }

public:
    ShapeList() noexcept : head(nullptr), tail(nullptr), count(0) {}
    ~ShapeList() { clear_nodes(); }

    // copy
    ShapeList(const ShapeList& other) : head(nullptr), tail(nullptr), count(0) {
        for (Node* cur = other.head; cur; cur = cur->next) push_back(cur->value);
    }
    ShapeList& operator=(const ShapeList& other) {
        if (this == &other) return *this;
        clear_nodes();
        for (Node* cur = other.head; cur; cur = cur->next) push_back(cur->value);
        return *this;
    }

    // move
    ShapeList(ShapeList&& o) noexcept : head(o.head), tail(o.tail), count(o.count) {
        o.head = o.tail = nullptr; o.count = 0;
    }
    ShapeList& operator=(ShapeList&& o) noexcept {
        if (this == &o) return *this;
        clear_nodes();
        head = o.head; tail = o.tail; count = o.count;
        o.head = o.tail = nullptr; o.count = 0;
        return *this;
    }

    bool empty() const noexcept { return head == nullptr; }
    size_type size() const noexcept { return count; }

    void push_front(const T& v) {
        Node* n = new Node(v);
        n->next = head;
        head = n;
        if (!tail) tail = n;
        ++count;
    }
    void push_front(T&& v) {
        Node* n = new Node(_move(v));
        n->next = head;
        head = n;
        if (!tail) tail = n;
        ++count;
    }

    void push_back(const T& v) {
        Node* n = new Node(v);
        if (tail) tail->next = n;
        else head = n;
        tail = n;
        ++count;
    }
    void push_back(T&& v) {
        Node* n = new Node(_move(v));
        if (tail) tail->next = n;
        else head = n;
        tail = n;
        ++count;
    }

    void pop_front() {
        if (!head) return;
        Node* n = head;
        head = head->next;
        delete n;
        if (!head) tail = nullptr;
        --count;
    }

    // remove elements matching predicate pred(T&) -> bool
    template<typename Pred>
    void remove_if(Pred pred) {
        // remove from head
        while (head && pred(head->value)) {
            Node* t = head;
            head = head->next;
            delete t;
            --count;
        }
        if (!head) { tail = nullptr; return; }

        Node* cur = head;
        while (cur->next) {
            if (pred(cur->next->value)) {
                Node* t = cur->next;
                cur->next = t->next;
                if (t == tail) tail = cur;
                delete t;
                --count;
            } else {
                cur = cur->next;
            }
        }
    }

    template<typename Fn>
    void for_each(Fn f) {
        for (Node* cur = head; cur; cur = cur->next) f(cur->value);
    }

    // simple forward iterator (does not rely on <iterator>)
    struct iterator {
        using value_type = T;
        using reference = T&;
        using pointer = T*;

        Node* node;
        iterator(Node* n = nullptr) : node(n) {}
        reference operator*() const { return node->value; }
        pointer operator->() const { return &node->value; }
        iterator& operator++() { node = node->next; return *this; }
        iterator operator++(int) { iterator tmp = *this; node = node->next; return tmp; }
        bool operator==(const iterator& o) const { return node == o.node; }
        bool operator!=(const iterator& o) const { return node != o.node; }
    };

    iterator begin() noexcept { return iterator(head); }
    iterator end() noexcept { return iterator(nullptr); }

    void clear() noexcept { clear_nodes(); }
};
#endif // SHAPELIST_H