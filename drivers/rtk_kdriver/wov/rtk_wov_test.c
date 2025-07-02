/* WOV user program for test if wov driver successfully sends uevent to Sound Trigger HAL
 *
 * Usage:
 * 1. Compile user program
 *    ./arm-linux-gnueabi-gcc -o wov_test --static rtk_wov_test.c
 * 2. Push wov_test into board by usb or adb
 * 3. Run in background and export log
 *    ./wov_test > test.log &
 * 4. Trigger wov driver to send uevent
 *    echo 1 > /dev/wov_gaowan
 * 5. Check result from user program log
 *    cat test.log
 * 6. You can see log like below:
 *    Received uevent:
 *    change@/devices/virtual/wov/wov_gaowan
 *    Env var: change@/devices/virtual/wov/wov_gaowan
 *    Env var: ACTION=change
 *    Env var: DEVPATH=/devices/virtual/wov/wov_gaowan
 *    Env var: SUBSYSTEM=wov
 *    Env var: CMD=COMMAND_RECOGNITION_TRIGGER
 *    Env var: UUID=7038ddc8-30f2-11e6-b0ac-40a8f03d3f15
 *    Env var: MAJOR=469
 *    Env var: MINOR=0
 *    Env var: DEVNAME=wov_gaowan
 *    Env var: SEQNUM=3648
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>

// #include <rtd_log/rtd_module_log.h>

#define UEVENT_BUFFER_SIZE 2048

int main()
{
	int sock_fd;
	struct sockaddr_nl addr;
	char buf[UEVENT_BUFFER_SIZE];

	// Create netlink socket
	sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);
	if (sock_fd == -1) {
		// perror("socket");
		return 1;
	}

	// Bind socket
	memset(&addr, 0, sizeof(addr));
	addr.nl_family = AF_NETLINK;
	addr.nl_pid = getpid();
	addr.nl_groups = 1; // Subscribe to multicast group 1
	if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		// perror("bind");
		close(sock_fd);
		return 1;
	}

	// Receive and print uevents
	while (1) {
		ssize_t count = recv(sock_fd, buf, sizeof(buf), 0);
		if (count == -1) {
			// perror("recv");
			close(sock_fd);
			return 1;
		}

		// Ensure null-termination of received data
		buf[count] = '\0';

		// Print received uevent data
		printf("Received uevent:\n%s\n", buf);

		// Process envp (environment variables)
		char *envp_start = buf;
		while (*envp_start) {
			printf("Env var: %s\n", envp_start);
			envp_start += strlen(envp_start) + 1;
		}

		printf("\n");
	}

	close(sock_fd);
	return 0;
}
