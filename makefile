all:
	make test

doc:
	doxygen Doxyfile

log:
	git log > Allocator.log

TestAllocator: TestAllocator.c++ Allocator.h
	g++ -pedantic -std=c++0x -Wall TestAllocator.c++ -o TestAllocator -lcppunit -ldl

test: TestAllocator
	TestAllocator

testv: TestAllocator
	valgrind TestAllocator

turnin-list:
	turnin --list reza cs371ppj3

turnin-submit:
	turnin --submit reza cs371ppj3 Allocator.zip

turnin-verify:
	turnin --verify reza cs371ppj3

zip:
	zip -r Allocator.zip html/ makefile Allocator.h Allocator.log \
	TestAllocator.c++ TestAllocator.out

clean:
	rm -f TestAllocator
