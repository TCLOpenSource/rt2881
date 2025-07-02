#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>

#define DEVICE_NAME "/dev/wov_gaowan"
#define WOV_MAGIC 'w'
#define RTK_WOV_WAIT _IOR(WOV_MAGIC, 1, int)
#define RTK_WOV_READ_PHY _IOR(WOV_MAGIC, 2, struct ioctl_data)
#define RTK_WOV_WRITE_PHY _IOR(WOV_MAGIC, 3, struct ioctl_data)

struct ioctl_data {
    unsigned long addr;
    unsigned long value;
};

unsigned long read_phy_addr(unsigned long addr)
{
    struct ioctl_data data = {addr, 0};

    int fd = open(DEVICE_NAME, O_RDWR);
    if (fd == -1) {
        // perror("open failed\n");
        return -1;
    }

    if (ioctl(fd, RTK_WOV_READ_PHY, &data) == -1) {
        // perror("ioctl failed\n");
        close(fd);
        return -1;
    }

    close(fd);
    return data.value;
}

int write_phy_addr(unsigned long addr, unsigned long value)
{
    struct ioctl_data data = {addr, value};

    int fd = open(DEVICE_NAME, O_RDWR);
    if (fd == -1) {
        // perror("open failed\n");
        return 1;
    }

    if (ioctl(fd, RTK_WOV_WRITE_PHY, &data) == -1) {
        // perror("ioctl failed\n");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}

int main() {
    int ret = 0;
    unsigned long val = 0;
    struct timespec req, rem;

    req.tv_sec = 0;
    req.tv_nsec = 100 * 1000000L;  // 500 milliseconds

    ret = write_phy_addr(0x1a800010, 0xAABBCCDD);
    if (ret) {
        // perror("Failed to write the phy addr\n");
        return EXIT_FAILURE;
    }

    ret = read_phy_addr(0x1a800010);
    printf("Phy address value = 0x%x\n", ret);

    int fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        // perror("Failed to open the device\n");
        return EXIT_FAILURE;
    }

    while (1) {
        // Sending RTK_WOV_WAIT command
        if (ioctl(fd, RTK_WOV_WAIT, NULL) < 0) {
            // perror("Failed to send RTK_WOV_WAIT command\n");
            close(fd);
            return EXIT_FAILURE;
        }
        printf("Do hotword detection here.\n");
        nanosleep(&req, &rem); // Add a small delay to prevent flooding the kernel with ioctl calls
    }

    close(fd);
    return EXIT_SUCCESS;
}
