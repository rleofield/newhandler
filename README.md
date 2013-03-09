newhandler
==========

In C++ are 3 variants of 'operator new()':

  1. an exception 'bad_alloc' is thrown
     'operator new()' don't returns NULL, like 'malloc()' in C

  2. a function is called,  can be set with 'set_new_handler()'
     'operator new()' don't returns NULL, like in 'malloc()' in C

  3. with the keyword '(nothrow)', new() return is like old plain 'malloc()' from C.
     'return' is NULL, in case of 'out of memory'

     please recognize:
     The macro NULL is in C++ different from C
     C++ NULL is (int)0
     C   NULL is (void*)0
     In C is NULL a pointer, in C++ an 'int'.


