#ifndef _AR_H
#define _AR_H

/*! \brief String that begins an archive file.  */
#define ARMAG	"!<arch>\n"
/*! \brief Size of ARMAG.  */
#define SARMAG	8
/*! \brief String in ar_fmag at end of each header.  */
#define ARFMAG	"`\n"
#define AR_HEADER_SIZE 60

/*! \brief ar header info */
struct ar_hdr_ {
	char 			*ar_name;	/*!< \brief File name in the archive */
	double 			ar_date;	/*!< \brief File date unix Epoch format */
	unsigned int	ar_uid;		/*!< \brief User id */
	unsigned int	ar_gid;		/*!< \brief Group id */
	unsigned int	ar_mode;	/*!< \brief File mode in octal */
	ssize_t			ar_size;	/*!< \brief File size */
	off_t			offset;		/*!< \brief Offset in the main archive */
	struct			ar_hdr_ *next;	/*!< \brief next header */
};

typedef struct ar_object ar_object;

extern struct ar_hdr_ *ar_search(ar_object *obj, const char *name);
extern void ar_extract_all(ar_object *obj, const char *dest);
extern ar_object *ar_open(const char *file);
extern int ar_close(ar_object *obj);
extern int ar_grab(ar_object *obj, const char *fname,
	void (*extern_func)(char *, int));

#endif /* _AR_H */
