#ifndef UGLYLIST_H
#define UGLYLIST_H

/*
    Ugly implementation of an intrussive list
    author: Vlad Mesco
*/

namespace UglyList {

template<class T> class List;

template<class T>
struct ListNode {
private:
    T* _ptr;
    struct ListNode* next;
    struct ListNode* prev;
    friend class List<T>;

public:
    T* ptr() { return _ptr; }
    T* operator->() { return _ptr; }
    T& operator*() { return *_ptr; }
    ListNode(T* that)
        : _ptr(that)
    {}

    void remove() {
        if(this->next) this->next->prev = this->prev;
        if(this->prev) this->prev->next = this->next;
        this->next = NULL;
        this->prev = NULL;
        delete _ptr;
    }
};

template<class T>
struct List {
public:
    struct iterator {
        iterator(ListNode<T>* v, bool reverse = false)
            : Value(v)
            , Reverse(reverse)
        {}

        ListNode<T>* operator*() {
            return Value;
        }

        ListNode<T>* operator->() {
            return Value;
        }

        iterator operator++() {
            iterator ret(Value);
            if(Reverse) {
                Value = Value->prev;
            } else {
                Value = Value->next;
            }
            return ret;
        }

        iterator operator++(int) {
            if(Reverse) {
                Value = Value->prev;
            } else {
                Value = Value->next;
            }
            return iterator(Value);
        }

        iterator operator--() {
            iterator ret(Value);
            if(Reverse) {
                Value = Value->next;
            } else {
                Value = Value->prev;
            }
            return ret;
        }

        iterator operator--(int) {
            if(Reverse) {
                Value = Value->next;
            } else {
                Value = Value->prev;
            }
            return iterator(Value, Reverse);
        }

        iterator operator+(int i) {
            if(i < 0) {
                return operator-(-i);
            } else {
                iterator ret(Value, Reverse);
                while(i-- > 0) {
                    ret++;
                    if(Value->ptr() == NULL) break;
                }
                return ret;
            }
        }

        iterator operator-(int i) {
            if(i < 0) {
                return operator+(-i);
            } else {
                iterator ret(Value, Reverse);
                while(i-- > 0) {
                    ret--;
                    if((*ret)->ptr() == NULL) break;
                }
                return ret;
            }
        }

        bool operator!=(const iterator& other) {
            return Value != other.Value;
        }
        
        bool operator==(const iterator& other) {
            return Value == other.Value;
        }
    private:
        ListNode<T>* Value;
        bool Reverse;
    };

    typedef struct iterator riterator;

public:

    List()
        : root(new ListNode<T>(NULL))
        , cache(NULL)
        , cacheI(0)
    {
        root->next = root;
        root->prev = root;
    }

    ~List()
    {
        clear();
        delete root;
    }

    template<typename pickFunc>
    List<T> extract_if(pickFunc f) {
        List<T> ret;
        for(iterator i = begin(); i != end(); ++i) {
            if(f(***i)) {
                ret.insert(extract(i), ret.root->prev, ret.root);
            }
        }
    }

    template<typename pickFunc>
    void remove_if(pickFunc f) {
        for(iterator i = begin(); i != end(); ++i) {
            if(f(***i)) {
                erase(i);
            }
        }
    }

    template<typename predicate>
    void for_each(predicate f, iterator first, iterator last) {
        for(iterator i = first; i != last; ++i) {
            f(***i);
        }
    }

    template<typename predicate>
    void for_each(predicate f, iterator first) {
        for_each(f, first, end());
    }

    template<typename predicate>
    void for_each(predicate f) {
        for_each(f, begin(), end());
    }

    template<typename pickFunc>
    iterator find_if(pickFunc func, iterator first, iterator last) {
        for(iterator i = first; i != last; ++i) {
            if(func(***i)) return i;
        }
        return end();
    }

    template<typename pickFunc>
    iterator find_if(pickFunc func, iterator first) {
        return find_if(func, first, end());
    }

    template<typename pickFunc>
    iterator find_if(pickFunc func) {
        return find_if(func, begin(), end());
    }

    template<class pickFunc>
    riterator rfind_if(pickFunc func, riterator first, riterator last) {
        for(riterator i = first; i != last; ++i) {
            if(func(***i)) return i;
        }
        return rend();
    }

    template<class pickFunc>
    riterator rfind_if(pickFunc func, riterator first) {
        return rfind_if(func, first, rend());
    }

