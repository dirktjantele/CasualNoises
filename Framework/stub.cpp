#include <errno.h>
#include <stddef.h>

int _getentropy(void *buffer, size_t length)
{
    (void)buffer;
    (void)length;
    return -ENOSYS;   // Function not implemented
}

