/*****************************************
CptS 360 Final Project

unlink

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 11/23/15
*****************************************/

//Unlinks files, aka rm and decrements the refcount between all linked files
void my_unlink(char *path)
{
	int ino, i;
	int parent_ino;
	MINODE *mip;
	MINODE *parent_mip;
	INODE *ip;
	INODE *parent_ip;

	char temp[64];
	char my_dirname[64];
	char my_basename[64];

	//Checks
	if(!path)
	{
		printf("ERROR: no path given\n");
		return;
	}

	//Gets the ino and checks to ensure it exists
	ino = getino(running->cwd, path);

	if(ino == 0)
	{
		printf("ERROR: bad path\n");
		return;
	}

	//Get the minode and check to make sure that it is a file
	mip = iget(dev, ino);

	if(!mip)
	{
		printf("ERROR: missing minode\n");
		return;
	}

	//Make sure its a file
	if(S_ISDIR(mip->INODE.i_mode))
	{
		printf("ERROR: can't unlink a directory\n");
		return;
	}

	printf("doing the unlinking\n");
	ip = &mip->INODE;

	//Decrement link count
	ip->i_links_count--;
	printf("links: %d\n", ip->i_links_count);

	//Deallocate its blocks
	for(i = 0; i < 12 && ip->i_block[i] != 0; i++)
	{
		bdealloc(dev, ip->i_block[i]);
	}

	//deallocate its inode
	idealloc(dev, ino);

	strcpy(temp, path);
	strcpy(my_dirname, dirname(temp));

	strcpy(temp, path);
	strcpy(my_basename, basename(temp));

	printf("dirname is %s basename is %s\n", my_dirname, my_basename);

	//Gets the parent and removes the file from its parent
	parent_ino = getino(running->cwd, my_dirname);
	parent_mip = iget(dev, parent_ino);
	parent_ip = &parent_mip->INODE;

	//Removes the child from the parent
	printf("removing %s from %s\n", my_basename, my_dirname);
	rm_child(parent_mip, my_basename);

	//Update the time, set dirty, and iput
	parent_ip->i_atime = time(0L);
	parent_ip->i_mtime = time(0L);
	parent_mip->dirty = 1;

	iput(parent_mip);
	mip->dirty = 1;
	iput(mip);

	return;
}
