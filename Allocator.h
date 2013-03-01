// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2013
// Glenn P. Downing
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
		std::cout << "[VALID] Validating..." << endl;
		
		for (int i = 0; i < N; i+=4) {
			cout << view(*(const_cast<char*>(a)+i)) << endl;
		}
		
        size_type cur = 0, pos = 0;
		while (pos+cur+8 <= N) {
			cur = view(*((const_cast<char*>(a)+pos)));
			cout << "[" << cur << "]=[" << view(*(const_cast<char*>(a)+pos+abs(cur)+4)) << "]" << endl;

			if (cur != view(*((const_cast<char*>(a))+pos+abs(cur)+4))) return false;
			if (cur < 0) cur *= -1;
			pos += (cur + 8);
		}
		cout <<  "...all good!" << endl;
        return true;
    }

public:
    // ------------
    // constructors
    // ------------

    /**
     * O(1) in space
     * O(1) in time
     * <your documentation>
     */
    Allocator () {
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
     * @param n Takes looks 
     * after allocation there must be enough space left for a valid block
     * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
     * choose the first block that fits
     */
    pointer allocate (size_type n) {
		std::cout << "[ALLOC]Allocating for " << n*sizeof(T) << " bytes!" << std::endl;
		assert(n > 0);
		n *= sizeof(T);
		size_type pos = 0, cur = 0;
		while (pos+cur+8 <= N) {
			cur = view(*(a+pos));
			assert (pos + cur + 8 <= N);
			if (cur < 1 || cur < n) {
				pos += (cur * -1 + 8); // skip taken block
				continue; // move on to next block
			}
			if (cur >= n && pos + cur + 8 <= N) {
				pointer block =  reinterpret_cast<T*>(a+pos+4);
				cout << "[ALLOC]Setting lead sentinel to " << view(*(a+pos)) << endl;
				if ((unsigned)((pos+cur+8)-(pos+n+8)) < (unsigned)sizeof(T)+8) {
					// give them all
					cout << "Problem?" << endl;
					
					view(*(a+pos)) = -cur;
					view(*(a+pos+cur+4)) = -cur;
				}
				else { 
					// give them what they need and fix sentinels
					cout << cur << ", " << n << endl;
					
					view(*(a+pos)) = -n;
					view(*(a+pos+n+4)) = -n;
					view(*(a+pos+n+8)) = view(*(a+pos+cur+4)) = cur - n - 8;
					cout << "[ALLOC]Setting end sentinel to " << view(*(a+pos+n+4)) << endl;
					cout << "[ALLOC]Block after is " << view(*(a+pos+n+8)) << " - " << view(*(a+pos+cur+4)) << endl;
				}
				
				assert(valid());
				return block;
			}
			
		}
		
        assert(valid());
		throw bad_alloc();
        return 0;
    }                   // replace!

    // ---------
    // construct
    // ---------

    /**
     * O(1) in space
     * O(1) in time
     * <your documentation>
     */
    void construct (pointer p, const_reference v) {
		cout << "[CONSTRUCT]Constructing at " << p-(pointer)a << endl;
        new (p) T(v);                            // uncomment!		
        assert(valid());
    } 

    // ----------
    // deallocate
    // ----------

    /**
     * O(1) in space
     * O(1) in time
     * <your documentation>
     * after deallocation adjacent free blocks must be coalesced
     */
    void deallocate (pointer p, size_type = 0) {
		cout << "[DEALLOC] Deallocating at " << p-(pointer)a << endl;
		size_type pre = -1, nex = -1, cur = -1, newsize = 0;
		
        //check the previous and next blocks
		if (p != (pointer)(a+4)) pre = view(*(reinterpret_cast<char*>(p)-8));
		cur = view(*(reinterpret_cast<char*>(p)-4));
		if ((pointer) a+N != p+cur+4) nex = view(*(reinterpret_cast<char*>(p)-cur+4));
		
		assert(cur < 0);
		if (pre > 0) {
			newsize += pre+8;
			//pre+=8;
		}
		else pre = 0;
		
		if (nex > 0) {
			newsize += nex+8;
			//nex+= (nex + 4) ;
		}
		else nex = 0;
		
		newsize -= cur;
		cout << pre << ", " << cur << ", " << nex << endl;
		view(*(reinterpret_cast<char*>(p)-pre-4)) = view(*(reinterpret_cast<char*>(p)+8-cur+nex)) = newsize;
		
		cout << "setting " << (p-pre-4) - (pointer) a << " to " << view(*reinterpret_cast<char*>(p-pre)) << endl;
        assert(valid());
    } 

    // -------
    // destroy
    // -------

    /**
     * O(1) in space
     * O(1) in time
     * <your documentation>
     */
    void destroy (pointer p) {
		cout << "[DESTROY] Destroying at " << p-(pointer)a << endl;
        p->~T();            // uncomment!
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
