/*****************************************
CptS 360 Final Project

mount root functions
contains init(), mount_root(), cd(), ls(), pwd(), and quit()

Programmers:
David Hoekman 11423672
Luke Darrow 11349190

Last Modified: 11/23/15
*****************************************/

/*
MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;
MOUNT  mounttab[5];

char names[64][128],*name[64];
int fd, dev, n;
int nblocks, ninodes, bmap, imap, inode_start, iblock;
int inodeBeginBlock;
char pathname[256], parameter[256];
int DEBUG;
*/

//2. Initialize data structures of LEVEL-1
init()
{
	//2 PROCs, P0 with uid=0, P1 with uid=1, all PROC.cwd = 0
	//MINODE minode[100]; all with refCount=0
	//MINODE *root = 0;
	int i;

	//allocate memory for running process
	running = malloc(sizeof(PROC));

	proc[0].pid = 1;
	proc[0].uid = 0;
	proc[0].cwd = 0;

	proc[1].pid = 2;
	proc[1].uid = 1;
	proc[1].cwd = 0;

	running = &proc[0];

	//MINODE minode[100] all with refCount=0
	for(i = 0; i < 100; i++)
	{
		minode[i].refCount = 0;
	}

	//MINODE *root = 0;
	root = 0;
}

//mount root file system, establish / and CWDs
mount_root(char device_name[64])
{
	char buf[1024];
	//open device for RW
	dev = open(device_name, O_RDWR);

	//check if open() worked
	if(dev < 0)
	{
		printf("ERROR: failed cannot open %s\n", device_name);
		exit(0);
	}

	//read super block to verify it's an EXT2 FS
	get_block(dev, SUPERBLOCK, buf);
	sp = (SUPER *)buf;
	//verify if it's an EXT2 FS
	if(sp->s_magic != 0xEF53)
	{
		printf("NOT AN EXT2 FS\n");
		exit(1);
	}

	//set some vars
	ninodes = sp->s_inodes_count;
	nblocks = sp->s_blocks_count;

	//read group block for info
	get_block(dev, GDBLOCK, buf);
	gp = (GD *)buf;

	imap = gp->bg_inode_bitmap;
	bmap = gp->bg_block_bitmap;

	inodeBeginBlock = gp->bg_inode_table;

	//get root inode
	root = iget(dev, 2);

	//let cwd of both p0 and p1 point at the root minode (refCount=3)
	proc[0].cwd = root;
	proc[1].cwd = root;

	root->refCount = 3;

	printf("%s mounted\n", device_name);
}

//print information for file or dir
void print_info(MINODE *mip, char *name)
{
	int i;
	INODE *ip = &mip->INODE;

	char *permissions = "rwxrwxrwx";

	//information for each file
	u16 mode   = ip->i_mode;
    u16 links  = ip->i_links_count;
    u16 uid    = ip->i_uid;
    u16 gid    = ip->i_gid;
    u32 size   = ip->i_size;

	char *time = ctime( (time_t*)&ip->i_mtime);
	//remove \r from time
	time[strlen(time) - 1] = 0;

	switch(mode & 0xF000)
	{
		case 0x8000:  putchar('-');     break; // 0x8 = 1000
        case 0x4000:  putchar('d');     break; // 0x4 = 0100
        case 0xA000:  putchar('l');     break; // oxA = 1010
        default:      putchar('?');     break;
	}

	//print the permissions
	for(i = 0; i < strlen(permissions); i++)
		putchar(mode & (1 << (strlen(permissions) - 1 - i)) ? permissions[i] : '-');

	//print the rest
	printf("%4hu %4hu %4hu %8u %26s  %s\n", links, gid, uid, size, time, name);
	strcat(teststr, name);
	strcat(teststr, " ");

	//done
	return;
}

//print directory
//Goes through the directory and runs print info for every dp
void print_dir(MINODE *mip)
{
	int i;
	DIR *dp;
	char *cp;
	char buf[1024], temp[1024];
	INODE *ip = &mip->INODE;
	MINODE *temp_mip;

	printf("printing directory\n");

	for(i = 0; i < ip->i_size/1024; i++)
	{
		if(ip->i_block[i] == 0)
			break;

		get_block(dev, ip->i_block[i], buf);
		dp = (DIR*)buf;
		cp = buf;

		//print the contents
		while(cp < buf + BLKSIZE)
		{
			strncpy(temp, dp->name, dp->name_len);
			temp[dp->name_len] = 0;

			//need to print file permissions and such
			//printf("%s\n", temp);

			temp_mip = iget(dev, dp->inode);
			if(temp_mip)
			{
				print_info(temp_mip, temp);
				iput(temp_mip);
			}
			else
				printf("ERROR: Cannot print info for Minode\n");

			memset(temp, 0, 1024);
			cp += dp->rec_len;
			dp = (DIR*)cp;
		}
	}

	printf("\n");
}

//prints everything located at the pathname
//Calls printdir which calls printinfo for a given pathname
//The main function of ls is to get the pathname and check for certain cases
//Finds the ino of the pathname
void ls(char *pathname)
{
	int ino, offset;
	MINODE *mip = running->cwd;
	char name[64][64], temp[64];
	char buf[1024];

	//ls cwd
	if(!strcmp(pathname, ""))
	{
		//print_dir(mip->INODE);
		print_dir(mip);
		return;
	}

	//ls root dir
	if(!strcmp(pathname, "/"))
	{
		//print_dir(root->INODE);
		print_dir(root);
		return;
	}

	//if there's a pathname, ls pathname
	if(pathname)
	{
		//check if path starts at root
		if(pathname[0] == '/')
		{
			mip = root;
		}

		//search for path to print
		ino = getino(mip, pathname);
		if(ino == 0)
		{
			return;
		}

		mip = iget(dev, ino);
		if(!S_ISDIR(mip->INODE.i_mode))
		{
			printf("%s is not a directory!\n", pathname);
			strcpy(teststr, pathname);
			strcat(teststr, " is not a directory!");
			iput(mip);
			return;
		}

		//print_dir(mip->INODE);
		print_dir(mip);
		iput(mip);
	}
	else
	{
		//print root dir
		//print_dir(root->INODE);
		print_dir(root);
	}
}

//change cwd to pathname. If no pathname, set cwd to root.
//Checks some casses, ultimatly sets running->cwd to the inode of the pathname
void cd(char *pathname)
{
	int ino = 0;

	MINODE *mip = running->cwd;
	MINODE *newmip = NULL;

	if (!strcmp(pathname, "")) //If pathname is empty CD to root
	{
		running->cwd = root;
		return;
	}

	if (!strcmp(pathname, "/"))
	{
		running->cwd = root;
		return;
	}

	printf("Path = %s\n", pathname);
	ino = getino(mip, pathname);
	if(ino == 0)
	{
		printf("The directory %s does not exist!\n", pathname);
		return;
	}

	newmip = iget(dev, ino);
	if(!S_ISDIR(newmip->INODE.i_mode))
	{
		printf("%s is not a directory!\n", pathname);
		iput(newmip);
		return;
	}

	running->cwd = newmip;
	iput(newmip);
	return;
}

//iput all DIRTY minodes before shutdown
int quit()
{
	int i;

	//go through all the minodes
	for(i = 0; i < NMINODE; i++)
	{
		//check if used and dirty
		if(minode[i].refCount > 0 && minode[i].dirty == 1)
		{
			minode[i].refCount = 1;
			iput(&minode[i]);
		}
	}

	printf("Goodbye!\n");
	exit(0);
}
