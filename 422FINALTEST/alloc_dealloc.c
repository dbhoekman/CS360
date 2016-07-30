/*****************************************
CptS 360 Final Project

alloc, dealloc and more

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 12/8/15
*****************************************/
//helper functions for ialloc from lab 6 prelab

//Sets a bit using OR
//Used in ialloc
void set_bit(char *buf, int bit)
{
	int i, j;
	i = bit / 8;
	j = bit % 8;
	buf[i] |= (1 << j);
}

//decrements the amount of free inodes on the device
//This is used to ensure we dont use more inodes than we have room for.
void decFreeInodes(int dev)
{
	char buf[BLKSIZE];

	//dec free inodes count in SUPER and Group Descriptor block
	get_block(dev, 1, buf);
	sp = (SUPER *)buf;
	sp->s_free_inodes_count--;
	put_block(dev, 1, buf);

	get_block(dev, 2, buf);
	gp = (GD *)buf;
	gp->bg_free_inodes_count--;
	put_block(dev, 2, buf);
}

int tst_bit(char *buf, int bit)
{
	int i, j;
	i = bit / 8;
	j = bit % 8;

	if(buf[i] & (1 << j))
		return 1;

	return 0;
}

//end of helper functions

//allocates a free inode number for writing
//taken from lab 6 pre lab
//This is used by any writing functions that require new inodes
int ialloc(int dev)
{
	int i;
	char buf[BLKSIZE];
	printf("imap is %d\n", imap);

	//read inode_bitmap block
	get_block(dev, imap, buf);

	printf("inode count %d\n", ninodes);
	for(i = 0; i < ninodes; i++)
	{
		if(tst_bit(buf, i) == 0)
		{
			set_bit(buf, i);

			decFreeInodes(dev);

			put_block(dev, imap, buf);

			return i + 1;
		}
	}
	printf("ERROR: no more free inodes\n");
	return 0;
}

//allocates a free block so we can put stuff in it
int balloc(int dev)
{
	int i;
	char buf[BLKSIZE];
	printf("bmap is %d\n", bmap);
	//read block_map block
	get_block(dev, bmap, buf);

	for(i = 0; i < nblocks; i++)
	{
		if(tst_bit(buf, i) == 0)
		{
			set_bit(buf, i);

			decFreeInodes(dev);

			put_block(dev, bmap, buf);

			return i;
		}
	}
	printf("ERROR: no more free blocks\n");
	return 0;
}

//deallocates an inode for a given ino on the dev
//This is used when we remove things
//Once dealocated, we increment the free inodes in the SUPER and in the group descriptor
int idealloc(int dev, int ino)
{
	char buf[1024];
	int byte;
	int bit;

	//clear bit(bmap, bno)
	get_block(dev, imap, buf);

	//Mailmans to where it is
	byte = ino / 8;
	bit = ino % 8;

	//Negate it
	buf[byte] &= ~(1 << bit);

	put_block(dev, imap, buf);

	//set free blocks
	get_block(dev, 1, buf);
	sp = (SUPER *)buf;
	sp->s_free_blocks_count++;
	put_block(dev, 1, buf);

	get_block(dev, 2, buf);
	gp = (GD *)buf;
	gp->bg_free_blocks_count++;
	put_block(dev, 2, buf);
}

//deallocate a block
//once deallocated we also increment the number of free blocks
int bdealloc(int dev, int bno)
{
	char buf[1024];
	int byte;
	int bit;

	//clear bit(bmap, bno)
	get_block(dev, bmap, buf);

	byte = bno / 8;
	bit = bno % 8;

	buf[byte] &= ~(1 << bit);

	put_block(dev, bmap, buf);

	//set free blocks
	get_block(dev, 1, buf);
	sp = (SUPER *)buf;
	sp->s_free_blocks_count++;
	put_block(dev, 1, buf);

	get_block(dev, 2, buf);
	gp = (GD *)buf;
	gp->bg_free_blocks_count++;
	put_block(dev, 2, buf);

	return 0;
}
