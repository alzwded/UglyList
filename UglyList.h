/*
Copyright (c) 2013, Vlad Mesco
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

*   Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef UGLYLIST_H
#define UGLYLIST_H

/*
    Ugly implementation of an intrussive list
    This implementation is a follower of the GIGO school of thought
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
class List {
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

        int operator-(iterator other) {
            iterator i = *this;
            size_t length = 0;
            do {
                if(i == other) return length;
                --i;
                ++length;
            } while((*i)->ptr() != NULL);

            length = 0;
            iterator j = other;
            do {
                if(operator==(j)) return length;
                --j;
                --length;
            } while((*j)->ptr() != NULL);
            
            return 0;
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
        , rc(new int(1))
    {
        root->next = root;
        root->prev = root;
    }

    List(const List<T>& other)
        : root(other.root)
        , cache(NULL)
        , cacheI(0)
        , rc(other.rc)
    {
        ++*rc;
    }

private:
    void deref() {
        if(--*rc == 0) {
            clear();
            delete root;
        }
    }

public:
    ~List()
    {
        deref();
    }

    size_t size() {
        return end() - begin();
    }

    List<T>& operator=(const List<T>& other) {
        deref();
        root = other.root;
        rc = other.rc;
        ++*rc;
        cacheI = 0;
        cache.Value = NULL;
    }

    template<typename pickFunc>
    void remove_if(pickFunc f) {
        iterator first = begin();
        iterator last = end();
        while(first != last) {
            iterator next = first;
            next++;
            if(f(***first)) {
                erase(first);
            }
            first = next;
        }
    }

    template<typename pickFunc>
    List<T> extract_if(pickFunc f) {
        List<T> ret;
        iterator first = begin();
        iterator last = end();
        while(first != last) {
            iterator next = first;
            next++;
            if(f(***first)) {
                ret.push_back(extract(first));
            }
            first = next;
        }
        return ret;
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

    void merge(List<T>& other) {
        splice(end(), other, other.begin(), other.end());
    }

    void splice(iterator pos, List<T>& other, iterator first, iterator last) {
        clearCache();
        for(iterator p = first; p != last;) {
            iterator prev = p;
            ++p;
            insert(other.extract(prev), *(pos - 1), *pos);
        }
    }

    void splice(iterator pos, List<T>& other, iterator first) {
        splice(pos, other, first, other.end());
    }

    void splice(iterator pos, List<T>& other) {
        splice(pos, other, other.begin(), other.end());
    }

    void clear() {
        clearCache();
        while(!empty()) {
            remove(root->next);
        }
    }

    void insert(ListNode<T>* node, ListNode<T>* prev, ListNode<T>* next) {
        clearCache();
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
        if(first == second) return;
        clearCache();
        ListNode<T>* a = *first;
        ListNode<T>* b = *second;
        ListNode<T>* bNext = b->next;
        ListNode<T>* extracted = extract(second);
        insert(extracted, a, a->next);
        extracted = extract(first);
        insert(extracted, bNext->prev, bNext);
    }

private:
    static bool default_sort(const T& a, const T& b) {
        return a < b;
    }

public:
    void sort() {
        sort(&UglyList::List<T>::default_sort);
    }

    template<typename sortFunc>
    void sort(sortFunc f) {
        // comb sort
        size_t mysize = size();
        int gap = mysize;
        float shrink = 1.3f;
        bool swapped = true;

        while(gap > 1 || swapped) {
            gap = gap / shrink;
            if(gap < 1) gap = 1;

            swapped = false;
            for(int i = 0; i + gap < mysize; ++i) {
                const T& firstval = **operator[](i);
                iterator first = cache;
                const T& secondval = **operator[](i + gap);
                iterator second = cache;
                if(f(secondval, firstval)) {
                    swap(first, second);
                    swapped = true;
                }
            }
        }
    }

    ListNode<T>* extract(iterator i) {
        clearCache();
        ListNode<T>* ret = *i;
        ret->next->prev = ret->prev;
        ret->prev->next = ret->next;
        ret->next = NULL;
        ret->prev = NULL;
        return ret;
    }

    void remove(ListNode<T>* node) {
        clearCache();
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

    void clearCache() {
        cache = NULL;
    }

private:
    ListNode<T>* root;
    iterator cache;
    int cacheI;
    int* rc;
};

} // template UglyList

#endif
