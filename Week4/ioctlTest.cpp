#include <sys/ioctl.h>  /* ioctl */
#include <stdio.h>
#include <stdlib.h>
int main() {
    int fd = open("/dev//dev/Character-driver-device", O_RDWR);
    ioctl(fd, HELLO_WORLD);
}
