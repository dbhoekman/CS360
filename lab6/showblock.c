#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ext2fs/ext2_fs.h>

typedef unsigned int   u32;

//Define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;//Need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp;

#define BLKSIZE 1024
#define BLOCK_OFFSET(block) (1024 + (block - 1) * 1024)

char buf[BLKSIZE];
int fd;
int imap, bmap;
int ninodes, nblocks, nfreeInodes, nfreeBlocks;
char *device;
int iblock;

//Step 0
void get_block(int fd, int blk, char buf[BLKSIZE])
{
	lseek(fd, (long)(blk*BLKSIZE), 0);
	read(fd, buf, BLKSIZE);
}

static void get_inode(int fd, int ino, INODE *inode)
{
	lseek(fd, BLOCK_OFFSET(iblock) + (ino - 1) * sizeof(INODE), SEEK_SET);
	read(fd, inode, sizeof(INODE));
}


//Taken from prelab
super()
{
  // Read SUPER block
  get_block(fd, 1, buf);
  sp = (SUPER *)buf;

  // Check for EXT2 magic number:
  // Lets us know if it is an EXT2FS
  printf("s_magic = \t\t\t\t%x\n", sp->s_magic);
  if (sp->s_magic != 0xEF53){
    printf("NOT an EXT2 FS\n");
    exit(1);
  }
  printf ("-------- info --------\n");
  printf("EXT2 FS OK\n");

  printf("s_inodes_count = \t\t\t%d\n", sp->s_inodes_count);
  printf("s_blocks_count = \t\t\t%d\n", sp->s_blocks_count);

  printf("s_free_inodes_count = \t\t\t%d\n", sp->s_free_inodes_count);
  printf("s_free_blocks_count = \t\t\t%d\n", sp->s_free_blocks_count);
  printf("s_first_data_blcok = \t\t\t%d\n", sp->s_first_data_block);


  printf("s_log_block_size = \t\t\t%d\n", sp->s_log_block_size);

  printf("s_blocks_per_group = \t\t\t%d\n", sp->s_blocks_per_group);
  printf("s_inodes_per_group = \t\t\t%d\n", sp->s_inodes_per_group);


  printf("s_mnt_count = \t\t\t\t%d\n", sp->s_mnt_count);
  printf("s_max_mnt_count = \t\t\t%d\n", sp->s_max_mnt_count);

  printf("s_magic = \t\t\t\t%x\n", sp->s_magic);

  printf("s_mtime = \t\t\t\t%s", ctime(&sp->s_mtime));
  printf("s_wtime = \t\t\t\t%s", ctime(&sp->s_wtime));
}

int dirSearch(char name[1024][1024], int ind, INODE *inoptr)
{
	char dirname[256];
	int i = 0;
	char *cp;
	int temp = 0;

	//We get the first i_block and put it in the buf
	//get_block(fd, ip->i_block[0], buf);
	//dp = (DIR *)buf; //set the dir
	//cp = buf; //set the cp

	printf("-------- Dir Search --------\n");
	printf("Searching for %s\n", name[ind]);
	while (inoptr->i_block[i] != 0)
	{
		//get_block(fd, ip->i_block[0], buf);
		//put the iblock in the buf
		get_block(fd, inoptr->i_block[i], buf);
		dp = (DIR *)buf; //set the dir
		cp = buf; //set the cp

		printf("%d\n", dp->inode);
		while (cp < buf + BLKSIZE)
		{
		strncpy(dirname, dp->name, dp->name_len);
		dirname[dp->name_len] = 0;
		printf ("Dir name = %s\n", dirname);
		//Compare the dirname to the name we are looking for
		if (!strcmp(name[ind], dirname))
		{
			printf("Found what we were looking for!\n");
			return dp->inode;
		}
		//increment
		cp += dp->rec_len;
		dp = (DIR *)cp;
		//scanf("%d", &temp);
	}
	i++;
	}
	printf("Couldn't find it!\n");
	return 0;
}

int fullSearch(char name [1024][1024], int num_paths)
{
	int i = 0;
	int mail = 0;
	int ino = 0;
	char *cp;
	INODE dirnode;

	get_inode(fd, 2, &dirnode);
	for (i = 0; i < num_paths; i++)
	{

    if (i < num_paths)
    {
      if(!S_ISDIR(dirnode.i_mode))
      {
        printf("Cannot search through a file!\n");
        exit(0);
      }
    }

		ino = dirSearch(name, i, &dirnode);
    if (ino == 0)
    {
      printf("File does not exist!\n");
      exit(0);
    }

    printf("Found %s at inode #%d!\n", name[i], ino);
		get_inode(fd, ino, &dirnode);
	}
  return ino;
}

mailMan(int ino)
{
	int blocknum = 0;
	int inodenum = 0;

	blocknum = (ino-1)/8 + ip->i_block[0] - 1;
	printf("blocknum = %d\n", blocknum);
	//inodenum = (ino-1)%8;
	//printf("inodenum = %d\n", inodenum);

  return blocknum;
}

