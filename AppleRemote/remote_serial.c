/*
 -----------------------------------------------------------------------------
 "THE BEER-WARE LICENSE":

 <johnny@netvor.sk> wrote this file. As long as you retain this notice you
 can do whatever you want with this stuff. If we meet some day, and you think
 this stuff is worth it, you can buy me a beer (*or Kofola) in return.   

      johnny ^_^ <johnny@netvor.sk>
 -----------------------------------------------------------------------------

 apple remote mpc control
 ========================

 uses lirc-like hardware ir receiver (DCD active on High) inside com2 ^^

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


struct APPLE_CMD {
	unsigned int remote: 10;
	unsigned char id: 3;
	unsigned int __stub: 1;
	unsigned int sync: 18;
};

unsigned int ONE(pos) {
	if (pos >= 32)
		return 0;
	return 1 << 31-pos;
}

int command();
void action(char *cmd);
void printb(unsigned int d);

int verbose = 0;

void print_help() {
	printf("apple remote control for mpc (lirc-like hw)\n");
	printf("http://netvor.sk/~johnny/projects/apple_remote_mpc/\n\n");
	printf(" -v     verbose output\n");
	printf(" -vv    even more verbose output\n");
	printf(" -vvv   .. and also some timing information\n");
	printf(" -l N   allow access only from remote id N (use -v to get rid)\n\n");
	printf("johnny ^_^ <johnny@netvor.sk>, 2008)\n\n");
}

int main(int argc, char **argv) {
	int raw, prev, o, limit=0;
	struct APPLE_CMD cmd;

	while ((o = getopt(argc, argv, "hvl:")) != -1) {
		switch (o) {
		case 'v':
			verbose++;
			break;

		case 'l':
			limit = atol(optarg);
			break;

		case 'h':
			print_help();
			exit(0);

		default:
			fprintf(stderr, "invalid argument...\n");
			print_help();
			exit(1);
		}
	}
	argc -= optind;
	argv += optind;

	printf("start (%d)\n",verbose);

	while (1) {
		raw = command();

		if (verbose >= 2) 
			printb(raw);
		else if (verbose)
			printf("0x%08x -- ",raw);

		switch(raw) {
		case -1:
			// bad action
			continue;

		case 0x40000000:
			// repeat previous action
			raw=prev;

		default:
			memcpy(&cmd,&raw,4);

			if (cmd.sync != 0x000177e1) {
				if (verbose)
					printf("not mac command? (0x%04x) ", cmd.sync);
				break;
			}

			if (verbose){
				printf("rid=%d ",cmd.remote);
			}

			if (limit && cmd.remote != limit) {
				if (verbose)
					printf("accepting only remote: %d ",limit);
				break;
			}

			switch(cmd.id) {
			case 0x04:
				action("mpc random");
				break;
			case 0x05:
				action("mpc volume +5");
				break;
			case 0x06:
				action("mpc next");
				break;
			case 0x03:
				action("mpc volume -5");
				break;
			case 0x01:
				action("mpc prev");
				break;
			case 0x02:
				action("mpc `mpc | grep playing | wc -l | sed 's/1/pause/' | sed 's/0/play/'`");
				break;
			default:
				printf ("cmd=0x%02x ", cmd.id);
				if (verbose)
					printf(" -- unknwon");
			}

			prev=raw;
		}
		if (verbose)
			printf("\n");


	}

	return 0;
}

int command() {
	int pstate=0,state=0,tty,delta=0,cmd=0,pos=0;
	struct timeval tvStart, tvDelta;

	if (!(tty = open("/dev/ttyd1",O_RDONLY))) {
		perror("open failed");
		return  -1;
	}

	gettimeofday(&tvStart,0);

	// 15ms of stable signal to get us out of here 
	while (delta < 15000) {
		// get current state of DCD
		if (ioctl(tty,TIOCMGET,&state) == -1) {
			perror("ioctl failed");
			return -1;
		}
		state = ((state & TIOCM_DCD) != 0);

		// calculate time difference from start
		gettimeofday(&tvDelta,0);
		tvDelta.tv_usec -= tvStart.tv_usec;	
		tvDelta.tv_usec += (tvDelta.tv_sec-tvStart.tv_sec)*1000000;
		delta = tvDelta.tv_usec;

		if (verbose >= 3)
			printf("state %d after %d ms\n", state, delta);

		// react to change of state
		if (pstate!=state) {
//			if (!state)
//				printf("%dus of state %d\n", tvDelta.tv_usec, pstate);
			if (state && delta > 8500) {
				//INIT
			} else if (state && delta>1500) {
				cmd |= ONE(pos);
				pos += 1;
			} else if (state && delta>700) {
				// error (2x0?)
				pos += 2;
			} else if (state) {
				pos += 1;
			}
				
//			if (state && delta < 5000) 		
//				printf("%c", ((delta > 1000) ? '1' : '0'));

			gettimeofday(&tvStart,0);
			pstate = state;
		}
	}

	close(tty);

	return cmd;
}

void action(char* cmd) {
	char * exec = malloc (strlen(cmd) + 20);
	sprintf(exec,"%s > /dev/null", cmd);
	system(exec);
	free(exec);
}

void printb(unsigned int d) {
	int i;
	for (i=0; i!=32; i++) {
		if ((i%8)==0 && i>0)
			printf(".");

		printf("%c",((d&(1<<i)) != 0) ? '1' : '0');
	}
}
