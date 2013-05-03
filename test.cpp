#include <stdio.h>
#include "UglyList.h"
#include <string>
#include <sstream>
#include <queue>

#define FORMAT ". %-16s -%c-\n"

/*
    Ugly implementation of an intrussive list
    author: Vlad Mesco
*/

static int failed = 0;
static int successful = 0;

class ElementFactory;
class Element {
    static int nextX;
    int x;
    bool talkative;
    Element(const int preferred) : link(this), x(preferred), talkative(false) {}
    friend class ElementFactory;
public:
    int X() const { return x; }
    std::string toString() {
        std::stringstream s; 
        s << X();
        return s.str();
    }
    UglyList::ListNode<Element> link;
    Element() : link(this) , x(++nextX) , talkative(true) { printf(": hello from %d\n", x); }
    ~Element() { if(talkative) printf(": bye bye from %d\n", x); }
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
        if(ret) successful++;
        else failed++;
        return ret;
    }
    bool operator!() {
        if(nodes.empty()) successful++;
        else failed++;
        return nodes.empty();
    }
};

int Element::nextX = 0;

void intro() {
    printf("  %-16s %-16s\n", "returned value", "matches expected");
    printf("--%-16s-%-16s\n", "----------------", "----------------");
}

void outro() {
    printf("--%-16s-%-16s\n", "----------------", "----------------");
    printf("%d comparissons of which %d were good and %d failed\n", successful + failed, successful, failed);
}

template<class T>
void print(T& value, Expect expected) {
    printf(FORMAT, value.toString().c_str(), (expected == value.toString()) ? 'v' : 'x');
}

void print(const char* value, bool pass) {
    printf(FORMAT, value, (pass) ? 'v' : 'x');
    if(pass) successful++;
    else failed++;
}

void print(const char* value) {
    printf(FORMAT, value, ' ');
}

template<class T>
void print(UglyList::List<T>& list, Expect expected) {
    for(typename UglyList::List<T>::iterator i = list.begin(); i != list.end(); ++i) {
        printf(FORMAT, (**i)->toString().c_str(), (expected == (**i)->toString()) ? 'v' : 'x');
    }
    printf(FORMAT, "<end of list>", (!expected ? 'v' : 'x'));
}

template<class T>
void rprint(UglyList::List<T>& list, Expect expected) {
    for(typename UglyList::List<T>::riterator i = list.rbegin(); i != list.rend(); ++i) {
        printf(FORMAT, (**i)->toString().c_str(), (expected == (**i)->toString()) ? 'v' : 'x');
    }
    printf(FORMAT, "<end of list>", (!expected ? 'v' : 'x'));
}

void println(const char* s) {
    printf("%s\n", s);
}

int main() {
    intro();

    {
        UglyList::List<Element> list;
        println("add 3 elements");
        list.push_back(&(new Element())->link);
        list.push_back(&(new Element())->link);
        list.push_back(&(new Element())->link);
        print(list, Expect(1)(2)(3));

        println("print in reverse");
        rprint(list, Expect(3)(2)(1));

        println("remove head using erase(begin())");
        list.erase(list.begin());
        print(list, Expect(2)(3));

        println("add new elements");
        list.push_back(&(new Element())->link);
        print(list, Expect(2)(3)(4));

        println("print elements using index operator");
        print(**list[1], Expect(3));
        print(**list[2], Expect(4));

        println("remove element using remove");
        list.remove(list[1]);
        print(list, Expect(2)(4));

        println("remove tail using pop_back()");
        list.pop_back();
        print(list, Expect(2));

        println("push head using push_front()");
        list.push_front(&(new Element())->link);
        print(list, Expect(5)(2));

        println("print using back()");
        print(list.back(), Expect(2));

        println("extract an element");
        UglyList::ListNode<Element>* e = list.extract(list.rbegin());
        print(**e, Expect(2));
        print("remaining list");
        print(list, Expect(5));

        println("call remove on node that's not in a list anymore");
        e->remove();
        print("success", true);

        println("add node and remove it with listnode->remove");
        Element* removabe = new Element();
        list.push_front(&removabe->link);
        println(". calling remove");
        removabe->link.remove();
        print(list, Expect(5));

        println("Add 3 more elements");
        list.push_back(&(new Element())->link);
        list.push_back(&(new Element())->link);
        list.push_back(&(new Element())->link);
        print(list, Expect(5)(7)(8)(9));

        println("swap elements 1 and 3");
        UglyList::List<Element>::iterator i1 = list.begin() + 1;
        UglyList::List<Element>::iterator i3 = list.begin() + 3;
        list.swap(i1, i3);
        print(list, Expect(5)(9)(8)(7));

        println("iterate in reverse");
        rprint(list, Expect(7)(8)(9)(5));

        println("splice in a list of 3 new elements");
        UglyList::List<Element> otherList;
        otherList.push_back(&(new Element())->link);
        otherList.push_back(&(new Element())->link);
        otherList.push_back(&(new Element())->link);
        list.splice(list.begin(), otherList, otherList.begin(), otherList.end() - 1);
        print(list, Expect(5)(10)(11)(9)(8)(7));

        println("call clear()");
        otherList.clear();
        print(otherList, Expect());

        println("find with functor");
        Element findThis(ElementFactory::NewElement(11));
        UglyList::List<Element>::iterator found1 = list.find(findThis, &ElementFactory::compareElement);
        if(found1 == list.end()) {
            print("failed", false);
        } else {
            print(***found1, Expect(11));
        }

        println("rfind with functor");
        UglyList::List<Element>::riterator found2 = list.rfind(findThis, &ElementFactory::compareElement);
        if(found2 == list.rend()) {
            print("failed", false);
        } else {
            print(***found2, Expect(11));
        }

        println("fail finding");
        Element unfindable(ElementFactory::NewElement(0));
        UglyList::List<Element>::riterator found3 = list.rfind(unfindable, &ElementFactory::compareElement);
        if(found3 != list.rend()) {
            print("failed", false);
        } else {
            print("success", true);
        }

        println("remove using destructor");
        print("see output", true);
    }

    outro();

    return 0;
}
