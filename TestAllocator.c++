// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------------

/*
To test the program:
    % ls /usr/include/cppunit/
    ...
    TestFixture.h
    ...
    % locate libcppunit.a
    /usr/lib/libcppunit.a
    % g++ -pedantic -std=c++0x -Wall Allocator.c++ TestAllocator.c++ -o TestAllocator -lcppunit -ldl
    % valgrind TestAllocator >& TestAllocator.out
*/

// --------
// includes
// --------

#include <algorithm> // count
#include <iostream>  // ios_base
#include <memory>    // allocator

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#include "Allocator.h"

#define private public // 

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // test_one
    // --------

    void test_one () {
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);
    }

    // --------
    // test_ten
    // --------

    void test_ten () {
        A x;
        const difference_type s = 10;
        const value_type      v = 2;
        const pointer         b = x.allocate(s);
              pointer         e = b + s;
              pointer         p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        CPPUNIT_ASSERT(std::count(b, e, v) == s);
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);
    }
        
    // --------
    // test_bad
    // --------
    
    void test_bad () {
        A x;
        const difference_type s = 10;
        const pointer         b = x.allocate(s);
        try {
            x.allocate(s);
        	CPPUNIT_ASSERT(false);
       	}
        catch (...) {
            x.deallocate(b, s);
        }
        
    }
	
	

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_bad);
    //CPPUNIT_TEST(test_zro);
    CPPUNIT_TEST_SUITE_END();
};

// ---------------
// TestMyAllocator
// ---------------

template <typename A>
struct TestMyAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

	// --------
	// test_vd1
	// --------
	
	void test_vd1 () {
		A x;
		CPPUNIT_ASSERT(x.valid());
	}
	
	void test_vd2 () {
		A x;
		x.allocate(5);
		CPPUNIT_ASSERT(x.valid());
	}
	
	void test_vd3 () {
		A x;
		try {
			x.allocate(40);
			x.allocate(30); 
		} catch (...) { }
		
		
		CPPUNIT_ASSERT(x.valid());
	}
	
	void test_vw1 () {
		A x;
		CPPUNIT_ASSERT(x.view(*(x.a)) == 92);
	}
	
	void test_vw2 () {
		A x;
		x.allocate(1);
		CPPUNIT_ASSERT(x.view(*(x.a)) == -(int) sizeof(value_type));
	
	}
	
	void test_vw3 () {
		A x;
		x.allocate(5);
		CPPUNIT_ASSERT(x.view(*(x.a)) == -5*(int) sizeof(value_type));
	}
	
	void test_ct1 () {
		A x;
		CPPUNIT_ASSERT(x.view(*(x.a)) == 92);
		CPPUNIT_ASSERT(x.view(*(x.a+96)) == 92);
	}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestMyAllocator);
	CPPUNIT_TEST(test_vd1);
	CPPUNIT_TEST(test_vd2);
	CPPUNIT_TEST(test_vd3);
	CPPUNIT_TEST(test_vw1);
	CPPUNIT_TEST(test_vw2);
	CPPUNIT_TEST(test_vw3);
	CPPUNIT_TEST(test_ct1);
    CPPUNIT_TEST_SUITE_END();
};

// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;
    CppUnit::TextTestRunner tr;

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); 

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); 
	
	//tests for my allocator
	tr.addTest(TestMyAllocator< Allocator<int, 100> >::suite()); 
	tr.addTest(TestMyAllocator< Allocator<double, 100> >::suite());
	
	//tr.addTest(TestMyAllocator< Allocator<double, 4> >::suite());
	
    tr.run();

    cout << "Done." << endl;
    return 0;}
