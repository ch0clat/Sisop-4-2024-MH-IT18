#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

static char access_password[256] = "";
static const char *dirpath = "/home/ubuntu-sisop/sisop/sensitif";

const char* get_password_from_env() {
    return getenv("FUSE_PASSWORD");
}

void decode_base64(const char *input, char *output) {
    BIO *bio, *b64;
    size_t decodeLen = strlen(input);
    char *buffer = (char *)malloc(decodeLen);
    FILE *stream = fmemopen((void*)input, decodeLen, "r");

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

void log_message(const char* status, const char* tag, const char* additional_info) {
    time_t rawtime;
    struct tm *info;
    char timestamp[80];

    time(&rawtime);
    info = localtime(&rawtime);

    strftime(timestamp, 80, "%d/%m/%Y-%H:%M:%S", info);

    FILE *fp = fopen("/home/ubuntu-sisop/sisop/logs-fuse.log", "a");
    if (fp == NULL) {
        perror("Failed to open log file");
        return;
    }

    fprintf(fp, "[%s]::%s::[%s]::[%s]\n", status, timestamp, tag, additional_info);
    fclose(fp);
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
    int res;
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);
    res = lstat(fpath, stbuf);

    if (res == -1) return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    dp = opendir(fpath);

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

static int xmp_open(const char *path, struct fuse_file_info *fi) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    if (strncmp(path, "/rahasia-berkas", 15) == 0 && strcmp(access_password, "") == 0) {
        printf("Masukkan password: ");
        if (fgets(access_password, sizeof(access_password), stdin) == NULL) {
            log_message("FAILED", "open", "Password input failed");
            return -EACCES;
        }
        access_password[strcspn(access_password, "\n")] = 0;
    }
    if (strncmp(path, "/rahasia-berkas", 15) == 0 && strcmp(access_password, "ihserem") != 0) {
        fprintf(stderr, "Invalid password!\n");
        log_message("FAILED", "open", "Invalid password");
        return -EACCES;
    }

    int res = open(fpath, fi->flags);
    if (res == -1) {
        perror("open");
        log_message("FAILED", "open", fpath);
        return -errno;
    }

    close(res);
    log_message("SUCCESS", "open", fpath);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd;
    int res;

    (void) fi;

    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    fd = open(fpath, O_RDONLY);
    if (fd == -1) {
        log_message("FAILED", "read", fpath);
        return -errno;
    }

    res = pread(fd, buf, size, offset);
    if (res == -1) {
        close(fd);
        log_message("FAILED", "read", fpath);
        return -errno;
    }

    char decoded_content[4096] = {0};
    if (strstr(fpath, "base64")) {
        decode_base64(buf, decoded_content);
    } else if (strstr(fpath, "rot13")) {
        decode_rot13(buf, decoded_content);
    } else if (strstr(fpath, "hex")) {
        decode_hex(buf, decoded_content);
    } else if (strstr(fpath, "rev")) {
        decode_rev(buf, decoded_content);
    }

    //!INI YANG PENTING
    if (decoded_content[0] != '\0') {
        strncpy(buf, decoded_content, size);
    }

    log_message("SUCCESS", "read", fpath);
    close(fd);
    return res;
}

int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char fpath[1000];
    sprintf(fpath, "%s%s", dirpath, path);

    int fd = open(fpath, O_WRONLY);
    if (fd == -1) {
        return -errno;
    }

    int res = pwrite(fd, buf, size, offset);
    if (res == -1) {
        res = -errno;
    }

    close(fd);
    log_message("SUCCESS", "write", fpath);
    return res;
}

int xmp_rename(const char *from, const char *to) {
    char ffrom[1000], fto[1000];
    sprintf(ffrom, "%s%s", dirpath, from);
    sprintf(fto, "%s%s", dirpath, to);

    // Perform the file rename operation
    int res = rename(ffrom, fto);
    if (res == -1) {
        log_message("FAILED", "rename", ffrom);
        perror("rename");
        return -errno;
    }

    if (strncmp(to, "decoded_", 8) == 0) {
        FILE *src_file = fopen(ffrom, "r");
        if (src_file == NULL) {
            log_message("FAILED", "rename", "Failed to open source file for decoding");
            return -errno;
        }

        fseek(src_file, 0, SEEK_END);
        long src_size = ftell(src_file);
        fseek(src_file, 0, SEEK_SET);
        char *file_content = (char *)malloc(src_size + 1);
        fread(file_content, 1, src_size, src_file);
        file_content[src_size] = '\0';
        fclose(src_file);

        char decoded_content[4096] = {0};
        if (strstr(from, "base64")) {
            decode_base64(file_content, decoded_content);
        } else if (strstr(from, "rot13")) {
            decode_rot13(file_content, decoded_content);
        } else if (strstr(from, "hex")) {
            decode_hex(file_content, decoded_content);
        } else if (strstr(from, "rev")) {
            decode_rev(file_content, decoded_content);
        }

        free(file_content);

        FILE *dst_file = fopen(fto, "w");
        if (dst_file == NULL) {
            log_message("FAILED", "rename", "Failed to open destination file for writing");
            return -errno;
        }
        fwrite(decoded_content, 1, strlen(decoded_content), dst_file);
        fclose(dst_file);

        log_message("SUCCESS", "rename", fto);
    } else {
        log_message("SUCCESS", "rename", fto);
    }

    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .open = xmp_open,
    .read = xmp_read,
    .write = xmp_write,
    .rename = xmp_rename,
};

int main(int argc, char *argv[]) {
    printf("Set password for 'rahasia-berkas': ");
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
