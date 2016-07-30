# Memory Allocator
This is a toy project of a memory allocator.
Takes a class Complex as a class example for create and delete instances and test the memory manager.
When you create a new instance with the keyword *new*, you can specify the memory manager to use.
I'm confident the program has no leaks, but can't say it for sure, can't find a program or something similar to watch because all the programs I tried crashed.

There are three approaches:
- Base memory: a simple wrap of malloc and free functions, without any strategy.
- Pool memory: uses a pool approach with linked lists
- Pool Bitmapped memory: uses a pool approach using a map of bit to store the availability of the memory

With a simple test, I have this values.
The pool bitmapped is badly implemented at the moment due to the absurding long time it takes when it should go better than Base memory at least.
All in milliseconds

Normal|Base|Pool|Pool bitmapped
|:-------------: |:-------------:| :-----:| :-----:|
|1415|1804.4 |763.2|8623.2|
|1433.4|1807.4|775.2|47199.8|


Improvements I want to implement:
- Fix and try to improve the Pool BitMapped Memory or see what kind of situation is better than the other approaches
- Adapt the Pool Memory to accept any kind of class without modifications from the user (a general-purpose memory manager).
- Same as above with the Pool BitMapped Memory
- Implement a Free Lists based memory manager from this link (Tutorial from IBM)[https://www.ibm.com/developerworks/aix/tutorials/au-memorymanager/]