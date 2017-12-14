# Thread-Safe-Q
Thread Safe FIFO multi readers &amp; multi writers concurrent access 
#Implemented in C++11

Design and develop a thread-safe container class called ThreadSafeContainer.

 The class shall conform to the following restrictions:
 
i. The data will be removed in a first in first out order.

ii. The class with be a template/generic container class

iii. The class will be initialized with a capacity at construction time.

iv. The add method will add one element to the FIFO and, if needed, block
the caller until space is available.

v. The remove method will block the caller until an element is available for
retrieval.

vi. The clear method will remove any objects which were added, but not yet
removed.

vii. The shutdown method will cause any blocked or future calls to add or
remove to throw a ShutdownException .
