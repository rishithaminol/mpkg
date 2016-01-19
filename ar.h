#ifndef _AR_H
#define _AR_H

/*! String that begins an archive file.  */
#define ARMAG	"!<arch>\n"
/*! Size of ARMAG.  */
#define SARMAG	8
/*! String in ar_fmag at end of each header.  */
#define ARFMAG	"`\n"
#define AR_HEADER_SIZE 60

/*! ar header info */
struct ar_hdr_ {
	char 			*ar_name;	/*!< Member file name. / terminated */
	double 			ar_date;	/*!< File date unix Epoch format */
	unsigned int	ar_uid;		/*!< User id */
	unsigned int	ar_gid;		/*!< Group id */
	unsigned int	ar_mode;	/*!< File mode in octal */
	ssize_t			ar_size;	/*!< File size */
	off_t			offset;		/*!< Offset in the main archive */
	struct			ar_hdr_ *next;	/*!< next header */
};

typedef struct ar_object ar_object;

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
extern struct ar_hdr_ *ar_search(ar_object *obj, const char *name);
extern int ar_extract(int fd, struct ar_hdr_ *hdr, const char *dest);
extern void ar_extract_all(ar_object *obj, const char *dest);
extern ar_object *ar_open(const char *file);
extern int ar_close(ar_object *obj);
extern int ar_grab(ar_object *obj, const char *fname,
	void (*extern_func)(char *, int));

#endif /* _AR_H */
