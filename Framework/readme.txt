color picker
------------

https://barth-dev.de/online/rgb565-color-picker/

syscalls.c
----------

Implement "_exit()" to do something usefull when assert(...) fails.

project properties
------------------

sprintf() is used in String.h:
	C/C++ Build -> Settings -> MCU Settings -> mark checkbox "Use float with printf from newly-nano …"
 
 cmsis_gcc.h
 -----------
 
 Comment out the extern "__STATIC_FORCEINLINE __NO_RETURN void __cmsis_start(void)" function ... why ??? ...