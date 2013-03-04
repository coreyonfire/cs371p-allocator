// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2013
// Corey M. Besmer
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <new>       // new
#include <stdexcept> // invalid_argument
#include <iostream>

#define private public

using namespace std;

// ---------
// Allocator
// ---------

template <typename T, int N>
class Allocator {
public:
    // --------
    // typedefs
    // --------

    typedef T                 value_type;

    typedef int               size_type;
    typedef std::ptrdiff_t    difference_type;

    typedef value_type*       pointer;
    typedef const value_type* const_pointer;

    typedef value_type&       reference;
    typedef const value_type& const_reference;

public:
    // -----------
    // operator ==
    // -----------

    friend bool operator == (const Allocator&, const Allocator&) {
        return true;
    }

    // -----------
    // operator !=
    // -----------

    friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
        return !(lhs == rhs);
    }

private:
    // ----
    // data
    // ----

    char a[N];
	size_type length;

    // -----
    // valid
    // -----

    /**
     * O(1) in space
     * O(n) in time
     * Goes through allocated memory ensuring that all 
	 * spaces have been set up with the correct sentinels.
     */
    bool valid () const {
		//cout << "Validating..." << endl;
		//for (int i = 0; i < N; i+=4) {
		//	cout << view(*(const_cast<char*>(a)+i)) << endl;
		//}
	
        size_type cur = 0, pos = 0;
		while (pos+cur+8 <= N) {
			cur = view(*((const_cast<char*>(a)+pos)));
			if (cur != view(*((const_cast<char*>(a))+pos+abs(cur)+4))) return false;
			if (cur < 0) cur *= -1;
			pos += (cur + 8);
		}
		//cout << "-------" << endl;
        return true;
    }

public:
    // ------------
    // constructors
    // ------------

    /**
     * O(1) in space
     * O(1) in time
     * General constructor for Allocator. Sets up sentinels and whatnot.
     */
    Allocator () {
    	//ensure i have enough space
    	assert (N > sizeof(int)*2 + sizeof(T));
		length = N;
        view(*a) = N - 8;
		view(*(a + N - 4)) = N - 8;
        assert(valid());
    }

    // Default copy, destructor, and copy assignment
    // Allocator  (const Allocator<T>&);
    // ~Allocator ();
    // Allocator& operator = (const Allocator&);

    // --------
    // allocate
    // --------

    /**
     * O(1) in space
     * O(n) in time
     * @param n Looks for n bytes to allocate, returns a pointer to said bytes.
     * after allocation there must be enough space left for a valid block
     * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
     * choose the first block that fits
     */
    pointer allocate (size_type n) {
		bad_alloc exception;
		if (n < 1) throw exception;
		pointer block = 0;
		if (n == 0) return block;
		n *= sizeof(T);
		size_type pos = 0, cur = 0;
		while (pos+cur+8 <= N) {
			cur = view(*(a+pos));
			//assert (pos + cur + 8 <= N);
			if (cur >= n+8 && pos + cur + 8 <= N) {
				block =  reinterpret_cast<T*>(a+pos+4);
				if ((unsigned)((pos+cur+8)-(pos+n+8)) < (unsigned)sizeof(T)+8) {
					// give them all
					view(*(a+pos)) = -cur;
					view(*(a+pos+cur+4)) = -cur;
				}
				else { 
					// give them what they need and fix sentinels
					view(*(a+pos)) = -n;
					view(*(a+pos+n+4)) = -n;
					view(*(a+pos+n+8)) = view(*(a+pos+cur+4)) = cur - n - 8;
				}
				assert(valid());
				return block;
			}
			pos += (abs(cur) + 8); // skip taken block
		}
        assert(valid());
     	throw exception;
        return 0;
    }

    // ---------
    // construct
    // ---------

    /**
     * O(1) in space
     * O(1) in time
     * Create a new T based on v at location p. Do not allocate, just construct.
     */
    void construct (pointer p, const_reference v) {
        new (p) T(v);	
        assert(valid());
    } 

    // ----------
    // deallocate
    // ----------

    /**
     * O(1) in space
     * O(1) in time
     * Deallocate block located at p. Coalesce all adjacent free blocks.
     * after deallocation adjacent free blocks must be coalesced
     */
    void deallocate (pointer p, size_type = 0) {
		//cout << "Deallocating..." << endl;
		
		size_type pre = -1, nex = -1, cur = -1, newsize = 0;
        //check the previous and next blocks
		
		if (p != (pointer)(a+4)) pre = view(*(reinterpret_cast<char*>(p)-8));
		
		cur = view(*(reinterpret_cast<char*>(p)-4));
		
		if (abs(cur)+4 != N-4) nex = view(*(reinterpret_cast<char*>(p)-cur+4));
		
		assert(cur < 0);
		
		if (pre > 0) { newsize += pre+8; pre +=8; }
		else pre = 0;
		
		if (nex > 0) newsize += nex+8;
		else if (cur == -N+8) nex = -8;
		else nex = -8;
		
		//cout << pre << ", " << cur << ", " << nex << endl;
		newsize -= cur;
		view(*(reinterpret_cast<char*>(p)-pre-4)) = view(*(reinterpret_cast<char*>(p)+8-cur+nex)) = newsize;
        assert(valid());
    } 

    // -------
    // destroy
    // -------

    /**
     * O(1) in space
     * O(1) in time
     * Destroy the T at location p.
     */
    void destroy (pointer p) {
        p->~T();
        assert(valid());
    }
	
	// ----
	// view
	// ----
	
	/**
	 * Take a byte, return it and the following 4 bytes.
	 *
	 */
	int& view (char& c) const {
		return *reinterpret_cast<int*>(&c);
	}
	
	
};

#endif // Allocator_h
