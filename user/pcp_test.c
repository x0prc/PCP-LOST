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

