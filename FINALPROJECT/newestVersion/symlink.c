/*****************************************
CptS 360 Final Project

symlink

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 11/23/15
*****************************************/

//Symlink links by name
//This could even link to places that dont exist
//Symlink doesnt care
void my_symlink(char *path)
{
	int ino, i;
	int link_ino;
	int parent_ino;
	char temp[64], parent[64], child[64];
	char old_name[64];

	MINODE *mip;
	MINODE *parent_mip;
	MINODE *link_mip;

	INODE *ip;
	INODE *parent_ip;
	INODE *link_ip;

	strcpy(temp, path);
	strcpy(old_name, basename(temp));

	//get inode of old file
	ino = getino(running->cwd, path);
	mip = iget(dev, ino);

	if(strlen(path) >= 60)
	{
		printf("ERROR: name too long\n");
		return;
	}

	if(!mip)
	{
		printf("ERROR: %s does not exist\n", path);
		return;
	}

	//get parent and child of old file pathname
	strcpy(temp, third);
	strcpy(parent, dirname(temp));
	strcpy(child, basename(third));

	printf("parent is %s,  child is %s\n", parent, child);

	parent_ino = getino(running->cwd, parent);
	parent_mip = iget(dev, parent_ino);

	if(!parent_mip)
	{
		printf("ERROR: can't get parent mip\n");
		return;
	}

	if(!S_ISDIR(parent_mip->INODE.i_mode))
	{
		printf("ERROR: parent is not a directory\n");
		return;
	}

	if(getino(running->cwd, child) > 0)
	{
		printf("ERROR: %s already exists\n", child);
		return;
	}

	link_ino = my_creat(parent_mip, child);
	//gets the newfile minode to set it's variables
	link_mip = iget(dev, link_ino);
	link_ip = &link_mip->INODE;

	//set the link mode
	link_ip->i_mode = 0120000;
	//set the link size, which is the size of the oldfiles name
	link_ip->i_size = strlen(old_name);

	link_mip->dirty = 1;
	iput(link_mip);
	iput(mip);


}
