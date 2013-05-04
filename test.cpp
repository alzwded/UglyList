#include <stdio.h>
#include "UglyList.h"
#include <string>
#include <sstream>
#include <queue>
#include <set>

#define FORMAT ". %-16s -%c-\n"

/*
    Ugly implementation of an intrussive list
    author: Vlad Mesco
*/

static int failed = 0;
static int successful = 0;
static std::set<int> expectedToBeDestroyed;
static std::set<int> keepAlive;

class ElementFactory;
class Element {
    static int nextX;
    int x;
    bool talkative;
    Element(const int preferred) : link(this), x(preferred), talkative(false) {}
    friend class ElementFactory;
public:
    int X() const { return x; }
    std::string toString() const {
        std::stringstream s; 
        s << X();
        return s.str();
    }
    UglyList::ListNode<Element> link;
    Element() : link(this) , x(++nextX) , talkative(true) { printf(": hello from %d\n", x); }
    ~Element() {
        if(talkative) {
            printf(": bye bye from %d\n", x);
            std::set<int>::iterator f = expectedToBeDestroyed.find(x);
            if(f != expectedToBeDestroyed.end()) {
                expectedToBeDestroyed.erase(f);
            }
        }
    }
    bool operator==(const Element& other) {
        return X() == other.X();
    }
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

struct pick_functor {
    void setX(const int x) { _x = x; }
    pick_functor(const int x) : _x(x) {}
    bool operator()(const Element& x) {
        if(x.X() == _x) return true;
        else return false;
    }
private:
    int _x;
};

class ExpectInt {
    std::queue<int> nodes;
public:
    ExpectInt(void) {}
    ExpectInt(const int x) {
        nodes.push(x);
    }
    ExpectInt& operator()(const int x) {
        nodes.push(x);
        return *this;
    }
    bool operator!() {
        return !nodes.empty();
    }
    int operator*() {
        int ret = nodes.front(); // don't care
        nodes.pop();
        return ret;
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
    bool operator!=(const std::string& other) {
        return !(operator==(other));
    }
    bool operator!() {
        if(nodes.empty()) successful++;
        else failed++;
        return nodes.empty();
    }
};

int Element::nextX = 0;

struct foreach_predicate {
    foreach_predicate(Expect expected)
        : _expected(expected)
        , _okay(true)
    {}
    void operator()(const Element& e) {
        if(_expected != e.toString()) _okay = false;
    }
    bool operator~() { return _okay; }
private:
    Expect _expected;
    bool _okay;
};

void intro() {
    printf("  %-16s %-16s\n", "returned value", "matches expected");
    printf("--%-16s-%-16s\n", "----------------", "----------------");
    fflush(stdout);
}

void outro() {
    printf("--%-16s-%-16s\n", "----------------", "----------------");
    printf("%d checks of which %d were good and %d failed\n", successful + failed, successful, failed);
    fflush(stdout);
}

template<class T>
void print(T& value, Expect expected) {
    printf(FORMAT, value.toString().c_str(), (expected == value.toString()) ? 'v' : 'x');
    fflush(stdout);
}

void print(const char* value, bool pass) {
    printf(FORMAT, value, (pass) ? 'v' : 'x');
    if(pass) successful++;
    else failed++;
    fflush(stdout);
}

void expectToBeDestroyed(ExpectInt expected) {
    expectedToBeDestroyed.clear();
    while(!expected) expectedToBeDestroyed.insert(*expected);
}

void dontDestroy(ExpectInt expected) {
    expectedToBeDestroyed.clear();
    keepAlive.clear();
    while(!expected) {
        int i = *expected;
        expectedToBeDestroyed.insert(i);
        keepAlive.insert(i);
    }
}

void print(const char*);
void isEverythingStillAlive() {
    bool fail = false;
    for(std::set<int>::iterator i = keepAlive.begin(); i != keepAlive.end(); ++i) {
        if(!expectedToBeDestroyed.count(*i)) {
            fail = true;
            std::stringstream s;
            s << *i << " is gone";
            print(s.str().c_str(), false);
        } else {
            std::stringstream s;
            s << *i << " still there";
            print(s.str().c_str(), true);
        }
    }
    if(!fail) {
        print("everything that should be alive is still alive");
    }
    fflush(stdout);
}

void wasEverythingDestroyed() {
    if(expectedToBeDestroyed.empty()) {
        print("success", true);
        printf(". all elements that were supposed to be destroyed were destroyed\n");
    } else {
        print("failed", false);
        printf(". element(s) ");
        for(std::set<int>::iterator i = expectedToBeDestroyed.begin(); i != expectedToBeDestroyed.end(); ++i) {
            printf("%d,", *i);
        }
        printf(" was/were not destroyed.\n");
    }
    fflush(stdout);
}

void print(const char* value) {
    printf(". %s\n", value);
    fflush(stdout);
}

template<class T>
void print(UglyList::List<T>& list, Expect expected) {
    printf(". printing list:\n");
    for(typename UglyList::List<T>::iterator i = list.begin(); i != list.end(); ++i) {
        printf(FORMAT, (**i)->toString().c_str(), (expected == (**i)->toString()) ? 'v' : 'x');
    }
    printf(FORMAT, "<end of list>", (!expected ? 'v' : 'x'));
    fflush(stdout);
}

template<class T>
void rprint(UglyList::List<T>& list, Expect expected) {
    printf(". rprinting list:\n");
    for(typename UglyList::List<T>::riterator i = list.rbegin(); i != list.rend(); ++i) {
        printf(FORMAT, (**i)->toString().c_str(), (expected == (**i)->toString()) ? 'v' : 'x');
    }
    printf(FORMAT, "<end of list>", (!expected ? 'v' : 'x'));
    fflush(stdout);
}

void println(const char* s) {
    printf("%s\n", s);
    fflush(stdout);
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
        expectToBeDestroyed(ExpectInt(1));
        list.erase(list.begin());
        wasEverythingDestroyed();
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
        expectToBeDestroyed(ExpectInt(4));
        list.pop_back();
        wasEverythingDestroyed();
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
        expectToBeDestroyed(ExpectInt(2));
        e->remove();
        wasEverythingDestroyed();

        println("add node and remove it with listnode->remove");
        Element* removabe = new Element();
        list.push_front(&removabe->link);
        println(". calling remove");
        expectToBeDestroyed(ExpectInt(6));
        removabe->link.remove();
        wasEverythingDestroyed();
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
        print(otherList, Expect(12));

        println("call clear()");
        expectToBeDestroyed(ExpectInt(12));
        otherList.clear();
        wasEverythingDestroyed();
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

        println("find range");
        UglyList::List<Element>::iterator found4 = list.find(findThis, list.begin() + 1, list.end() - 2);
        if(found4 != list.end()) {
            print(***found4, Expect(11));
        } else {
            print("failed", false);
        }

        println("find_if");
        pick_functor aPickFunctor(11);
        UglyList::List<Element>::iterator found6 = list.find_if(aPickFunctor);
        if(found6 == list.end()) {
            print("failed", false);
        } else {
            print(***found6, Expect(11));
        }

        println("find_if fails");
        aPickFunctor.setX(-1);
        UglyList::List<Element>::iterator found7 = list.find_if(aPickFunctor, found6 + 1);
        if(found7 != list.end()) {
            print("failed", false);
        } else {
            print("success", true);
        }

        println("for_each");
        foreach_predicate frch(Expect(10)(11)(9)(8)(7));
        list.for_each(frch, list.begin() + 1);
        print("for_each", ~frch);

//=DON'T=TRIM=HERE====================================START
        println("remove using destructor");
        expectToBeDestroyed(ExpectInt(5)(10)(11)(9)(8)(7));
    }
    wasEverythingDestroyed();
//=DON'T=TRIM=HERE======================================END

