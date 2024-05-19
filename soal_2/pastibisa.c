#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

static  const  char *dirpath = "/home/sisop/sisop/sensitif";

//todo Make a base64 decode
void decode_base64(char *input, char *output) {
//     //!NOT DONE YET
//    while (*input) {
//         if ((*input >= 'a' && *input <= 'z') || (*input >= 'A' && *input <= 'Z')) {
//             if ((*input >= 'n' && *input <= 'z') || (*input >= 'N' && *input <= 'Z')) {
//                 *output = *input - 13;
//             } else {
//                 *output = *input + 13;
//             }
//         } else {
//             *output = *input;
//         }
//         input++;
//         output++;
//     }
//     *output = '\0';
}
 
void decode_rot13(char *input, char *output) {
    while (*input) {
        if ((*input >= 'a' && *input <= 'z') || (*input >= 'A' && *input <= 'Z')) {
            if ((*input >= 'n' && *input <= 'z') || (*input >= 'N' && *input <= 'Z')) {
                *output = *input - 13;
            } else {
                *output = *input + 13;
            }
        } else {
            *output = *input;
        }
        input++;
        output++;
    }
    *output = '\0';
}

void decode_hex(char *input, char *output) {
    while (*input && *(input + 1)) {
        *output = (char)((strchr("0123456789ABCDEF", toupper(*input)) - "0123456789ABCDEF") * 16 +
                         (strchr("0123456789ABCDEF", toupper(*(input + 1))) - "0123456789ABCDEF"));
        input += 2;
        output++;
    }
    *output = '\0';
}

void decode_rev(char *input, char *output) {
    size_t len = strlen(input);
    for (size_t i = 0; i < len; i++) {
        output[i] = input[len - i - 1];
    }
    output[len] = '\0';
}

void log_message(const char* source, const char* tag, const char* additional_info) {
    time_t rawtime;
    struct tm *info;
    char timestamp[80];

    time(&rawtime);
    info = localtime(&rawtime);

    strftime(timestamp, 80, "%d/%m/%Y-%H:%M:%S", info);

    FILE *fp = fopen("/home/sisop/sisop/logs-fuse.log", "a");
    if (fp == NULL) {
        perror("Failed to open log file");
        return;
    }

    fprintf(fp, "%s::[%s]::[%s]\n",timestamp, tag, additional_info);
    fclose(fp);
}

static  int  xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    res = lstat(path, stbuf);

    if (res == -1) return -errno;
    return 0;
}



static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(path);

    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if(filler(buf, de->d_name, &st, 0)) break;
    }
    closedir(dp);
    return 0;
}



static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;
    (void) fi;

    fd = open(path, O_RDONLY);

    if (fd == -1) return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1) res = -errno;

    if (strncmp(filename, "base64-", 7) == 0) {
        decode_base64(file_buf, output_buf);
    } else if (strncmp(filename, "rot13-", 6) == 0) {
        decode_rot13(file_buf, output_buf);
    } else if (strncmp(filename, "hex-", 4) == 0) {
        decode_hex(file_buf, output_buf);
    } else if (strncmp(filename, "rev-", 4) == 0) {
        decode_rev(file_buf, output_buf);
    } else {
        strncpy(output_buf, file_buf, size);
        output_buf[size] = '\0';
    }

    close(fd);

    return res;
}



static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
};



int  main(int  argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}