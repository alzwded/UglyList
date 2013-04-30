#include <stdio.h>
#include "UglyList.h"

/*
    Ugly implementation of an intrussive list
    author: Vlad Mesco
*/

class Element {
    static int nextX;
    int x;
public:
    int X() { return x; }
    UglyList::ListNode<Element> link;
    Element() : link(this) , x(++nextX) { printf(": hello from %d\n", x); }
    ~Element()
    {
        printf(": bye bye from %d\n", x);
    }
};

int Element::nextX = 0;

int main() {
    UglyList::List<Element> list;
    printf("add 3 elements\n");
    list.push_back(&(new Element())->link);
    list.push_back(&(new Element())->link);
    list.push_back(&(new Element())->link);

    for(UglyList::List<Element>::iterator i = list.begin(); i != list.end(); ++i) {
        printf(". %d\n", (**i)->X());
    }

    printf("print in reverse\n");

    for(UglyList::List<Element>::riterator i = list.rbegin(); i != list.rend(); ++i) {
        printf(". %d\n", (**i)->X());
    }

    printf("remove head using erase(begin())\n");

    list.erase(list.begin());

    for(UglyList::List<Element>::iterator i = list.begin(); i != list.end(); ++i) {
        printf(". %d\n", (**i)->X());
    }

    printf("add new elements\n");

    list.push_back(&(new Element())->link);

    for(UglyList::List<Element>::iterator i = list.begin(); i != list.end(); ++i) {
        printf(": %d\n", (**i)->X());
    }

    printf("print elements using index operator\n");

    printf(". %d\n", (*list[1])->X());
    printf(". %d\n", (*list[2])->X());

    printf("remove element using remove\n");
    list.remove(list[1]);

    for(UglyList::List<Element>::iterator i = list.begin(); i != list.end(); ++i) {
        printf(". %d\n", (**i)->X());
    }

    printf("remove tail using pop_back()\n");
    list.pop_back();
    for(UglyList::List<Element>::iterator i = list.begin(); i != list.end(); ++i) {
        printf(". %d\n", (**i)->X());
    }

    printf("push head using push_front()\n");
    list.push_front(&(new Element())->link);
    for(UglyList::List<Element>::iterator i = list.begin(); i != list.end(); ++i) {
        printf(". %d\n", (**i)->X());
    }

    printf("print using back()\n");
    printf(". %d\n", list.back().X());

    printf("remove using destructor\n");

    return 0;
}
