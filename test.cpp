/*
Copyright (c) 2013, Vlad Mesco
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

*   Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include "UglyList.h"
#include <string>
#include <sstream>
#include <queue>
#include <set>

#define FORMAT ". %-16s -%c-\n"

#ifndef FAIL_CODE
# define FAIL_CODE 1
#endif

/*
    Ugly implementation of an intrussive list
    This is the test/documentation executable source file that comes with
    UglyList.h
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
    bool operator<(const Element& other) const {
        return X() < other.X();
    }
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

struct reverse_compare {
    bool operator()(const Element& a, const Element& b) {
        return b.X() < a.X();
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
    printf("%d checks of which %d were good (-v-) and %d failed (-x-)\n", successful + failed, successful, failed);
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
        print(**list[0], Expect(2));

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
        list.splice(list.begin() + 1, otherList, otherList.begin(), otherList.end() - 1);
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

        println("merge");
        UglyList::List<Element> b;
        b.push_back(&(new Element())->link); // 21
        b.push_back(&(new Element())->link); // 22
        b.push_back(&(new Element())->link); // 23
        dontDestroy(ExpectInt(18)(19)(20)(21)(22)(23));
        a.merge(b);
        isEverythingStillAlive();
        print(a, Expect(18)(19)(20)(21)(22)(23));
        print(b, Expect());

        println("swap(2)");
        a.swap(a.begin(), a.end() - 1);
        a.swap(a.begin(), a.begin() + 3);
        print(a, Expect(21)(19)(20)(23)(22)(18));
        rprint(a, Expect(18)(22)(23)(20)(19)(21));
        println("swap(3)");
        a.swap(a.begin() + 1, a.begin() + 2);
        print(a, Expect(21)(20)(19)(23)(22)(18));
        rprint(a, Expect(18)(22)(23)(19)(20)(21));

        println("swap(same)");
        a.swap(a.begin(), a.begin());
        print(a, Expect(21)(20)(19)(23)(22)(18));
        rprint(a, Expect(18)(22)(23)(19)(20)(21));

        println("sort");
        a.sort();
        print(a, Expect(18)(19)(20)(21)(22)(23));
        rprint(a, Expect(23)(22)(21)(20)(19)(18));

        println("sort with functor");
        reverse_compare fctor;
        a.sort(fctor);
        print(a, Expect(23)(22)(21)(20)(19)(18));

        expectToBeDestroyed(ExpectInt(18)(19)(20)(21)(22)(23));
        a.clear();
        wasEverythingDestroyed();
    }

    {
        println("test validity during byval function call & return");
        struct giytxkk {
            static UglyList::List<Element> f(UglyList::List<Element> l) {
                isEverythingStillAlive();
                print(l, Expect(24)(25));
                return l;
            }
        };
        {
            UglyList::List<Element> list;
            list.push_back(&(new Element())->link); // 24
            list.push_back(&(new Element())->link); // 25
            dontDestroy(ExpectInt(24)(25));
            giytxkk::f(list);
            isEverythingStillAlive();
            print(list, Expect(24)(25));
            expectToBeDestroyed(ExpectInt(24)(25));
        }
        wasEverythingDestroyed();
    }

	{
		println("test cache of []");
		UglyList::List<Element> list;
		list.push_back(&(new Element())->link); // 26
		list.push_back(&(new Element())->link); // 27
		list.push_back(&(new Element())->link); // 28
		list.push_back(&(new Element())->link); // 29
		list.push_back(&(new Element())->link); // 30
		print(**list[0], Expect(26));
		print(**list[4], Expect(30));
		print(**list[1], Expect(27));
		print(**list[2], Expect(28));
		print(**list[3], Expect(29));
		print(**list[0], Expect(26));
		print(**list[4], Expect(30));
		print(**list[3], Expect(29));
		print(**list[2], Expect(28));
		print(**list[4], Expect(30));
		println("test cache validation after insert");
		list.push_back(&(new Element())->link); // 31
		print(**list[5], Expect(31));
		println("test after erase");
		list[5]->remove();
		print(**list[4], Expect(30));
	}

    outro();

    if(failed) return FAIL_CODE;
    else return 0;
}
