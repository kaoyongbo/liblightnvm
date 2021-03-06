#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <liblightnvm.h>

int get(NVM_DEV dev, NVM_GEO geo, NVM_ADDR addr, int flags)
{
	NVM_VBLOCK vblk;
	ssize_t err;

	printf("** nvm_vblock_gets(..., %d, %d)\n", addr.g.ch, addr.g.lun);

	vblk = nvm_vblock_new();
	if (!vblk) {
		printf("FAILED: allocating vblk\n");
		return -ENOMEM;
	}

	err = nvm_vblock_gets(vblk, dev, addr.g.ch, addr.g.lun);
	if (err) {
		printf("FAILED: nvm_vblock_gets err(%ld)\n", err);
	} else {
		printf("got ");
		nvm_vblock_pr(vblk);
	}

	nvm_vblock_free(&vblk);

	return err;
}

int put(NVM_DEV dev, NVM_GEO geo, NVM_ADDR addr, int flags)
{
	NVM_VBLOCK vblk;
	ssize_t err;

	printf("** nvm_vblock_put(...): ");
	nvm_addr_pr(addr);

	vblk = nvm_vblock_new_on_dev(dev, addr.ppa);
	if (!vblk) {
		printf("FAILED: allocating vblk\n");
		return -ENOMEM;
	}

	err = nvm_vblock_put(vblk);
	if (err) {
		printf("FAILED: nvm_vblock_put err(%ld)\n", err);
	}

	nvm_vblock_free(&vblk);

	return err;
}

int pread(NVM_DEV dev, NVM_GEO geo, NVM_ADDR addr, int flags)
{
	NVM_VBLOCK vblk;
	ssize_t err;

	void *buf;
	int buf_len;

	printf("** nvm_vblock_pread(...): ");
	nvm_addr_pr(addr);

	vblk = nvm_vblock_new_on_dev(dev, addr.ppa);
	if (!vblk) {
		printf("FAILED: allocating vblk\n");
		return -ENOMEM;
	}

	buf_len = nvm_dev_attr_vpage_nbytes(dev);
	buf = nvm_buf_alloc(geo, buf_len);
	if (!buf) {
		printf("FAILED: allocating buf\n");
		free(vblk);
		return -ENOMEM;
	}

	err = nvm_vblock_pread(vblk, buf, addr.g.pg);
	if (getenv("NVM_BUF_PR"))
		nvm_buf_pr(buf, buf_len);
	if (err) {
		printf("FAILED: nvm_vblock_pread err(%ld)\n", err);
	}

	nvm_vblock_free(&vblk);
	free(buf);

	return err;
}

int read(NVM_DEV dev, NVM_GEO geo, NVM_ADDR addr, int flags)
{
	NVM_VBLOCK vblk;
	ssize_t err;

	void *buf;
	int buf_len;

	printf("** nvm_vblock_read(...): ");
	nvm_addr_pr(addr);

	vblk = nvm_vblock_new_on_dev(dev, addr.ppa);
	if (!vblk) {
		printf("FAILED: allocating vblk\n");
		return -ENOMEM;
	}

	buf_len = nvm_dev_attr_vblock_nbytes(dev);
	buf = nvm_buf_alloc(geo, buf_len);
	if (!buf) {
		printf("FAILED: allocating buf\n");
		free(vblk);
		return -ENOMEM;
	}

	err = nvm_vblock_read(vblk, buf);
	if (getenv("NVM_BUF_PR"))
		nvm_buf_pr(buf, buf_len);
	if (err) {
		printf("FAILED: nvm_vblock_read err(%ld)\n", err);
	}

	nvm_vblock_free(&vblk);
	free(buf);

	return err;
}

int pwrite(NVM_DEV dev, NVM_GEO geo, NVM_ADDR addr, int flags)
{
	NVM_VBLOCK vblk;
	ssize_t err;

	char *buf;
	int buf_len;

	printf("** nvm_vblock_pwrite(...): ");
	nvm_addr_pr(addr);

	vblk = nvm_vblock_new_on_dev(dev, addr.ppa);
	if (!vblk) {
		printf("FAILED: allocating vblk\n");
		return -ENOMEM;
	}

	buf_len = nvm_dev_attr_vpage_nbytes(dev);
	buf = nvm_buf_alloc(geo, buf_len);
	if (!buf) {
		printf("FAILED: allocating buf\n");
		free(vblk);
		return -ENOMEM;
	}

	nvm_buf_fill(buf, buf_len);

	err = nvm_vblock_pwrite(vblk, buf, addr.g.pg);
	if (err) {
		printf("FAILED: nvm_vblock_pwrite err(%ld)\n", err);
	}

	nvm_vblock_free(&vblk);
	free(buf);

	return err;
}

