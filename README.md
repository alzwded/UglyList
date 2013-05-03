UglyList
========

ugly implementation of intrusive lists

TODO
====

* [ ] unit tests
* [ ] import the test framework from older project that takes abort calls and sigsegvs into account because that was cool and I want it here
* [x] extract to remove an element from the list without baleeting it
* [x] splice(iterator, otherList, iterator, iterator) method to transfer an element to another list
* [x] swap(iterator, iterator)
* [ ] add iterator.operator-(iterator)
* [x] add find(), rfind()
* [x] findif(functor)
* [/] find, rfind, findif(range)
* [x] clear()
* [ ] emplace()
* [ ] sort(functor)
* [ ] foreach(predicate)
* [ ] foreach_if(predicate)
* [ ] remove_if
* [ ] extract_if -- return a new list of stuff that matches a functor
* [x] make list passable by value
