/*

client side for UsbIrNEC 

*/

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/ttycom.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "crc.h"

void action(char *cmd, int delay_ms);

int verbose = 0;
struct timeval prev_action = { 0, 0};
uint32_t prev_action_crc = 0;
char prev_action_cmd[128];
int prev_action_delay_ms = -1;

int main(int argc, char **argv) {
	int o, cmd, d, s=1;
	char *dev, *self = *argv;

	while ((o = getopt(argc, argv, "hv")) != -1) {
		switch (o) {
		case 'v':
			verbose++;
			break;
		case 'h':
			printf("version: %s\n", SVN_REV);
			printf("apple remote control for mpc (UsbIrNEC)\n");
			printf("usage: %s [-vh] (device)\n", self);
			printf(" -v     increase output verbosity\n");
			printf(" -h     show this help\n");
			exit(0);
		}
	}
	argc -= optind;
	argv += optind;

	dev = (argc==1) ? argv[0] : "/dev/uhid0";

	if (verbose > 1)
		printf("open(%s) %d\n",dev, verbose);

	while (1) {
		if ((d = open(dev,O_RDONLY))>0) {
			s=1;
			printf("ready\n");
		}

		while (read(d,&cmd,4) > 0) {
			if (verbose>=2)
				printf("rcmd=0x%08x \n", cmd);

			//TODO: verify remote_id here
	
			cmd &= 0xff;

			if (cmd == 0)
				continue;

			switch(cmd) {
			case 0x03:
				action("mpc random", 500);
				break;
			case 0x0b:
				action("mixer pcm +5", 50);
				break;
			case 0x0d:
				action("mixer pcm -5", 50);
				break;
			case 0x07:
				action("mpc next", 500);
				break;
			case 0x09:
				action("mpc prev", 500);
				break;
			case 0x05:
				action("mpc `mpc | grep playing | wc -l | sed 's/1/pause/' | sed 's/0/play/'`", 500);
				break;

			case 0x01:
				action(NULL, 0);

				break;
			default:
				printf("unknown cmd=0x%02x \n", cmd);
			}
		}

		close(d);
		printf("reopening %s in %ds...\n",dev,s);
		sleep(s);
		if (s < 64)
			s = s*2;
	}

	return 0;
}

void action(char* cmd, int delay_ms) {
	
	struct timeval now;
	int delta;
	uint32_t crc;
	char * exec;

	gettimeofday(&now, 0);

	// reexecute command?
	if (!cmd) {
		if (verbose>=2)
			printf("reexec command \"%s\" (delay_ms=%dms)\n", prev_action_cmd, prev_action_delay_ms);
		cmd = prev_action_cmd;
		delay_ms = prev_action_delay_ms;
	}

	// invalid command?
	if (delay_ms < 0)
		return;


	crc = crc32((uint8_t*)cmd, strlen(cmd));
	if (verbose>=3)
		printf("crc=%x prev_action_crc=%x\n", crc, prev_action_crc);

	delta = now.tv_sec - prev_action.tv_sec;
	if (delta <= 1 && crc==prev_action_crc) {
		delta *= 1000000;
		delta += (now.tv_usec - prev_action.tv_usec);

		if (delta <= delay_ms*1000) {
			if (verbose >= 2)
				printf("skip action %s (only %.2fms elapsed)", cmd, (float)delta/1000);
			return;
		}
	}

	memcpy(&prev_action, &now, sizeof(now));
	prev_action_crc = crc;

	strncpy(prev_action_cmd, cmd, sizeof(prev_action_cmd)-1);
	prev_action_cmd[sizeof(prev_action_cmd)-1] = 0;
	prev_action_delay_ms = delay_ms;

	exec = malloc (strlen(cmd) + 20);
	sprintf(exec,"%s > /dev/null", cmd);
	if (verbose)
		printf("exec: %s\n",cmd);
	system(exec);
	free(exec);
}

