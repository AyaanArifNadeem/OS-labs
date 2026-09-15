#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void do_sixfive(int fd) {
    char c;
    int num = 0;
    int in_number = 0;
    int is_valid = 1; 

    while(read(fd, &c, 1) > 0) {
        
        if(strchr(" -\r\t\n./,", c) != 0) {
            if(in_number && is_valid) {
                if(num % 5 == 0 || num % 6 == 0) {
                    printf("%d\n", num);
                }
            }
            num = 0;
            in_number = 0;
            is_valid = 1;
        } 
        else if(c >= '0' && c <= '9') {
            in_number = 1;
            num = num * 10 + (c - '0');
        } 
        else { is_valid = 0; }
    }

    if(in_number && is_valid) {
        if(num % 5 == 0 || num % 6 == 0) { printf("%d\n", num); }
    }
}

int main(int argc, char *argv[]) {
    if(argc == 1) { do_sixfive(0); } 
    else {
        for(int i = 1; i < argc; i++) {
            int fd = open(argv[i], 0);
            if(fd < 0) { printf("sixfive: cannot open %s\n", argv[i]); } 
            else {
                do_sixfive(fd);
                close(fd);
            }
        }
    }
    exit(0);
}