    template<class pickFunc>
    riterator rfind_if(pickFunc func) {
        return rfind_if(func, rbegin(), rend());
    }

    riterator rfind(T& what, riterator first, riterator last) {
        for(riterator i = first; i != last; ++i) {
            if(***i == what) {
                return i;
            }
        }
        return rend();
    }

    riterator rfind(T& what, riterator first) {
        return rfind(what, first, rend());
    }

    riterator rfind(T& what) {
        return rfind(what, rbegin(), rend());
    }

    template<class compFunc>
    riterator rfind(T& what, compFunc func, riterator first, riterator last) {
        for(riterator i = first; i != last; ++i) {
            if(func(***i, what)) {
                return i;
            }
        }
        return rend();
    }

    template<class compFunc>
    riterator rfind(T& what, compFunc func, riterator first) {
        return rfind(what, func, first, rend());
    }

    template<class compFunc>
    riterator rfind(T& what, compFunc func) {
        return rfind(what, func, rbegin(), rend());
    }

    iterator find(T& what, iterator first, iterator last) {
        for(iterator i = first; i != last; ++i) {
            if(***i == what) {
                return i;
            }
        }
        return end();
    }

    iterator find(T& what, iterator first) {
        return find(what, first, end());
    }

    iterator find(T& what) {
        return find(what, begin(), end());
    }

    template<class compFunc>
    iterator find(T& what, compFunc func, iterator first, iterator last) {
        for(iterator i = first; i != last; ++i) {
            if(func(***i, what)) {
                return i;
            }
        }
        return end();
    }

    template<class compFunc>
    iterator find(T& what, compFunc func, iterator first) {
        return find(what, func, first, end());
    }

    template<class compFunc>
    iterator find(T& what, compFunc func) {
        return find(what, func, begin(), end());
    }

    void splice(iterator pos, List<T>& other, iterator first, iterator last) {
        for(iterator p = first; p != last;) {
            iterator prev = p;
            ++p;
            insert(other.extract(prev), *pos, *(pos + 1));
            ++pos;
        }
    }

    void splice(iterator pos, List<T>& other, iterator first) {
        splice(pos, other, first, other.end());
    }

    void splice(iterator pos, List<T>& other) {
        splice(pos, other, other.begin(), other.end());
    }

    void clear() {
        while(!empty()) {
            remove(root->next);
        }
    }

    void insert(ListNode<T>* node, ListNode<T>* prev, ListNode<T>* next) {
        node->next = next;
        node->prev = prev;
        next->prev = node;
        prev->next = node;
    }

    void push_front(ListNode<T>* node) {
        insert(node, root, root->next);
    }

    void push_back(ListNode<T>* node) {
        insert(node, root->prev, root);
    }

    void pop_front() {
        erase(begin());
    }

    void pop_back() {
        erase(rbegin());
    }

    T& front() {
        return ***begin();
    }

    T& back() {
        return ***rbegin();
    }

    ListNode<T>* operator[](int i) {
        if(cache == end()) {
            cache = NULL;
        }
        if(cache == NULL) {
            cache = begin();
            cacheI = 0;
        }
        cache = cache + (i - cacheI);
        cacheI = i;
        return *cache;
    }

    bool empty() {
        return root->next == root;
    }

    void swap(iterator first, iterator second) {
        ListNode<T>* a = *first;
        ListNode<T>* b = *second;
        ListNode<T>* tempNext = a->next;
        ListNode<T>* tempPrev = a->prev;
        a->next = b->next;
        a->prev = b->prev;
        tempNext->prev = b;
        tempPrev->next = b;
        b->next->prev = a;
        b->prev->next = a;
        b->next = tempNext;
        b->prev = tempPrev;
    }

    ListNode<T>* extract(iterator i) {
        ListNode<T>* ret = *i;
        ret->next->prev = ret->prev;
        ret->prev->next = ret->next;
        ret->next = NULL;
        ret->prev = NULL;
        return ret;
    }

    void remove(ListNode<T>* node) {
        node->remove();
    }

    iterator begin() {
        return iterator(root->next);
    }

    iterator end() {
        return iterator(root);
    }

    iterator rbegin() {
        return iterator(root->prev, true);
    }

    iterator rend() {
        return iterator(root, true);
    }

    void erase(iterator i) {
        remove((*i));
    }

private:
    ListNode<T>* root;
    iterator cache;
    int cacheI;

};

} // template UglyList

#endif
