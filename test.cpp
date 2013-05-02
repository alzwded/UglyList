#include <stdio.h>
#include "UglyList.h"
#include <string>
#include <sstream>

/*
    Ugly implementation of an intrussive list
    author: Vlad Mesco
*/

class Element {
    static int nextX;
    int x;
public:
    int X() { return x; }
    std::string toString() {
        std::stringstream s; 
        s << X();
        return s.str();
    }
    UglyList::ListNode<Element> link;
    Element() : link(this) , x(++nextX) { printf(": hello from %d\n", x); }
    ~Element() { printf(": bye bye from %d\n", x); }
};

int Element::nextX = 0;

template<class T>
void print(UglyList::List<T>& list) {
    for(typename UglyList::List<T>::iterator i = list.begin(); i != list.end(); ++i) {
        printf(". %s\n", (**i)->toString().c_str());
    }
}

int main() {
    UglyList::List<Element> list;
    printf("add 3 elements\n");
    list.push_back(&(new Element())->link);
    list.push_back(&(new Element())->link);
    list.push_back(&(new Element())->link);
    print(list);

    printf("print in reverse\n");
    for(UglyList::List<Element>::riterator i = list.rbegin(); i != list.rend(); ++i) {
        printf(". %d\n", (**i)->X());
    }

    printf("remove head using erase(begin())\n");
    list.erase(list.begin());
    print(list);

    printf("add new elements\n");
    list.push_back(&(new Element())->link);
    print(list);

    printf("print elements using index operator\n");
    printf(". %d\n", (*list[1])->X());
    printf(". %d\n", (*list[2])->X());

    printf("remove element using remove\n");
    list.remove(list[1]);
    print(list);

    printf("remove tail using pop_back()\n");
    list.pop_back();
    print(list);

    printf("push head using push_front()\n");
    list.push_front(&(new Element())->link);
    print(list);

    printf("print using back()\n");
    printf(". %d\n", list.back().X());

    printf("extract an element\n");
    UglyList::ListNode<Element>* e = list.extract(list.rbegin());
    printf(". extracted: %d\n", (*e)->X());
    e->remove();
    print(list);

    printf("add node and remove it with listnode->remove\n");
    Element* removabe = new Element();
    list.push_front(&removabe->link);
    printf(". calling remove\n");
    removabe->link.remove();
    print(list);

    printf("remove using destructor\n");

    return 0;
}
