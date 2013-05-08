UglyList
========

ugly implementation of intrusive lists

TODO
====

* ~~[ ] unit tests~~ I don't wanna rewrite test.cpp
* ~~[ ] import the test framework from older project that takes abort calls and sigsegvs into account because that was cool and I want it here~~
* [x] extract to remove an element from the list without baleeting it
* [x] splice(iterator, otherList, iterator, iterator) method to transfer an element to another list
* [x] merge(list)
* [x] swap(iterator, iterator)
* [x] add iterator.operator-(iterator)
* [x] size()
* [x] add find(), rfind()
* [x] findif(functor)
* [x] find, rfind, findif(range)
* [x] clear()
* ~~[ ] emplace()~~ wait until c++11 becomes a hosehold name
* [x] sort(functor)
* [x] foreach(predicate)
* ~~[ ] foreach_if(predicate)~~ overruled
* [x] remove_if (new idea!)
* [x] extract_if -- return a new list of stuff that matches a functor (same new idea!)
* [x] make list passable by value

Lessons Learnt
==============

Lesson 1
--------

```C++
struct T { ... }
void main() {
    UglyList::List<T> list;
    struct functor {
        bool operator(const T&) { return false; }
    } f;
    UglyList::List<T>::iterator r = list.find_if(f); // will never compile. ever.
}
```

```C++
struct T { ... }
struct functor {
    bool operator(const T&) { return false; }
};
void main() {
    UglyList::List<T> list;
    UglyList::List<T>::iterator r = list.find_if(f); // will compile successfully and it will work.
}
```
