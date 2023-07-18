#if 0
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
find(char *path, char *fname)
{
  char fmt_fname[DIRSIZ+1];
  memmove(fmt_fname,fname,strlen(fname));
  memset(fmt_fname+strlen(fname),' ',DIRSIZ-strlen(fname));


  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    char name[DIRSIZ+1];
    strcpy(name,fmtname(path));
    //printf("%s, %d\n",fmtname(path), strcmp(name,fmt_fname));
    if(!strcmp(name, fmt_fname)){
      printf("%s\n",path);
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0 || de.inum==1||strcmp(de.name,".")==1 || strcmp(de.name,"..")==1)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[strlen(de.name)] = 0;
      find(path,fname);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc!=3){
    printf("Usage: %s <path_name> <filename>\n",argv[0]);
    exit(-1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
#else

#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"


char* fmt_name(char *path){
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--);
  p++;
  memmove(buf, p, strlen(p)+1);
  return buf;
}

void IsEqual(char *fileName, char *findName){
	if(strcmp(fmt_name(fileName), findName) == 0){
		printf("%s\n", fileName);
	}
}


void find(char *path, char *findName){
	int fd;
	struct stat st;	
    char buf[512], *p;	
	struct dirent de;

	if((fd = open(path, O_RDONLY)) < 0){
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}
	if(fstat(fd, &st) < 0){
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}
	
	switch(st.type){	
		case T_FILE:
			IsEqual(path, findName);			
			break;
		case T_DIR:
			if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
				printf("find: path too long\n");
				break;
			}
			strcpy(buf, path);
			p = buf+strlen(buf);
			*p++ = '/';
			while(read(fd, &de, sizeof(de)) == sizeof(de)){
				if(de.inum == 0 || de.inum == 1 || strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0)
					continue;				
				memmove(p, de.name, strlen(de.name));
				p[strlen(de.name)] = 0;
				find(buf, findName);
			}
			break;
	}
	close(fd);	
}

int main(int argc, char *argv[]){
	if(argc < 3){
		printf("find: find <path> <fileName>\n");
		exit(0);
	}
	find(argv[1], argv[2]);
	exit(0);
}
#endif