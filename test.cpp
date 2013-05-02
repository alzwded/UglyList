#include <stdio.h>
#include "UglyList.h"
#include <string>
#include <sstream>
#include <queue>

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

class Expect {
    std::queue<std::string> nodes;
public:
    Expect(void) {}
    Expect(const char* node)
    {
        nodes.push(std::string(node));
    }
    Expect(const int node)
    {
        std::stringstream s;
        s << node;
        nodes.push(s.str());
    }
    Expect& operator()(const char* node) {
        nodes.push(std::string(node));
        return *this;
    }
    Expect& operator()(const int node) {
        std::stringstream s;
        s << node;
        nodes.push(s.str());
        return *this;
    }
    bool operator==(const std::string& other) {
        if(nodes.empty()) return false;
        bool ret = (nodes.front() == other);
        nodes.pop();
        return ret;
    }
    bool operator!() {
        return nodes.empty();
    }
};

int Element::nextX = 0;

template<class T>
void print(UglyList::List<T>& list, Expect expected) {
    for(typename UglyList::List<T>::iterator i = list.begin(); i != list.end(); ++i) {
        printf(". %-16s %c\n", (**i)->toString().c_str(), (expected == (**i)->toString()) ? 'v' : 'x');
    }
    printf(". %-16s %c\n", "<end of list>", (!expected ? 'v' : 'x'));
}

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
    print(list, Expect(1)(2)(3));

    printf("print in reverse\n");
    for(UglyList::List<Element>::riterator i = list.rbegin(); i != list.rend(); ++i) {
        printf(". %d\n", (**i)->X());
    }

    printf("remove head using erase(begin())\n");
    list.erase(list.begin());
    print(list, Expect(2)(3));

    printf("add new elements\n");
    list.push_back(&(new Element())->link);
    print(list, Expect(2)(3)(4));

    printf("print elements using index operator\n");
    printf(". %d\n", (*list[1])->X());
    printf(". %d\n", (*list[2])->X());

    printf("remove element using remove\n");
    list.remove(list[1]);
    print(list, Expect(2)(4));

    printf("remove tail using pop_back()\n");
    list.pop_back();
    print(list, Expect(2));

    printf("push head using push_front()\n");
    list.push_front(&(new Element())->link);
    print(list, Expect(5)(2));

    printf("print using back()\n");
    printf(". %d\n", list.back().X());

    printf("extract an element\n");
    UglyList::ListNode<Element>* e = list.extract(list.rbegin());
    printf(". extracted: %d\n", (*e)->X());
    e->remove();
    print(list, Expect(5));

    printf("add node and remove it with listnode->remove\n");
    Element* removabe = new Element();
    list.push_front(&removabe->link);
    printf(". calling remove\n");
    removabe->link.remove();
    print(list, Expect(5));

    printf("Add 3 more elements\n");
    list.push_back(&(new Element())->link);
    list.push_back(&(new Element())->link);
    list.push_back(&(new Element())->link);
    print(list, Expect(5)(7)(8)(9));

    printf("swap elements 1 and 3\n");
    UglyList::List<Element>::iterator i1 = list.begin() + 1;
    UglyList::List<Element>::iterator i3 = list.begin() + 3;
    list.swap(i1, i3);
    print(list, Expect(5)(9)(8)(7));

    printf("iterate in reverse\n");
    for(UglyList::List<Element>::riterator i = list.rbegin(); i != list.rend(); ++i) {
        printf(". %d\n", (**i)->X());
    }

    printf("splice in a list of 3 new elements\n");
    UglyList::List<Element> otherList;
    otherList.push_back(&(new Element())->link);
    otherList.push_back(&(new Element())->link);
    otherList.push_back(&(new Element())->link);
    list.splice(list.begin(), otherList, otherList.begin(), otherList.end() - 1);
    print(list, Expect(5)(10)(11)(9)(8)(7));

    printf("call clear()\n");
    otherList.clear();
    print(otherList, Expect());

    printf("remove using destructor\n");

    return 0;
}
