#ifndef MPKG_ERR_H
#define MPKG_ERR_H

#ifdef DEBUG___
 #define mpkg_err_warn(X) mpkg_err(__FILE__, __func__, __LINE__, X)
#else
 #define mpkg_err_warn(X) mpkg_err(X)
#endif

#ifdef DEBUG___
extern void mpkg_err(char *section, const char *func, int line_num, const char *err_str);
#else
extern void mpkg_err(const char *err_str);
#endif

#endif /* MPKG_ERR_H */
