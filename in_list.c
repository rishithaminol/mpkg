/* This program handle an inode table for the purpose of
 * copying hard links all functions are prefixed with in_
 * all the features in this file is used by
 * copy.c:copy_hlink() only
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "in_list.h"

struct in_list {
	int old_ino;
	char *hlink;
	struct in_list *next;
};

static struct in_list *in_new_segment_allocate(void)
{
	return (struct in_list *)malloc(sizeof(struct in_list));
}

struct in_list *in_new_segment(int old_inode, const char *y)
{
	struct in_list *tmp = in_new_segment_allocate();

	tmp->old_ino = old_inode;
	tmp->hlink = strdup(y);
	tmp->next = NULL;

	return tmp;
}

void in_append(int old_inode, const char *y, struct in_list *z)
{
	struct in_list *i = z;

	while (i->next != NULL)
		i = i->next;

	/* link new segment at the end of the linked list */
	i->next = in_new_segment(old_inode, y);
}

/* find the hlink for the given old_inode number */
char *in_search(int old_inode, struct in_list *z)
{
	struct in_list *i = z;

	while (1) {
		if (i->old_ino == old_inode)
			return i->hlink;
		if (i->next == NULL)
			return NULL;	/* no inode found */

		i = i->next;
	}

	return NULL;	/* no inode found */
}

/* free in_list structure chain */
void in_free(struct in_list *z)
{
	struct in_list *i = z;
	struct in_list *tmp;

	while (1) {
		tmp = i->next;
		free(i->hlink);
		free(i);
		if (tmp == NULL)
			break;
		i = tmp;
	}
}
