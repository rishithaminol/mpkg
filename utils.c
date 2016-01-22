#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

/*! @brief check the exsistance of a file.
 *  @return return 0 on success. Otherwise return
 *			\b errno occured.
 */
int file_exist(const char *filename)
{
	struct stat tmp_stat;
	int ret;

	if ((ret = stat(filename, &tmp_stat)) == -1)
		return errno;

	return ret;
}
