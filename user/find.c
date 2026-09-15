#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* get_filename(char *path){
    char *p;
    for(p = path + strlen(path); p >= path && *p != '/'; p--){}
    p++;
    return p;
}

void do_find(char *current_path, char *target_name){
    char buf[512];
    char *p;
    int fd;
    struct dirent dir_entry;
    struct stat file_stat;

    fd = open(current_path, 0);
    if(fd < 0){
        fprintf(2, "find: cannot open %s\n", current_path);
        return;
    }

    if(fstat(fd, &file_stat) < 0){
        fprintf(2, "find: cannot stat %s\n", current_path);
        close(fd);
        return;
    }

    char *name = get_filename(current_path);
    if(strcmp(name, target_name) == 0){
        printf("%s\n", current_path);
    }

    if(file_stat.type == T_DIR){
        
        strcpy(buf, current_path);
        p = buf + strlen(buf);
        *p = '/';
        p++;

        while(read(fd, &dir_entry, sizeof(dir_entry)) == sizeof(dir_entry)){
            if(dir_entry.inum == 0){ continue; }
            if(strcmp(dir_entry.name, ".") == 0 || strcmp(dir_entry.name, "..") == 0){ continue; }
            memmove(p, dir_entry.name, DIRSIZ);
            p[DIRSIZ] = 0;
            do_find(buf, target_name);
        }
    }
    
    close(fd);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("usage: find <path> <filename>\n");
        exit(1);
    }
    
    do_find(argv[1], argv[2]);
    
    exit(0);
}
