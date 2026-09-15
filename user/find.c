#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

// --- REGEX FUNCTIONS BORROWED FROM GREP.C ---
int matchhere(char*, char*);
int matchstar(int, char*, char*);

int match(char *re, char *text){
    if(re[0] == '^')
        return matchhere(re+1, text);
    do{
        if(matchhere(re, text))
            return 1;
    } while(*text++ != '\0');
    return 0;
}

int matchhere(char *re, char *text){
    if(re[0] == '\0')
        return 1;
    if(re[1] == '*')
        return matchstar(re[0], re+2, text);
    if(re[0] == '$' && re[1] == '\0')
        return *text == '\0';
    if(*text != '\0' && (re[0] == '.' || re[0] == *text))
        return matchhere(re+1, text+1);
    return 0;
}

int matchstar(int c, char *re, char *text){
    do{
        if(matchhere(re, text))
            return 1;
    } while(*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}
// --------------------------------------------

char* get_filename(char *path){
    char *p;
    for(p = path + strlen(path); p >= path && *p != '/'; p--){}
    p++; 
    return p;
}

void do_find(char *current_path, char *target_name, char **exec_cmd, int exec_cmd_len){
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
    
    if(match(target_name, name)){
        
        if (exec_cmd_len > 0){
            if (fork() == 0){
                char *cmd_args[MAXARG];
                int i;
                
                for (i = 0; i < exec_cmd_len; i++){ cmd_args[i] = exec_cmd[i]; }
                
                cmd_args[i] = current_path;
                cmd_args[i+1] = 0;

                exec(cmd_args[0], cmd_args);
                
                printf("exec failed\n");
                exit(1);
            } else{
                wait(0);
            }
        } else{
            printf("%s\n", current_path);
        }
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
            
            do_find(buf, target_name, exec_cmd, exec_cmd_len);
        }
    }
    
    close(fd);
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("usage: find <path> <filename/regex> [-exec cmd ...]\n");
        exit(1);
    }
    
    char **exec_cmd = 0;
    int exec_cmd_len = 0;

    if (argc >= 4 && strcmp(argv[3], "-exec") == 0){
        exec_cmd = &argv[4];
        exec_cmd_len = argc - 4;
    }
    
    do_find(argv[1], argv[2], exec_cmd, exec_cmd_len);
    
    exit(0);
}