int write(NVM_DEV dev, NVM_GEO geo, NVM_ADDR addr, int flags)
{
	NVM_VBLOCK vblk;
	ssize_t err = 0;

	char *buf;
	int buf_len;

	printf("** nvm_vblock_write(...): ");
	nvm_addr_pr(addr);

	vblk = nvm_vblock_new_on_dev(dev, addr.ppa);
	if (!vblk) {
		printf("FAILED: allocating vblk\n");
		return -ENOMEM;
	}

	buf_len = nvm_dev_attr_vblock_nbytes(dev);
	buf = nvm_buf_alloc(geo, buf_len);
	if (!buf) {
		printf("FAILED: allocating buf\n");
		free(vblk);
		return -ENOMEM;
	}

	nvm_buf_fill(buf, buf_len);

	err = nvm_vblock_write(vblk, buf);
	if (err) {
		printf("FAILED: nvm_vblock_write err(%ld)\n", err);
	}

	nvm_vblock_free(&vblk);
	free(buf);

	return err;
}

int erase(NVM_DEV dev, NVM_GEO geo, NVM_ADDR addr, int flags)
{
	NVM_VBLOCK vblk;
	ssize_t err = 0;

	printf("** nvm_vblock_erase(...): ");
	nvm_addr_pr(addr);

	vblk = nvm_vblock_new_on_dev(dev, addr.ppa);
	if (!vblk) {
		printf("FAILED: allocating vblk\n");
		return -ENOMEM;
	}

	err = nvm_vblock_erase(vblk);
	if (err) {
		printf("FAILED: nvm_vblock_erase err(%ld)\n", err);
	}

	nvm_vblock_free(&vblk);

	return err;
}

int mark(NVM_DEV dev, NVM_GEO geo, NVM_ADDR addr, int flags)
{
	ssize_t err;

	printf("** nvm_dev_mark(...): ");
	nvm_addr_pr(addr);

	switch(flags) {
		case 0x0:	// free / good
		case 0x1:	// bad
		case 0x2:	// grown bad
			break;
		default:
			return -EINVAL;
	}

	err = nvm_dev_mark(dev, addr, flags);
	if (err) {
		printf("FAILED: nvm_dev_mark err(%ld)\n", err);
	}

	return err;
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
	int (*func)(NVM_DEV, NVM_GEO, NVM_ADDR, int);
	int argc;
	int flags;
} NVM_CLI_VBLK_CMD;

static NVM_CLI_VBLK_CMD cmds[] = {
	{"get", get, 5, 0},
	{"put", put, 6, 0},
	{"read", read, 6, 0},
	{"write", write, 6, 0},
	{"erase", erase, 6, 0},
	{"pread", pread, 7, 0},
	{"pwrite", pwrite, 7, 0},
	{"mark_f", mark, 6, 0x0},
	{"mark_b", mark, 6, 0x1},
	{"mark_g", mark, 6, 0x2},
};

static int ncmds = sizeof(cmds) / sizeof(cmds[0]);
static char *args[] = {"dev_name", "ch", "lun", "blk", "pg"};

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

	printf("OR using PPA (parts as above are extracted from address):\n");
	for (cmd = 0; cmd < ncmds; cmd++) {
		printf(" %s %6s dev_name ppa\n", cli_name, cmds[cmd].name);
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
	NVM_ADDR addr;

	if (argc < 4) {
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

	for (i = 0; i < ncmds; ++i) {		// Get `cmd`
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

	if ((argc != cmd->argc) && (argc != 4)) {	// Check argument count
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

	addr.ppa = 0;
	switch(argc) {					// Get `addr`

		case 7:					// ch lun blk pg
			addr.g.pg = atoi(argv[6]);
		case 6:					// ch lun blk
			addr.g.blk = atoi(argv[5]);
		case 5:					// ch lun
			addr.g.lun = atoi(argv[4]);
			addr.g.ch = atoi(argv[3]);
			break;

		case 4:					// ppa
			addr.ppa = atol(argv[3]);
			break;

		default:
			_usage_pr(argv[0]);
			return -1;
	}

	dev = nvm_dev_open(dev_name);			// open `dev`
	if (!dev) {
		printf("Failed opening device, dev_name(%s)\n", dev_name);
		return -EINVAL;
	}
	geo = nvm_dev_attr_geo(dev);			// Get `geo`

	if (addr.g.ch >= geo.nchannels) {		// Check `addr`
		printf("ch(%u) too large\n", addr.g.ch);
		return -EINVAL;
	}
	if (addr.g.lun >= geo.nluns) {
		printf("lun(%u) too large\n", addr.g.lun);
		return -EINVAL;
	}
	if (addr.g.blk >= geo.nblocks) {
		printf("blk(%u) too large\n", addr.g.blk);
		return -EINVAL;
	}
	if (addr.g.pg >= geo.npages) {
		printf("pg(%u) too large\n", addr.g.pg);
		return -EINVAL;
	}
	if (addr.g.sec >= geo.nsectors) {
		addr.g.sec = 0;
	}

	ret = cmd->func(dev, geo, addr, cmd->flags);

	nvm_dev_close(dev);				// close `dev`

	return ret;
}
