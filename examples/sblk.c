#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <omp.h>
#include <liblightnvm.h>

#include <sys/time.h>

size_t start, stop;

size_t wclock_sample(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec + tv.tv_sec * 1000000;
}

size_t timer_start(void)
{
    start = wclock_sample();
    return start;
}

size_t timer_stop(void)
{
    stop = wclock_sample();
    return stop;
}

double timer_elapsed(void)
{
    return (stop-start)/(double)1000000.0;
}

void timer_pr(const char* tool)
{
    printf("Ran %s, elapsed wall-clock: %lf\n", tool, timer_elapsed());
}

int io(NVM_DEV dev, NVM_GEO geo, size_t blk_idx, int flags)
{
	int nerr = 0;

	int nchannels = 1;
	int nluns = 1;

	int ch;
	for (ch = 0; ch < nchannels; ++ch) {
		int lun;
		for (lun = 0; lun < nluns; ++lun) {

			int buf_nbytes = geo.vpage_nbytes;
			char *buf = nvm_buf_alloc(geo, buf_nbytes);
			nvm_buf_fill(buf, buf_nbytes);

			NVM_VBLOCK vblk;
			NVM_ADDR addr;

			addr.ppa = 0;
			addr.g.ch = ch;
			addr.g.lun = lun;
			addr.g.blk = blk_idx;

			vblk = nvm_vblock_new_on_dev(dev, addr.ppa);

			int pg;
			for (pg = 0; pg < geo.npages; ++pg) {
				ssize_t err;
				switch (flags) {
					case 0x1:
						err = nvm_vblock_pwrite(vblk, buf, pg);
						if (err) {
							++nerr;
							printf("write err(%ld)\n", err);
						}
						break;
					case 0x2:
						err = nvm_vblock_pread(vblk, buf, pg);
						if (err) {
							++nerr;
							printf("read err(%ld)\n", err);
						}
						break;
					default:
						printf("invalid IO!");
						break;
				}
			}
			nvm_vblock_free(&vblk);
			free(buf);
		}
	}

	return nerr;
}

// From hereon out the code is mostly boiler-plate for command-line parsing,
// there is a bit of useful code exemplifying:
//
//  * nvm_dev_open
//  * nvm_dev_close
//  * nvm_dev_attr_geo
//
// as well as using the NVM_ADDR data structure.

#define NVM_CLI_CMD_LEN 50

typedef struct {
	char name[NVM_CLI_CMD_LEN];
	int (*func)(NVM_DEV, NVM_GEO, size_t blk_idx, int);
	int argc;
	int flags;
} NVM_CLI_VBLK_CMD;

static NVM_CLI_VBLK_CMD cmds[] = {
	{"write", io, 4, 0x1},
	{"read", io, 4, 0x2},
};

static int ncmds = sizeof(cmds) / sizeof(cmds[0]);
static char *args[] = {"dev_name", "blk"};

void _usage_pr(char *cli_name)
{
	int cmd;

	printf("Usage:\n");
	for (cmd = 0; cmd < ncmds; cmd++) {
		int arg;
		printf(" %s %6s", cli_name, cmds[cmd].name);
		for (arg = 0; arg < cmds[cmd].argc-2; ++arg) {
			printf(" %s", args[arg]);
		}
		printf("\n");
	}
}

int main(int argc, char **argv)
{
	char cmd_name[NVM_CLI_CMD_LEN];
	char dev_name[DISK_NAME_LEN+1];
	int ret, i;

	NVM_CLI_VBLK_CMD *cmd = NULL;
	
	NVM_DEV dev;
	NVM_GEO geo;
	size_t blk_idx;

	if (argc < 3) {
		_usage_pr(argv[0]);
		return -1;
	}
							// Get `cmd_name`
	if (strlen(argv[1]) < 1 || strlen(argv[1]) > (NVM_CLI_CMD_LEN-1)) {
		printf("Invalid cmd\n");
		_usage_pr(argv[0]);
		return -EINVAL;
	}
	memset(cmd_name, 0, sizeof(cmd_name));
	strcpy(cmd_name, argv[1]);

	for (i = 0; i < ncmds; ++i) {			// Get `cmd`
		if (strcmp(cmd_name, cmds[i].name) == 0) {
			cmd = &cmds[i];
			break;
		}
	}
	if (!cmd) {
		printf("Invalid cmd(%s)\n", cmd_name);
		_usage_pr(argv[0]);
		return -EINVAL;
	}

	if (argc != cmd->argc) {			// Check argument count
		printf("Invalid cmd(%s) argc(%d) != %d\n",
			cmd_name, argc, cmd->argc);
		_usage_pr(argv[0]);
		return -1;
	}

	if (strlen(argv[2]) > DISK_NAME_LEN) {		// Get `dev_name`
		printf("len(dev_name) > %d\n", DISK_NAME_LEN);
		return -1;
	}
	memset(dev_name, 0, sizeof(dev_name));
	strcpy(dev_name, argv[2]);

	blk_idx = atol(argv[3]);

	dev = nvm_dev_open(dev_name);			// open `dev`
	if (!dev) {
		printf("Failed opening device, dev_name(%s)\n", dev_name);
		return -EINVAL;
	}
	geo = nvm_dev_attr_geo(dev);			// Get `geo`
	ret = cmd->func(dev, geo, blk_idx, cmd->flags);

	nvm_dev_close(dev);				// close `dev`

	return ret;
}
