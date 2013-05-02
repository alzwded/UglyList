UglyList
========

ugly implementation of intrusive lists

TODO
====

* [ ] unit tests
* [x] extract to remove an element from the list without baleeting it
* [x] splice(iterator, otherList, iterator, iterator) method to transfer an element to another list
* [x] swap(iterator, iterator)
* [ ] add iterator.operator-(iterator)
* [x] add find(), rfind()
* [ ] findif(functor)
* [/] find, rfind, findif(range)
* [x] clear()
* [ ] emplace()
* [ ] sort(functor)
* [ ] remove_if
* [ ] extract_if -- return a new list of stuff that matches a functor
* [x] make list passable by value
