# Kernel
Implementation of a small, but completely functional operating system kernel. Supports concepts such as multithreading, time sharing, semaphores, events and asynchronous signals.

Kernel is implemented so the user application and the kernel itself share the same address space. In other words, they are parts of a single program. Concurrent processes created within the application are actually lightweight processes (threads). The application and the kernel are compiled and linked together to produce a single executable program. I.e. they are written as a unique source code.

Within this project, a special subsystem of the kernel is implemented - the thread management subsystem. This subsystem implements the concept of threads (as well as the services for creating and running threads), the concept of semaphores and events triggered by interrupts, and it provides the support for time sharing.

For the purpose of demonstration, easier developement, testing and experimenting, the kernel and the application need to be executed on any type of 32-bit Windows operating system like a console application.
