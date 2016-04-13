#ifndef MPKG_ERR_H
#define MPKG_ERR_H

#ifdef DEBUG___
 #define mpkg_err_warn(Type, X) mpkg_err(__FILE__, __func__, __LINE__, Type, X)
#else
 #define mpkg_err_warn(Type, X) mpkg_err(Type, X)
#endif

typedef enum error_type {
	mpkg_err_warning,
	mpkg_err_error
} error_type;

#ifdef DEBUG___
extern void mpkg_err(char *section, const char *func,
	int line_num, error_type er_type, const char *err_str);
#else
extern void mpkg_err(error_type er_type, const char *err_str);
#endif

#endif /* MPKG_ERR_H */
