/*****************************************
CptS 360 Final Project

makedir

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 12/8/15
*****************************************/


void make_dir(char path[124])
{
	int i, ino;
	MINODE *pmip;
	INODE *pip;

	char buf[1024];
	char temp1[1024], temp2[1024];
	char parent_name[1024], child_name[1024];

	//copy path so we don't destroy it
	strcpy(temp1, path);
	strcpy(temp2, path);

	//get parent and child name
	strcpy(parent_name, dirname(temp1));
	strcpy(child_name, basename(temp2));
	//printf("parent: %s\nchild: %s\n", parent_name, child_name);

	//get parent's ino
	ino = getino(running->cwd, parent_name);
	printf("ino is %d\n", ino);
	pmip = iget(dev, ino);
	pip = &pmip->INODE;

	//check if parent exists
	if(!pmip)
	{
		printf("ERROR: parent does not exist\n");
		return;
	}

	if(pmip == root)
		printf("in root\n");

	//check if dir
	if(!S_ISDIR(pip->i_mode))
	{
		printf("ERROR: parent is not directory\n");
		return;
	}

	//check if dir already exists
	if(getino(running->cwd, path) != 0)
	{
		printf("ERROR: %s already exists\n", path);
		return;
	}

	printf("running my_mkdir\n");
	//Call my_mkdir to make the dir
	my_mkdir(pmip, child_name);

	//increment the parents link count and adjust the time
	pip->i_links_count++;
	pip->i_atime = time(0L);
	pmip->dirty = 1;
	//set dirty to true and iput

	iput(pmip);

	return;
}

//puts the name into the parent directory
int enter_name(MINODE *mip, int myino, char *myname)
{
	int i;
	INODE *parent_ip = &mip->INODE;

	char buf[1024];
	char *cp;
	DIR *dp;

	int need_len = 0, ideal = 0, remain = 0;
	int bno = 0, block_size = 1024;

	for(i = 0; i < parent_ip->i_size / BLKSIZE; i++)
	{
		if(parent_ip->i_block[i] == 0)
			break;

		bno = parent_ip->i_block[i];

		get_block(dev, bno, buf);

		dp = (DIR*)buf;
		cp = buf;

		//need length
		need_len = 4 * ( (8 + strlen(myname) + 3) / 4);
		printf("need len is %d\n", need_len);

		//step into last dir entry
		while(cp + dp->rec_len < buf + BLKSIZE)
		{
			cp += dp->rec_len;
			dp = (DIR*)cp;
		}

		printf("last entry is %s\n", dp->name);
		cp = (char*)dp;

		//ideal length uses name len of last dir entry
		ideal = 4 * ( (8 + dp->name_len + 3) / 4);

		remain = dp->rec_len - ideal;
		printf("remain is %d\n", remain);

		if(remain >= need_len)
		{
			//set rec_len to ideal
			dp->rec_len = ideal;

			cp += dp->rec_len;
			dp = (DIR*)cp;

			//sets the dirpointer inode to the given myino
			dp->inode = myino;
			dp->rec_len = block_size - ((u32)cp - (u32)buf);
			printf("rec len is %d\n", dp->rec_len);
			dp->name_len = strlen(myname);
			dp->file_type = EXT2_FT_DIR;
			//sets the dp name to the given name
			strcpy(dp->name, myname);

			//puts the block
			put_block(dev, bno, buf);

			return 1;
		}
	}

	printf("number is %d\n", i);

	//no space in existing data blocks, time to allocate in next block
	bno = balloc(dev);
	parent_ip->i_block[i] = bno;

	parent_ip->i_size += BLKSIZE;
	mip->dirty = 1;

	get_block(dev, bno, buf);

	dp = (DIR*)buf;
	cp = buf;

	printf("dir name is %s\n", dp->name);

	dp->inode = myino;
	dp->rec_len = 1024;
	dp->name_len = strlen(myname);
	dp->file_type = EXT2_FT_DIR;
	strcpy(dp->name, myname);

	put_block(dev, bno, buf);

	return 1;
}


void my_mkdir(MINODE *pmip, char *child_name)
{
	int ino = ialloc(dev);
	int bno = balloc(dev);
	int i;

	printf("device is %d\n", dev);
	printf("ino is %d\nbno is %d\n", ino, bno);

	MINODE *mip = iget(dev, ino);
	INODE *ip = &mip->INODE;

	char *cp, buf[1024];
	DIR *dp;

	ip->i_mode = 0x41ED; // OR 040755: DIR type and permissions
	//printf("mode is %d\n", ip->i_mode);
	ip->i_uid  = running->uid; // Owner uid
	//printf("uid is %d\n", ip->i_uid);
	ip->i_gid  = running->gid; // Group Id
	//printf("gid is %d\n", ip->i_gid);
	//We set the size to blksize to because that is the size of a dir
	ip->i_size = BLKSIZE;// Size in bytes
	ip->i_links_count = 2;// Links count=2 because of . and ..
	ip->i_atime = time(0L);// set access time to current time
	ip->i_ctime = time(0L);// set creation time to current time
	ip->i_mtime = time(0L);// set modify time to current time

	//This is for . and ..
	ip->i_blocks = 2;                	// LINUX: Blocks count in 512-byte chunks
	ip->i_block[0] = bno;             // new DIR has one data block

	//set all blocks to 0
	for(i = 1; i < 15; i++)
	{
		ip->i_block[i] = 0;
	}

	mip->dirty = 1;//set dirty to true and iput
	iput(mip);

	//create data block for new DIR containing . and ..
	get_block(dev, bno, buf);

	dp = (DIR*)buf;
	cp = buf;

	dp->inode = ino;
	dp->rec_len = 4 * (( 8 + 1 + 3) / 4);
	dp->name_len = strlen(".");
	dp->file_type = (u8)EXT2_FT_DIR;
	dp->name[0] = '.';

	cp += dp->rec_len;
	dp = (DIR*)cp;

	//This portion is for the parent (..)
	dp->inode = pmip->ino;
	dp->rec_len = 1012;//will always be 12 in this case
	printf("rec_len is %d\n", dp->rec_len);
	dp->name_len = strlen("..");
	dp->file_type = (u8)EXT2_FT_DIR;
	dp->name[0] = '.';
	dp->name[1] = '.';

	//write buf to disk block bno
	put_block(dev, bno, buf);

	//enter name entry into parent's directory
	enter_name(pmip, ino, child_name);

	return 1;
}
