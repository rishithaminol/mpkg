/* all the features in this file is used by copy.c:copy_hlink() */

/* ATTENTION! 
 * all the functions are compatible only with this structure
 */
struct in_list;

struct in_list *in_new_segment(int old_inode, const char *y);
void in_append(int old_inode, const char *y, struct in_list *z);
char *in_search(int old_inode, struct in_list *z);
void in_free(struct in_list *z);
