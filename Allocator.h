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
        size_type cur = 0, pos = 0;
		while (cur < N) {
			cur = view(*(a+pos));
			if (cur < 1) cur *= -1;
			if (cur != view(*(a+pos+cur+4))) return false;
			pos += (cur + 8);
		}
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
        *a = N - 8;
		*(a + N - 4) = N - 8;
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
		assert(n > 0);
		n *= sizeof(T);
		size_type pos = cur = 0;
		while (pos < N) {
			cur = view(*(a+pos));
			assert (pos + cur + 8 <= N);
			if (cur < 1) {
				pos += (cur * -1 + 8); // skip taken block
				continue; // move on to next block
			}
			if (cur >= n && pos + cur + 8 < N) {
				pointer block = a+pos;
				*(a+pos) = -n;
				if ((pos+cur+8)-(pos+n+8) < sizeof(T)+8) {
					// give them all
					*(pos+cur+4) = -n;
				}
				else { 
					// give them what they need and fix sentinels
					*(pos+n+4) = -n;
					*(pos+n+8) = *(pos+cur+4) = cur - n - 8;
				}
				return block;
			}
		}
		
        assert(valid());
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
        // <your code>
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
        // p->~T();            // uncomment!
        assert(valid());
    }
	
	// ----
	// view
	// ----
	
	/**
	 * Take a byte, return it and the following 4 bytes.
	 *
	 */
	int& view (char& c) {
		return *reinterpret_cast<int*>(&c);
	}
};

#endif // Allocator_h
