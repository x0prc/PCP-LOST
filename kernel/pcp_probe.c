#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>

#define MAX_PAGES 1024 // upper bound to keep interface simple

// persistent arrays
static struct page *pages[MAX_PAGES];
static unsigned long pfns[MAX_PAGES]; // physical frame numbers
static u64 times[MAX_PAGES];     // allocation latency (timing oracle)
static int page_count; 

static long pcp_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    int i;

    switch (cmd) {

    case 1: // allocate N pages
        // user controls allocation pressure → influences PCP state
        if (copy_from_user(&page_count, (int __user *)arg, sizeof(int)))
            return -EFAULT;

        if (page_count > MAX_PAGES)
            return -EINVAL;

        for (i = 0; i < page_count; i++) {

            // timing window used to distinguish PCP (fast) vs buddy (slow)
            u64 t1 = ktime_get_ns();

            pages[i] = alloc_pages(GFP_KERNEL, 0);

            u64 t2 = ktime_get_ns();

            if (!pages[i])
                return -ENOMEM;

            // PFN is used to detect physical page reuse across allocations
            pfns[i] = page_to_pfn(pages[i]);

            // latency recorded for userland classifier (PCP oracle)
            times[i] = t2 - t1;
        }
        break;

    case 2: // free all previously allocated pages
        // freeing feeds pages into PCP (critical for grooming)
        for (i = 0; i < page_count; i++) {
            if (pages[i])
                __free_pages(pages[i], 0);
        }
        break;

    case 3: // return PFNs to userland
        // enables reuse detection and cross-cache overlap measurement
        if (copy_to_user((void __user *)arg,
                         pfns,
                         sizeof(unsigned long) * page_count))
            return -EFAULT;
        break;

    case 4: // return timing data
        // exposes allocator path indirectly (PCP vs buddy)
        if (copy_to_user((void __user *)arg,
                         times,
                         sizeof(u64) * page_count))
            return -EFAULT;
        break;

    default:
        return -EINVAL;
    }

    return 0;
}





