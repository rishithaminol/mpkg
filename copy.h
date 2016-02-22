#ifndef COPY_H
#define COPY_H

extern void copy(const char *src, const char *dest,
	void *clb_opt, void (*callback)(void *clb_opt, char *str));
extern void remove_tmpdir(const char *f);

#endif /* COPY_H */