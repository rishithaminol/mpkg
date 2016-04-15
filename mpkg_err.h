#ifndef MPKG_ERR_H
#define MPKG_ERR_H

#ifdef DEBUG___
 #define mpkg_err(ER_FRMT, ...) mpkg_err1(__FILE__, __func__, __LINE__, ER_FRMT, ##__VA_ARGS__)
 #define mpkg_warn(ER_FRMT, ...) mpkg_err2(__FILE__, __func__, __LINE__, ER_FRMT, ##__VA_ARGS__)
#else
 #define mpkg_err(ER_FRMT, ...) mpkg_err1(ER_FRMT, ##__VA_ARGS__)
 #define mpkg_warn(ER_FRMT, ...) mpkg_err2(ER_FRMT, ##__VA_ARGS__)
#endif


#ifdef DEBUG___
extern void mpkg_err1(char *section, const char *func, int line_num,
	const char *err_str, ...);
extern void mpkg_err2(char *section, const char *func,	int line_num,
	const char *warn_str, ...);
#else
extern void mpkg_err1(const char *err_str, ...);
extern void mpkg_err2(const char *warn_str, ...);
#endif

#endif /* MPKG_ERR_H */
