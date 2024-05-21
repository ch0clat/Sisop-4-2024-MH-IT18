#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <cstring>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>

static const char *SOURCE_DIR = "relics";
static const char *MOUNT_POINT = "[nama_bebas]";
static const size_t MAX_CHUNK_SIZE = 10240; // 10 KB

std::vector<std::string> get_combined_file(const std::string& file_path) {
    std::vector<std::string> chunks;
    int chunk_index = 0;
    std::string chunk_path;

    do {
        chunk_path = file_path + "." + std::to_string(chunk_index);
        if (access(chunk_path.c_str(), F_OK) == 0) {
            chunks.push_back(chunk_path);
            chunk_index++;
        } else {
            break;
        }
    } while (true);

    return chunks;
}

std::vector<std::string> split_file(const std::string& file_path, size_t max_size) {
    std::vector<std::string> chunks;
    FILE* file = fopen(file_path.c_str(), "rb");
    if (file == nullptr) {
        return chunks;
    }

    std::vector<char> buffer(max_size);
    size_t bytes_read;
    int chunk_index = 0;
    do {
        bytes_read = fread(buffer.data(), 1, max_size, file);
        if (bytes_read > 0) {
            std::string chunk_path = file_path + "." + std::to_string(chunk_index);
            chunks.push_back(chunk_path);
            FILE* chunk_file = fopen(chunk_path.c_str(), "wb");
            fwrite(buffer.data(), 1, bytes_read, chunk_file);
            fclose(chunk_file);
            chunk_index++;
        }
    } while (bytes_read == max_size);

    fclose(file);
    return chunks;
}

static int fuse_getattr(const char *path, struct stat *st) {
    int res = lstat(path, st);
    if (res == -1)
        return -errno;

    return 0;
}

static int fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;

    (void)offset;
    (void)fi;

    dp = opendir(path);
    if (dp == nullptr)
        return -errno;

    while ((de = readdir(dp)) != nullptr) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        std::string entry_path = std::string(path) + "/" + std::string(de->d_name);
        if (strcmp(path, "/") == 0)
            entry_path = "/" + std::string(de->d_name);

        if (strcmp(entry_path.c_str(), SOURCE_DIR) == 0) {
            std::vector<std::string> combined_files;
            std::string combined_name;

            DIR *source_dir = opendir(SOURCE_DIR);
            if (source_dir == nullptr) {
                closedir(dp);
                return -errno;
            }

            struct dirent *source_entry;
            while ((source_entry = readdir(source_dir)) != nullptr) {
                if (source_entry->d_type == DT_REG) {
                    std::string source_path = std::string(SOURCE_DIR) + "/" + std::string(source_entry->d_name);
                    std::vector<std::string> chunks = get_combined_file(source_path);
                    if (!chunks.empty()) {
                        combined_name = source_entry->d_name;
                        for (const auto& chunk : chunks) {
                            st.st_size += lstat(chunk.c_str(), &st);
                        }
                        if (filler(buf, combined_name.c_str(), &st, 0))
                            break;
                    }
                }
            }

            closedir(source_dir);
        } else {
            if (filler(buf, de->d_name, &st, 0))
                break;
        }
    }

    closedir(dp);
    return 0;
}

static int fuse_read(const char *path, char *buf, size_t size, off_t offset,
                     struct fuse_file_info *fi) {
    std::vector<std::string> chunks = get_combined_file(path);
    if (chunks.empty()) {
        return -ENOENT;
    }

    size_t total_read = 0;
    for (const auto& chunk_path : chunks) {
        FILE* chunk_file = fopen(chunk_path.c_str(), "rb");
        if (chunk_file == nullptr) {
            return -errno;
        }

        fseek(chunk_file, offset, SEEK_SET);
        size_t chunk_read = fread(buf + total_read, 1, size - total_read, chunk_file);
        total_read += chunk_read;
        offset += chunk_read;

        fclose(chunk_file);
    }

    return total_read;
}

static int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    std::vector<std::string> chunks = split_file(path, MAX_CHUNK_SIZE);
    if (chunks.empty()) {
        return -errno;
    }

    return 0;
}

static int fuse_unlink(const char *path) {
    std::vector<std::string> chunks = get_combined_file(path);
    if (chunks.empty()) {
        return -ENOENT;
    }

    for (const auto& chunk_path : chunks) {
        if (unlink(chunk_path.c_str()) != 0) {
            return -errno;
        }
    }

    return 0;
}

static int fuse_write(const char *path, const char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {
    std::vector<std::string> chunks = get_combined_file(path);
    if (chunks.empty()) {
        return -ENOENT;
    }

    size_t total_written = 0;
    off_t current_offset = offset;
    for (const auto& chunk_path : chunks) {
        FILE* chunk_file = fopen(chunk_path.c_str(), "r+b");
        if (chunk_file == nullptr) {
            return -errno;
        }

        fseek(chunk_file, current_offset, SEEK_SET);
        size_t chunk_written = fwrite(buf + total_written, 1, size - total_written, chunk_file);
        total_written += chunk_written;
        current_offset += chunk_written;

        fclose(chunk_file);
    }

    return total_written;
}

static struct fuse_operations fuse_ops = {
    .getattr = fuse_getattr,
    .readdir = fuse_readdir,
    .read = fuse_read,
    .create = fuse_create,
    .unlink = fuse_unlink,
    .write = fuse_write,
};

int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &fuse_ops, nullptr);
}tes
