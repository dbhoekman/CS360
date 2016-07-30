/*****************************************
CptS 360 Final Project

Utility Functions

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 12/8/15
*****************************************/

#include <libgen.h>

//get_block() takes in file descriptor, block and buf
//reads a block into buffer
void get_block(int fd, int blk, char buf[BLKSIZE])
{
	lseek(fd, (long)(blk*BLKSIZE), 0);
	read(fd, buf, BLKSIZE);
}

//put_block() takes in file descriptor, block and buf
//writes a block back
void put_block(int fd, int blk, char buf[BLKSIZE])
{
	lseek(fd, (long)(blk*BLKSIZE), 0);
	write(fd, buf, BLKSIZE);
}

//loads inode into slot in minode[] array
MINODE *iget(int dev, int ino)
{
	int ipos = 0;
	int i = 0;
	int offset = 0;
	char buf[1024];
	INODE *ip = NULL;
	MINODE *mip = malloc(sizeof(MINODE));

	//First we check to see if we have a minode for the given ino already in use
	//If we do we will just increment the refCount and use that minode
	for (i = 0; i < NMINODE; i++)
	{
		mip = &minode[i];

		if(mip->dev == dev && mip->ino == ino)
		{
			mip->refCount++;
			//printf("minode[%d]->refCount incremented\n", i);
      return mip;
		}
	}

	//mailman's
	ipos = (ino - 1)/8 + INODE_START_POS;
	offset = (ino - 1) % 8;

	//get the block
	get_block(dev, ipos, buf);
	//load inode
	ip = (INODE *)buf + offset;

	for (i = 0; i < NMINODE; i++)
	{
		mip = &minode[i];
		//printf("minode[%d].refCount = %d\n", i, minode[i].refCount);

		if (mip->refCount == 0)
		{
			//printf("using minode[%d]\n", i);
			mip->INODE = *ip;
			mip->dev = dev;
			mip->ino = ino;
			mip->refCount++;

			return mip;
		}
	}
}

//tokenize pathname and put parts into name[64][64]
void tokenize(char pathname[256])
{
	char path_copy[256] = "";
	char *temp;
	int i = 0;

	for(i = 0; i < 64; i++)
	{
		strcpy(names[i], "");
	}

	i = 0;

	//copy pathname so we don't destroy the original pathname
	strcpy(path_copy, pathname);
	strcat(path_copy, "/");
	temp = strtok(path_copy, "/");

	//populate names
	while(temp != NULL)
	{
		strcpy(names[i], temp);
		temp = strtok(NULL, "/");
		i++;
	}
}

//returns the ino for a given pathname
int getino(MINODE *mip, char pathname[64])
{
	int ino = 0, i = 0, n = 0;
	int inumber, offset;
	char path[64];
	char name[64][64];
	char *temp;
	char buf[1024];

	//check if it is root
	if(!strcmp(pathname, "/"))
		return 2;//return root ino

	//check if starts at root
	if(pathname[0] == '/')
	{
		mip = root;
	}

	//if there's a pathname, then parse it
	if(pathname)
	{
		//parse the string and put it into name
		strcat(pathname, "/");
		temp = strtok(pathname, "/");

		while(temp != NULL)
		{
			strcpy(name[i], temp);
			temp = strtok(NULL, "/");
			i++;
			n++;
		}
		//parsing complete
	}

	//time to do the searching
	for(i = 0; i < n; i++)
	{
		//printf("inode we are in = %d\n", mip->ino);
		//printf("now searching for %s\n", name[i]);
		ino = search(mip, name[i]);

		if(ino == 0)
		{
			//can't find name[i]
			return 0;
		}

		//printf("found %s at inode %d\n", name[i], ino);

		mip = iget(dev, ino);
	}

	return ino;
}

