/*****************************************
CptS 360 Final Project

stat

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 11/23/15
*****************************************/

//Prints stats, simple as that.
void my_stat(char *path)
{
	int ino;
	MINODE *mip;
	INODE *ip;
	char *cp;
	char buf[1024];
	char name[64];
	char *my_atime;
	char *my_mtime;
	char *my_ctime;
	DIR *dp;

	ino = getino(running->cwd, path);
	mip = iget(dev, ino);

	strcpy(name, basename(path));

	ip = &mip->INODE;

	printf("  File: %s\n", name);
	printf("  Size: %d\tBlocks: %12d ", ip->i_size, ip->i_blocks);
	if(S_ISDIR(ip->i_mode))
		printf("  Directory\n");
	else
		printf("  File\n");
	printf("Inode: %d Links:%d \n", ino, ip->i_links_count);

	my_atime = ctime( (time_t*)&ip->i_atime);
	my_mtime = ctime( (time_t*)&ip->i_mtime);
	my_ctime = ctime( (time_t*)&ip->i_ctime);

	printf("Access: %26s", my_atime);
	printf("Modify: %26s", my_mtime);
	printf("Change: %26s", my_ctime);

	mip->dirty = 1;
	iput(mip);

	return;
}
