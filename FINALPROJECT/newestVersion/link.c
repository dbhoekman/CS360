/*****************************************
CptS 360 Final Project

Main

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 12/3/15
*****************************************/

//link oldfilename newfilename
//This makes newfile linked to oldfile
//This makes them have the same ino
void my_link(char *path)
{
	char old[64], new[64], temp[64];
	char link_parent[64], link_child[64];
	int ino;
	int p_ino;
	MINODE *mip;
	MINODE *p_mip;
	INODE *ip;
	INODE *p_ip;

	//Checks
	if(!strcmp(path, ""))
	{
		printf("ERROR: no old file\n");
		return;
	}

	if(!strcmp(third, ""))
	{
		printf("ERROR: no new file\n");
		return;
	}

	strcpy(old, path);
	strcpy(new, third);

	//get oldfilename's inode
	ino = getino(running->cwd, old);
	mip = iget(dev, ino);

	//verify old file exists
	if(!mip)
	{
		printf("ERROR: %s does not exist\n", old);
		return;
	}
	//Verify it is a file
	if(S_ISDIR(mip->INODE.i_mode))
	{
		printf("ERROR: can't link directory\n");
		return;
	}
	printf("got it!\n");

	//get new's dirname
	if(!strcmp(new, "/"))
	{
		strcpy(link_parent, "/");
	}
	else
	{
		strcpy(temp, new);
		strcpy(link_parent, dirname(temp));
	}

	//get new's basename
	strcpy(temp, new);
	strcpy(link_child, basename(temp));

	printf("after stuff new is %s\n", new);

	//get new's parent
	printf("parent %s\n", link_parent);
	p_ino = getino(running->cwd, link_parent);
	p_mip = iget(dev, p_ino);

	//verify that link parent exists
	if(!p_mip)
	{
		printf("ERROR: no parent\n");
		return;
	}

	//verify link parent is a directory
	if(!S_ISDIR(p_mip->INODE.i_mode))
	{
		printf("ERROR: not a directory\n");
		return;
	}

	//verify that link child does not exist yet
	if(getino(running->cwd, new))
	{
		printf("ERROR: %s already exists\n", new);
		return;
	}

	//Enter the name for the newfile into the parent dir
	printf("entering name for %s\n", link_child);
	//This ino is the ino of the old file
	//This is how it is linked
	enter_name(p_mip, ino, link_child);

	ip = &mip->INODE;

	//increment the link count
	ip->i_links_count++;
	mip->dirty = 1;

	p_ip = &p_mip->INODE;

	p_ip->i_atime = time(0L);
	//Set dirty and iput
	p_mip->dirty = 1;

	iput(p_mip);

	iput(mip);

	printf("end of link\n");
	return;
}
