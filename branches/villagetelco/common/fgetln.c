/*
 * fgetln() for linux.   Hacked up for sed, et al.
 *
 * BSD tweaks around with the inside of a FILE*; we can't do that
 * in the land of GNU libc, so we keep a private array of FILE*
 * keys and pointer/size pairs to hold the lines.
 *
 * Courtesy of: 
 *   http://mastodon.biz/~orc/Code/bsd/bsd-1.0/00library/fgetln.c
 */
#include <stdio.h>
#include <sys/types.h>
#include <malloc.h>
#include <errno.h>

#define MAXFILES	20

typedef struct {
    FILE *key;		/* file descriptor this bfr is associated with */
    char *bfr;		/* buffer for latest fgetln()ed line */
    int buflen;		/* length of the buffer */
} LINEREC;

static LINEREC tbl[MAXFILES] = { 0 };

char*
fgetln(FILE *fd, size_t *len)
{
    int idx;
    int c, lnsize;
    int newest;

    /* we want to do fgetln, eh?  Check to see if all the variables
     * are okay first (don't want to waste space if the file is bad
     * or the len argument is bogus (==zero)
     */
    if (fd == 0 || len == 0) {
	errno = EINVAL;
	return 0;
    }

    /* find an available slot in the table
     */
    for (idx = 0; idx < MAXFILES; idx++)
	if (tbl[idx].key == fd)
	    break;

    if (idx == MAXFILES) {
	for (idx=0; idx < MAXFILES; idx++)
	    if (tbl[idx].key == 0) {
		/* usable file slot */
		break;
	    }

	if (idx == MAXFILES) {
	    errno = ENFILE;
	    return 0;
	}
    }

    /* Initialize the buffer, if needed
     */
    if (tbl[idx].bfr == 0) {
	tbl[idx].bfr = malloc(200);
	if (tbl[idx].bfr == 0)
	    return 0;
	tbl[idx].buflen = 200;
    }
    tbl[idx].key = fd;

    /* we've got a buffer, so lets start writing a line into
     * it.
     */
    for (lnsize = 0; (c = getc(fd)) != EOF; ) {
	tbl[idx].bfr[lnsize++] = c;
	if (lnsize == tbl[idx].buflen) {
	    tbl[idx].bfr = realloc(tbl[idx].bfr, tbl[idx].buflen * 2);
	    if (tbl[idx].bfr == 0)
		return 0;
	    tbl[idx].buflen *= 2;
	}
	if (c == '\n')
	    break;
    }

    /* save out the size, then return to sender
     */
    *len = lnsize;
    return lnsize ? tbl[idx].bfr : 0;
} /* fgetln */