    {
        println("test list transfer");
        UglyList::List<Element> a;
        a.push_back(&(new Element())->link); // 13
        a.push_back(&(new Element())->link); // 14
        dontDestroy(ExpectInt(13)(14));
        UglyList::List<Element> b = a;
        isEverythingStillAlive();
        print(a, Expect(13)(14));
        print(b, Expect(13)(14));
        {
            println("trying not to crash everything");
            UglyList::List<Element> c = a;
        }
        isEverythingStillAlive();
        print("should see elements destructed now");
        expectToBeDestroyed(ExpectInt(13)(14));
    }
    wasEverythingDestroyed();
    print("success", true);

    {
        println("remove_if");
        UglyList::List<Element> a;
        a.push_back(&(new Element())->link); // 15
        a.push_back(&(new Element())->link); // 16
        a.push_back(&(new Element())->link); // 17
        struct holder {
            static bool f(const Element& e) {
                return e.X() == 16;
            }
            static bool g(const Element& e) {
                return e.X() == 15;
            }
        };
        expectToBeDestroyed(ExpectInt(16));
        a.remove_if(&holder::f);
        wasEverythingDestroyed();
        print(a, Expect(15)(17));

        println("extract_if");
        UglyList::List<Element> b = a.extract_if(&holder::g);
        print(a, Expect(17));
        print(b, Expect(15));
    }

    {
        println("iterator difference");
        UglyList::List<Element> a;
        a.push_back(&(new Element())->link); // 18
        a.push_back(&(new Element())->link); // 19
        a.push_back(&(new Element())->link); // 20
        UglyList::List<Element>::iterator i = a.begin() + 1;
        UglyList::List<Element>::iterator j = a.begin() + 2;
        print("difference is okay", (j - i) == 1);
        print("rdifference is okay", (i - j) == -1);

        println("size");
        print("size is okay", a.size() == 3);
    }

    outro();

    return 0;
}
