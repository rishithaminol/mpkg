char *get_current_working_dir(void)
{
	return getcwd(NULL, 0);
}