//Search() takes in an minode and searches its data blocks for a name
//if found, returns name's ino
//returns 0 on failure
int search(MINODE *mip, char *name)
{
	//search for name in the data blocks of this INODE
	//if found, return name's ino;
	//return 0

	int i;
	char buf[BLKSIZE], *cp;
	char dir_name[64];
	DIR *dp;

	//make sure it's a directory before we do anything
	if(!S_ISDIR(mip->INODE.i_mode))
	{
		printf("ERROR: not a directory\n");
		return 0;
	}

	//search through the direct blocks
	for(i = 0; i < 12; i++)
	{
		//if the data block has stuff in it
		if(mip->INODE.i_block[i])
		{
			//get the block
			get_block(dev, mip->INODE.i_block[i], buf);

			dp = (DIR *)buf;
			cp = buf;

			while(cp < buf + BLKSIZE)
			{
				//null terminate dp->name for strcmp
				if(dp->name_len < 64)
				{
					strncpy(dir_name, dp->name, dp->name_len);
					dir_name[dp->name_len] = 0;
				}
				else
				{
					strncpy(dir_name, dp->name, 64);
					dir_name[63] = 0;
				}

				//printf("current dir: %s\n", dir_name);
				//check if it's the name we're looking for
				if(strcmp(name, dir_name) == 0)
					return dp->inode;//name matches, return the inode
				cp += dp->rec_len;
				dp = (DIR *)cp;
			}
		}
	}
	//name does not exist, print error message
	printf("name %s does not exist\n", name);
	strcpy(teststr, "name ");
	strcat(teststr, name);
	strcat(teststr, " does not exist");
	return 0;
}

//releases a minode[]
void iput(MINODE *mip)
{
	int ino = 0;
	int offset, ipos;
	char buf[1024];

	ino = mip->ino;

	//decrement refCount by 1
	mip->refCount--;

	//check refcount to see if anyone using it
	//check dirty to see if it's been changed, dirty == 1 if changed
	//if refCount > 0 or dirty return
	if (mip->refCount == 0 && mip->dirty == 0)
	{
		return;
	}

	//mail man's to determine disk block and which inode in that block
	ipos = (ino - 1) / 8 + INODE_START_POS;
	offset = (ino -1) % 8;

	//read that block in
	get_block(mip->dev, ipos, buf);

	//copy minode's inode into the inode area in that block
	ip = (INODE*)buf + offset;
	*ip = mip->INODE;

	//write block back to disk
	put_block(mip->dev, ipos, buf);
	mip->dirty = 0;
}

int findmyname(MINODE *parent, int myino, char *myname)
{
	/*
	Given the parent DIR (MINODE pointer) and myino, this function finds
	the name string of myino in the parent's data block. This is the SAME
	as SEARCH() by myino, then copy its name string into myname[ ].

	It does this using the dir pointer to get the name
	*/
	int i;
	INODE *ip;
	char buf[BLKSIZE];
	char *cp;
	DIR *dp;

	if(myino == root->ino)
	{
		strcpy(myname, "/");
		return 0;
	}

	if(!parent)
	{
		printf("ERROR: no parent\n");
		return 1;
	}

	ip = &parent->INODE;

	if(!S_ISDIR(ip->i_mode))
	{
		printf("ERROR: not directory\n");
		return 1;
	}

	for(i = 0; i < 12; i++)
	{
		if(ip->i_block[i])
		{
			get_block(dev, ip->i_block[i], buf);
			dp = (DIR*)buf;
			cp = buf;

			while(cp < buf + BLKSIZE)
			{
				if(dp->inode == myino)
				{
					strncpy(myname, dp->name, dp->name_len);
					myname[dp->name_len] = 0;
					return 0;
				}
				else
				{
					cp += dp->rec_len;
					dp = (DIR*)cp;
				}
			}
		}
	}
	return 1;
}

int findino(MINODE *mip, int *myino, int *parentino)
{
	/*
	For a DIR Minode, extract the inumbers of . and ..
	Read in 0th data block. The inumbers are in the first two dir entries.
	*/
	INODE *ip;
	char buf[1024];
	char *cp;
	DIR *dp;

	//check if exists
	if(!mip)
	{
		printf("ERROR: ino does not exist!\n");
		return 1;
	}

	//point ip to minode's inode
	ip = &mip->INODE;

	//check if directory
	if(!S_ISDIR(ip->i_mode))
	{
		printf("ERROR: ino not a directory\n");
		return 1;
	}

	get_block(dev, ip->i_block[0], buf);
	dp = (DIR*)buf;
	cp = buf;

	//.
	*myino = dp->inode;

	cp += dp->rec_len;
	dp = (DIR*)cp;

	//..
	*parentino = dp->inode;

	return 0;
}
