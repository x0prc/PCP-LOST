#include "msg_spray.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

// msg_msg objects live in kmalloc caches → used for cross-cache interaction
struct msg {
    long mtype;
    char mtext[512];  // size chosen to hit a predictable kmalloc-* cache
};


