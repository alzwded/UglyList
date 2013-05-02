#include <stdio.h>
#include "UglyList.h"
#include <string>
#include <sstream>
#include <queue>

/*
    Ugly implementation of an intrussive list
    author: Vlad Mesco
*/

class ElementFactory;
class Element {
    static int nextX;
    int x;
    Element(const int preferred) : link(this), x(preferred) {}
    friend ElementFactory;
public:
    int X() const { return x; }
    std::string toString() {
        std::stringstream s; 
        s << X();
        return s.str();
    }
    UglyList::ListNode<Element> link;
    Element() : link(this) , x(++nextX) { printf(": hello from %d\n", x); }
    ~Element() { printf(": bye bye from %d\n", x); }
};

class ElementFactory {
    ElementFactory() {}
public:
    static Element NewElement(const int preferred) {
        return Element(preferred);
    }
    static bool compareElement(const Element& a, const Element& b) {
        return a.X() == b.X();
    }
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

void intro() {
    printf("  %-16s %-16s\n", "returned value", "matches expected");
    printf("--%-16s-%-16s\n", "----------------", "----------------");
}

template<class T>
void print(T& value, Expect expected) {
    printf(". %-16s %c\n", value.toString().c_str(), (expected == value.toString()) ? 'v' : 'x');
}

void print(const char* value, bool pass) {
    printf(". %-16s %c\n", value, (pass) ? 'v' : 'x');
}

void print(const char* value) {
    printf(". %-16s %c\n", value, ' ');
}

template<class T>
void print(UglyList::List<T>& list, Expect expected) {
    for(typename UglyList::List<T>::iterator i = list.begin(); i != list.end(); ++i) {
        printf(". %-16s %c\n", (**i)->toString().c_str(), (expected == (**i)->toString()) ? 'v' : 'x');
    }
    printf(". %-16s %c\n", "<end of list>", (!expected ? 'v' : 'x'));
}

template<class T>
void rprint(UglyList::List<T>& list, Expect expected) {
    for(typename UglyList::List<T>::riterator i = list.rbegin(); i != list.rend(); ++i) {
        printf(". %-16s %c\n", (**i)->toString().c_str(), (expected == (**i)->toString()) ? 'v' : 'x');
    }
    printf(". %-16s %c\n", "<end of list>", (!expected ? 'v' : 'x'));
}

void println(const char* s) {
    printf("%s\n", s);
}

int main() {
    intro();

    UglyList::List<Element> list;
    printf("add 3 elements\n");
    list.push_back(&(new Element())->link);
    list.push_back(&(new Element())->link);
    list.push_back(&(new Element())->link);
    print(list, Expect(1)(2)(3));

    printf("print in reverse\n");
    rprint(list, Expect(3)(2)(1));

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
    print(list.back(), Expect(2));

    printf("extract an element\n");
    UglyList::ListNode<Element>* e = list.extract(list.rbegin());
    print(**e, Expect(2));
    print("remaining list");
    print(list, Expect(5));

    printf("call remove on node that's not in a list anymore\n");
    e->remove();
    print("success", true);

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
    rprint(list, Expect(7)(8)(9)(5));

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
        
    printf("find with functor\n");
    Element findThis(ElementFactory::NewElement(11));
    UglyList::List<Element>::iterator found1 = list.find(findThis, &ElementFactory::compareElement);
    if(found1 == list.end()) {
        print("failed", false);
    } else {
        print(***found1, Expect(11));
    }

    println("remove using destructor");

    return 0;
}
