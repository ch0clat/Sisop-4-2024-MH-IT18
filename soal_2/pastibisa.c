//! SECOND COMMIT COMPILING PROBLEM
// pastibisa.c:20:22: warning: ‘dirpath’ defined but not used [-Wunused-variable]
//    20 | static  const  char *dirpath = "/home/sisop/sisop/sensitif";
//       |                      ^~~~~~~
// /usr/bin/ld: /tmp/ccESjxPa.o: in function `decode_base64':
// /home/sisop/Sisop-4-2024-MH-IT18/soal_2/pastibisa.c:30: undefined reference to `BIO_f_base64'
// /usr/bin/ld: /home/sisop/Sisop-4-2024-MH-IT18/soal_2/pastibisa.c:30: undefined reference to `BIO_new'
// /usr/bin/ld: /home/sisop/Sisop-4-2024-MH-IT18/soal_2/pastibisa.c:31: undefined reference to `BIO_new_fp'
// /usr/bin/ld: /home/sisop/Sisop-4-2024-MH-IT18/soal_2/pastibisa.c:32: undefined reference to `BIO_push'
// /usr/bin/ld: /home/sisop/Sisop-4-2024-MH-IT18/soal_2/pastibisa.c:34: undefined reference to `BIO_read'
// /usr/bin/ld: /home/sisop/Sisop-4-2024-MH-IT18/soal_2/pastibisa.c:38: undefined reference to `BIO_free_all'
// /usr/bin/ld: /tmp/ccESjxPa.o: in function `main':
// /home/sisop/Sisop-4-2024-MH-IT18/soal_2/pastibisa.c:207: undefined reference to `fuse_main_real'
// collect2: error: ld returned 1 exit status
//!


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
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

static char access_password[256] = "";

static  const  char *dirpath = "/home/sisop/sisop/sensitif";

//// Make a base64 decode
void decode_base64(char *input, char *output) {
//     //!NOT DONE YET
BIO *bio, *b64;
    int decodeLen = strlen(input);
    char *buffer = (char *)malloc(decodeLen);
    FILE *stream = fmemopen(input, decodeLen, "r");

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_fp(stream, BIO_NOCLOSE);
    bio = BIO_push(b64, bio);

    decodeLen = BIO_read(bio, buffer, decodeLen);
    buffer[decodeLen] = '\0';
    strncpy(output, buffer, decodeLen + 1);

    BIO_free_all(bio);
    fclose(stream);
    free(buffer);
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

    const char *filename = strrchr(path, '/');
    if (filename == NULL) filename = path; else filename++;

    char file_buf[4096]; // Sesuaikan ukuran buffer dengan kebutuhan
    char output_buf[4096];

    if (strncmp(filename, "rahasia", 7) == 0 && strcmp(access_password, "") == 0) {
        printf("Masukkan password: ");
        if (fgets(access_password, sizeof(access_password), stdin) == NULL) {
            return -1;
        }
        // Hapus newline character di akhir input
        access_password[strcspn(access_password, "\n")] = 0;
    }

    if (strcmp(filename, "notes_base64.txt") == 0) {
        decode_base64(file_buf, output_buf);
    } else if (strcmp(filename, "enkripsi_rot13.txt") == 0) {
        decode_rot13(file_buf, output_buf);
    } else if (strcmp(filename, "new_hex.txt") == 0) {
        decode_hex(file_buf, output_buf);
    } else if (strcmp(filename, "rev_text.txt") == 0) {
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

    printf("Set password for 'rahasia' : ");
    if (fgets(access_password, sizeof(access_password), stdin) == NULL) {
        return 1;
    }

    size_t len = strlen(access_password);
    if (len > 0 && access_password[len - 1] == '\n') {
        access_password[len - 1] = '\0';
    }

    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}