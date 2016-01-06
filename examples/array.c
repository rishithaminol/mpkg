#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct inode_list {
	int old_ino;
	char *hlink;
	struct inode_list *next;
};

struct inode_list *list_allocate(void)
{
	return (struct inode_list *)malloc(sizeof(struct inode_list));
}

struct inode_list *new_list(int x, char *y)
{
	struct inode_list *tmp = list_allocate();

	tmp->old_ino = x;
	tmp->hlink = strdup(y);
	tmp->next = NULL;

	return tmp;
}

void append_list(int x, char *y, struct inode_list *z)
{
	struct inode_list *i = z;

	while (i->next != NULL)
		i = i->next;

	i->next = new_list(x, y);
}

int main(int argc, char *argv[])
{
	struct inode_list *list = new_list(32, "hello");
	append_list(44, "minol", list);
	printf("%d %s\n", list->old_ino, list->hlink);
	printf("%d %s\n", list->next->old_ino, list->next->hlink);
	return 0;
}
