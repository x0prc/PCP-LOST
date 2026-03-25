#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include "msg_spray.h"

#define DEVICE "/dev/pcp_probe"
#define MAX_PAGES 1024


// PFN snapshots before/after operations → used to detect reuse/overlap
unsigned long pfns1[MAX_PAGES];
unsigned long pfns2[MAX_PAGES];

// timing samples → used for PCP oracle
uint64_t times[MAX_PAGES];

void pin_cpu(int cpu)
{
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);

    // PCP is per-CPU → pinning removes cross-CPU noise
    sched_setaffinity(0, sizeof(set), &set);
}

int is_pcp(uint64_t t)
{
    // heuristic classifier:
    // PCP allocations are fast, buddy allocations slower
    // threshold must be tuned per system
    return t < 2000;
}

void groom(int fd, int n)
{
    // adaptive loop: tries to push allocator into PCP-dominant state
    for (int i = 0; i < 500; i++) {

        ioctl(fd, 1, &n);   // allocate
        ioctl(fd, 4, times); // fetch timings

        int hits = 0;

        // estimate how many allocations came from PCP
        for (int j = 0; j < n; j++)
            if (is_pcp(times[j]))
                hits++;

        ioctl(fd, 2, 0); // free → refill PCP

        // stop once PCP appears sufficiently populated
        if (hits > n / 2)
            break;
    }
}

int cross_cache_trial(int fd, int n)
{
    // Step 1: allocate pages (baseline state)
    ioctl(fd, 1, &n);
    ioctl(fd, 3, pfns1);

    // Step 2: free → push into PCP
    ioctl(fd, 2, 0);

    // Step 3: groom PCP to bias reuse
    groom(fd, n);

    // Step 4: trigger slab allocations in a different cache
    int q = spray_msgs(500);
    free_msgs(q, 500);

    // Step 5: allocate again → observe which pages are reused
    ioctl(fd, 1, &n);
    ioctl(fd, 3, pfns2);

    int overlap = 0;

    // detect cross-cache reuse:
    // same PFN appearing across phases implies page reuse
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (pfns1[i] == pfns2[j])
                overlap++;

    return overlap;
}

int main()
{
    int fd = open(DEVICE, O_RDWR);
    int n = 64;

    pin_cpu(0); // critical for PCP stability

    int total = 0;

    for (int i = 0; i < 200; i++) {

        // each trial attempts to induce cross-cache reuse
        int o = cross_cache_trial(fd, n);

        total += o;

        printf("[trial %d] overlap = %d\n", i, o);
    }

    // aggregate metric used in evaluation section
    printf("Average overlap: %.2f\n", total / 200.0);

    close(fd);
    return 0;
}
