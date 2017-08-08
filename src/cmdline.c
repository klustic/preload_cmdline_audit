/*
 * cmdline.c
 *
 *  Created on: Aug 8, 2017
 *      Author: user
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "common.h"

#ifndef UDS_SOCKNAME
#define UDS_SOCKNAME	"/tmp/audit.sock"
#endif

struct msg_s
{
	uint32_t	time;
	uint32_t	pid;
	char		user[256];
	char		cmdline[1024];
};

void
__attribute__((constructor))
__ini(void)
{
	int fd=-1, uds=-1;
	char *user = NULL;
	struct msg_s msg = {0};
	struct sockaddr_un addr = {.sun_family=AF_UNIX};

//	puts("Loaded.");

	/* Find the listening UNIX socket (UDS) and connect to it */
	if ((uds = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		goto error;
	strncpy(addr.sun_path, UDS_SOCKNAME, sizeof(addr.sun_path)-1);
	if (connect(uds, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		goto error;

	/* Open the commandline arguments in /proc for reading */
	if ((fd = open("/proc/self/cmdline", O_RDONLY)) < 0)
		goto error;

	/* Construct a message containing size/time/pid/user/cmdline */
	msg.time = time(NULL);
	msg.pid = getpid();

	if (!(user = getenv("USER")))
		user = "<user unknown>";
	strncpy(msg.user, user, sizeof(msg.user)-1);

	if (read(fd, msg.cmdline, sizeof(msg.cmdline)) <= 0)
		goto error;
	for (off_t i=0; i < sizeof(msg.cmdline)-1; i++)
		if (msg.cmdline[i] == 0 && msg.cmdline[i+1] != 0)
			msg.cmdline[i] = ' ';  // Replace NULL delimiters with spaces

	/* Send message to UDS */
	if (send(uds, &msg, sizeof(msg), 0) != sizeof(msg))
		goto error;

	goto cleanup;

error:
//	perror("libcmdline.so");
cleanup:
	if (fd >= 0)
		close(fd);
	if (uds >= 0)
		close(uds);
	return;
}
