#define ARMAG	"!<arch>\n"	/* String that begins an archive file.  */
#define SARMAG	8		/* Size of that string.  */

/* ar header info */
struct ar_hdr_ {
	char 	*ar_name;	/* Member file name. / terminated */
	time_t 	ar_date;	/* File date unix Epoch format */
	uid_t	ar_uid;		/* User id */
	gid_t	ar_gid;		/* Group id */
	mode_t 	ar_mode;	/* File mode in octal */
	off_t 	ar_size;	/* File size */
	off_t	offset;		/* Offset in the main archive */
	struct ar_hdr_ *next;	/* next header */
};

/*
	ar_headers() - After the ar_test(int fd) return all the headers
							in ar_hdr_ format
	ar_extract() - Extract the given file to the destination
	ar_free() - free up ar_hdr_ chain
	ar_open() - return file descriptor if file is an ar archive.
				if it is not an archive return -1.
	ar_close() - close opened ar file descriptor

	see doc/ar_info for more infomation.
*/

extern struct ar_hdr_ *ar_headers(int fd);
extern int ar_extract(int fd, struct ar_hdr_ *t, const char *dest);
extern int ar_free(struct ar_hdr_ *z);
extern int ar_open(const char *file);
extern void ar_close(int fd);
extern void ar_extract_all(int fd, struct ar_hdr *ar_hdrs, const char *dest);