void printStuff(int ino, char name[1024][1024], int num)
{
	int i, j, cycle_blocks, num_blocks, indirect[256], double_indirect[256];
	INODE file;
	SUPER super;
	int blk_size = 1024;

	lseek(fd, iblock, SEEK_SET);
	read(fd, &super, sizeof(super));

	get_inode(fd, ino, &file);
	num_blocks = file.i_size / BLKSIZE;
	cycle_blocks = num_blocks;

	printf("========== PRINTING STUFF ==========\n");
	printf("Located %s!\n", name[num-1]);
	printf("Size: %u\n", file.i_size);
	printf("Blocks: %u\n", num_blocks);
	printf("Access time: %s", ctime(&(file.i_atime)));
	printf("Creation time: %s", ctime(&(file.i_ctime)));
	printf("Modification time: %s", ctime(&(file.i_mtime)));
	printf("Gid: %d\n", file.i_gid);
	printf("Links count: %d\n", file.i_links_count);
	printf("Flags: %d\n", file.i_flags);

	printf("-------- DISK BLOCKS --------\n");
	for (i = 0; i < 14; i++)
	{
		printf("block[%d]: %d\n", i, file.i_block[i]);
	}

	printf("\n-------- DIRECT BLOCKS --------\n");
	if (cycle_blocks > 12)
	{
		cycle_blocks = 12;
	}

	printBlocks(cycle_blocks, file.i_block);
	num_blocks = num_blocks - cycle_blocks;
	printf("\nBlocks Remaining: %u\n", num_blocks);

	if (num_blocks > 0)
	{
		printf("-------- INDIRECT BLOCKS --------\n");
		cycle_blocks = num_blocks;
		if (cycle_blocks > 256)
		{
			cycle_blocks = 256;
		}
		get_block(fd, file.i_block[12], indirect);
		printBlocks(cycle_blocks, indirect);
		num_blocks = num_blocks - cycle_blocks;
		printf("\nBlocks Remaining: %u\n", num_blocks);

		if (num_blocks > 0)
		{
			printf("-------- DOUBLE INDIRECT BLOCKS --------\n");
			get_block(fd, file.i_block[13], double_indirect);
			for (j = 0; j < 256; j++)
			{
				if (double_indirect[j] == 0)
				{
					break;
				}

				printf("-------- %d --------\n", double_indirect[j]);
				cycle_blocks = num_blocks;

				if (cycle_blocks > 256)
				{
					cycle_blocks = 256;
				}

				get_block(fd, double_indirect[j], indirect);
				printBlocks(cycle_blocks, indirect);
				num_blocks = num_blocks - cycle_blocks;
				printf("\nBlocks Remaining: %u\n", num_blocks);
			}
		}
	}

}


void printBlocks(int cycle_blocks, int indirect[256])
{
	int i;
	for (i = 0; i < cycle_blocks; i++)
	{
		printf("%d ", indirect[i]);
		if ((i + 1) % 16 == 0)
		{
			printf("\n");
		}
	}
}

int main(int argc, char *argv[])
{
	int i, j, ino;
	char buf[BLKSIZE];
	char path[2048];
	char *temp;
	//For parsed path
	char name[1024][1024];
  char *cp;

	//Check if enough arguments
	if(argc > 2)
	{
		device = argv[1];
		strcpy(path, argv[2]);
	}
	else
	{
		printf("not enough arguments\n");
		exit(0);
	}

	//Parse the path
  //This is also task 4
	i = 0;
	j = 0;
	strcat(path, "/");
	temp = strtok(path, "/");
	while(temp != NULL)
	{
		strcpy(name[i], temp);
		temp = strtok(NULL, "/");
		i++;
		j++;
	}

	printf("n = %d", j + 1);

	for(i = 0; i < j; i++)
		printf("  %s  ", name[i]);

  fd = open(device, O_RDONLY);
	if(fd < 0)
	{
		printf("open %s failed\n", device);
		exit(1);
	}

	printf("\n");

	//task 1
	super();

	//task 2
  //This puts the group descriptor block in the buff
  //So we can access the inodes
	get_block(fd, 2, buf);
  gp = (GD *)buf; //Points to the struct in our buf
  iblock = gp->bg_inode_table;   // Get inode start block#
  printf("inode_block=\t\t\t\t%d\n", iblock);

  // get inode start block
  // iblock is the starting point of the Inodes
  get_block(fd, iblock, buf); //This puts the inode table into the buf

  //Task 3
  ip = (INODE *)buf + 1;      // This makes ip point at the 2nd INODE WHICH IS ROOT

  printf ("-------- Root Node Info --------\n");
  printf("mode=%4x ", ip->i_mode);
  printf("uid=%d  gid=%d\n", ip->i_uid, ip->i_gid);
  printf("size=%d\n", ip->i_size);
  printf("time=%s", ctime(&ip->i_ctime));
  printf("link=%d\n", ip->i_links_count);
  printf("i_block[0]=%d\n", ip->i_block[0]);

  //Task 5
	//ino = dirSearch(name);

  //Task 6
	//mailMan(ino);

	ino = fullSearch(name, j);

	//Task 9
	//Print some stuff
	printStuff(ino, name, j);


	return 0;
}
