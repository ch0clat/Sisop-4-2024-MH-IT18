# Sisop-4-2024-MH-IT18

## Anggota kelompok
   ### Callista Meyra Azizah 5027231060
   ### Abhirama Triadyatma Hermawan 5027231061
   ### Adi Satria Pangestu 5027231043

# Soal
## Soal 1
Adfi merupakan seorang CEO agency creative bernama Ini Karya Kita. Ia sedang melakukan inovasi pada manajemen project photography Ini Karya Kita. Salah satu ide yang dia kembangkan adalah tentang pengelolaan foto project dalam sistem arsip Ini Karya Kita. Dalam membangun sistem ini, Adfi tidak bisa melakukannya sendirian, dia perlu bantuan mahasiswa Departemen Teknologi Informasi angkatan 2023 untuk membahas konsep baru yang akan mengubah project fotografinya lebih menarik untuk dilihat. Adfi telah menyiapkan portofolio hasil project fotonya yang bisa didownload dan diakses di www.inikaryakita.id . Silahkan eksplorasi web Ini Karya Kita dan temukan halaman untuk bisa mendownload projectnya. Setelah kalian download terdapat folder gallery dan bahaya.
Pada folder “gallery”:
Membuat folder dengan prefix "wm." Dalam folder ini, setiap gambar yang dipindahkan ke dalamnya akan diberikan watermark bertuliskan inikaryakita.id. 
			Ex: "mv ikk.jpeg wm-foto/" 
Output: 
Before: (tidak ada watermark bertuliskan inikaryakita.id)
After: (terdapat watermark tulisan inikaryakita.id)
Pada folder "bahaya," terdapat file bernama "script.sh." Adfi menyadari pentingnya menjaga keamanan dan integritas data dalam folder ini. 
Mereka harus mengubah permission pada file "script.sh" agar bisa dijalankan, karena jika dijalankan maka dapat menghapus semua dan isi dari  "gallery"
Adfi dan timnya juga ingin menambahkan fitur baru dengan membuat file dengan prefix "test" yang ketika disimpan akan mengalami pembalikan (reverse) isi dari file tersebut.  

## Soal 2
Masih dengan Ini Karya Kita, sang CEO ingin melakukan tes keamanan pada folder sensitif Ini Karya Kita. Karena Teknologi Informasi merupakan departemen dengan salah satu fokus di Cyber Security, maka dia kembali meminta bantuan mahasiswa Teknologi Informasi angkatan 2023 untuk menguji dan mengatur keamanan pada folder sensitif tersebut. Untuk mendapatkan folder sensitif itu, mahasiswa IT 23 harus kembali mengunjungi website Ini Karya Kita pada www.inikaryakita.id/schedule . Silahkan isi semua formnya, tapi pada form subject isi dengan nama kelompok_SISOP24 , ex: IT01_SISOP24 . Lalu untuk form Masukkan Pesanmu, ketik “Mau Foldernya” . Tunggu hingga 1x24 jam, maka folder sensitif tersebut akan dikirimkan melalui email kalian. Apabila folder tidak dikirimkan ke email kalian, maka hubungi sang CEO untuk meminta bantuan.   
Pada folder "pesan" Adfi ingin meningkatkan kemampuan sistemnya dalam mengelola berkas-berkas teks dengan menggunakan fuse.
Jika sebuah file memiliki prefix "base64," maka sistem akan langsung mendekode isi file tersebut dengan algoritma Base64.
Jika sebuah file memiliki prefix "rot13," maka isi file tersebut akan langsung di-decode dengan algoritma ROT13.
Jika sebuah file memiliki prefix "hex," maka isi file tersebut akan langsung di-decode dari representasi heksadesimalnya.
Jika sebuah file memiliki prefix "rev," maka isi file tersebut akan langsung di-decode dengan cara membalikkan teksnya.
Contoh:
File yang belum didecode/ dienkripsi rot_13

File yang sudah didecode/ dienkripsi rot_13

Pada folder “rahasia-berkas”, Adfi dan timnya memutuskan untuk menerapkan kebijakan khusus. Mereka ingin memastikan bahwa folder dengan prefix "rahasia" tidak dapat diakses tanpa izin khusus. 
Jika seseorang ingin mengakses folder dan file pada “rahasia”, mereka harus memasukkan sebuah password terlebih dahulu (password bebas). 
Setiap proses yang dilakukan akan tercatat pada logs-fuse.log dengan format :
[SUCCESS/FAILED]::dd/mm/yyyy-hh:mm:ss::[tag]::[information]
Ex:
[SUCCESS]::01/11/2023-10:43:43::[moveFile]::[File moved successfully]

## Soal 3
Seorang arkeolog menemukan sebuah gua yang didalamnya tersimpan banyak relik dari zaman praaksara, sayangnya semua barang yang ada pada gua tersebut memiliki bentuk yang terpecah belah akibat bencana yang tidak diketahui. Sang arkeolog ingin menemukan cara cepat agar ia bisa menggabungkan relik-relik yang terpecah itu, namun karena setiap pecahan relik itu masih memiliki nilai tersendiri, ia memutuskan untuk membuat sebuah file system yang mana saat ia mengakses file system tersebut ia dapat melihat semua relik dalam keadaan utuh, sementara relik yang asli tidak berubah sama sekali.
Ketentuan :
Buatlah sebuah direktori dengan ketentuan seperti pada tree berikut
.
├── [nama_bebas]
├── relics
│   ├── relic_1.png.000
│   ├── relic_1.png.001
│   ├── dst dst…
│   └── relic_9.png.010
└── report

Direktori [nama_bebas] adalah direktori FUSE dengan direktori asalnya adalah direktori relics. Ketentuan Direktori [nama_bebas] adalah sebagai berikut :
Ketika dilakukan listing, isi dari direktori [nama_bebas] adalah semua relic dari relics yang telah tergabung.

Ketika dilakukan copy (dari direktori [nama_bebas] ke tujuan manapun), file yang disalin adalah file dari direktori relics yang sudah tergabung.

Ketika ada file dibuat, maka pada direktori asal (direktori relics) file tersebut akan dipecah menjadi sejumlah pecahan dengan ukuran maksimum tiap pecahan adalah 10kb.

File yang dipecah akan memiliki nama [namafile].000 dan seterusnya sesuai dengan jumlah pecahannya.
Ketika dilakukan penghapusan, maka semua pecahannya juga ikut terhapus.

Direktori report adalah direktori yang akan dibagikan menggunakan Samba File Server. Setelah kalian berhasil membuat direktori [nama_bebas], jalankan FUSE dan salin semua isi direktori [nama_bebas] pada direktori report.

# Penyelesaian

## Soal 1
Pertama, tentunya kita define fuse version berapa yang akan gita gunakan. Lalu, kita masukkan library yang akan kita pakai. Tidak  lupa, kita declare max length untuk memory allocation juga.
```c
#define FUSE_USE_VERSION 31 // Disini kita memakai fuse ver 31 karena lebih stabil dan lancar.

#include <fuse.h> // Fuse operations
#include <stdio.h> // Input-Output standar
#include <stdlib.h> // Alokasi memori, konversi string menjadi angka
#include <string.h> // Manipulasi string
#include <unistd.h> // pengelolaan proses, direktori, dan akses ke sistem
#include <sys/types.h> // data types
#include <sys/stat.h> // menunggu child process selesai
#include <dirent.h> // directory handling
#include <errno.h> // error handling

#define PATH_MAX_LENGTH 512  // define max length untuk paths agar memory allocation aman dan tidak overflow pada buffernya.
```
Lalu kita masukkan beberapa fungsi void. Seperti pada fungsi memberi watermark dan memindahkan file yang telahj di watermark ke folder wm, menghandle folder gallery, reverse text file, dan seterusnya.

```c
//====================================================================================================//
//    void digunakan dalam fungsi dibawah untuk specify bahwa fungsi tsb tidak perlu return value.    //
//====================================================================================================//

void watermark_and_move(const char *img_path); // FUNGSI add watermark dan memindahkan gambar
void handle_gallery_folder(); // FUNGSI handle files di folder "gallery"
void reverse_test_files(); // FUNGSI reverse content dari files dengan prefix "test"

// FUNGSI execute script.sh
void execute_script();
```
Setelah memberi fungsi void sebelumnya, kita lanjutkan dengan membuat fungsi untuk memberi watermark pada folder gallery yang berada pada direktori portofolio, yang akan di pindah ke folder wm.
```c
//====================================================================//
// FUNGSI BUAT WATERMARK DI SETIAP IMAGES PADA DIREKTORI PORTOFOLIO   //
//====================================================================//

void watermark_and_move(const char *img_path) { // FUNGSI add watermark ke foto lalu dimasukkan ke folder "wm"
    char cmd[1024]; // buffer untuk shell command
    char dest_path[PATH_MAX_LENGTH]; // buffer untuk destination path

    // create folder "gallery/wm" kalau belum ada di directory
    mkdir("gallery/wm", 0777);

    // construct destination path buat store foto yang telah di WM di "gallery/wm"
    snprintf(dest_path, sizeof(dest_path), "gallery/wm/%s", strrchr(img_path, '/') + 1);

    // command untuk add WM dan mindahin image-nya
    snprintf(cmd, sizeof(cmd), "convert \"%s\" -gravity south -pointsize 40 -fill white -draw \"text 0,0 'inikaryakita.id'\" \"%s\" && mv \"%s\" \"%s\"",
        img_path, img_path, img_path, dest_path);
             
    // execute watermarking and moving command
    system(cmd);
}

// FUNGSI untuk handle files di folder "gallery"
void handle_gallery_folder() {
    DIR *directory;  // pointer ke directory (seketika keingat strukdat '-')
    struct dirent *entry;  // structure buat store directory entry information 

    // open direktori "gallery" directory
    if ((directory = opendir("gallery")) != NULL) {
        
        // read setiap entry di directory
        while ((entry = readdir(directory)) != NULL) {
            
            // check file extension buat ensure file itu image atau bukan
            if (strstr(entry->d_name, ".jpeg") || strstr(entry->d_name, ".jpg") || strstr(entry->d_name, ".png") ||
                strstr(entry->d_name, ".JPEG") || strstr(entry->d_name, ".JPG") || strstr(entry->d_name, ".PNG")) {
                    
                char source_path[PATH_MAX_LENGTH];  // buffer untuk store source path
                
                // combine directory name dengan entry name to form source path
                snprintf(source_path, sizeof(source_path), "gallery/%s", entry->d_name);

                // add watermark terus move file ke "wm" folder
                watermark_and_move(source_path);
                printf("File %s moved to the 'wm' folder with watermark successfully. Yay! :D\n", entry->d_name);
            }
        }
        
        // close directory after processing
        closedir(directory);
    } else {
        // display error message kalau directory gabisa di open
        perror("Could not open gallery directory, hiks :(");
    }
}
```
Sesuai soal, setelah kita berhasil memberi watermark pada setiap foto, kita akan me-reverse text file yang ada prefixt "test"nya. Command dibawah ini akan membuka direktori "bahaya" dan mengecek setiap filenya, dan apabila menemukan file yang ada prefix "test"nya, akan direverse.
```c
//====================================================================//
// FUNGSI BUAT REVERSE TEXTFILE/CONTENT YANG ADA PREFIX "TEST"NYA     //
//====================================================================//

void reverse_test_files() { // FUNGSI reverse content-nya files dengan prefix "test" terus save jadi file baru
    struct dirent *entry;  // structure untuk store directory entry information
    DIR *dir_ptr = opendir("bahaya");  // open directory "bahaya"

    // check apakah directory sudah successfully opened
    if (dir_ptr == NULL) {
        perror("opendir: bahaya");  // display error message kalau gabisa dibuka
        return;
    }

    // read setiap entry di directory tadi (dir bahaya)
    while ((entry = readdir(dir_ptr))) {
        
        // check apakah entry tersebut merupakan regular file dan punya prefix "test"
        if (entry->d_type == DT_REG && strncmp(entry->d_name, "test", 4) == 0) {
            
            char src_path[PATH_MAX_LENGTH];  // buffer untuk source file path
            
            // construct source file pathnya
            snprintf(src_path, sizeof(src_path), "bahaya/%s", entry->d_name);
            
            char out_path[PATH_MAX_LENGTH];  // buffer for output file path
            
            // construct output file path with "reversed_" prefix
            snprintf(out_path, sizeof(out_path), "bahaya/reversed_%s", entry->d_name);

            // open source file in read mode, jadi entar di read sama programnya dulu
            FILE *src_file = fopen(src_path, "r");
            
            // open output file in write mode, supaya bisa di edit filenya sama programnya
            FILE *out_file = fopen(out_path, "w");  

            // check apakah file sudah successfully opened
            if (src_file == NULL || out_file == NULL) {
                perror("fopen");  // display error message
                continue;
            }

            // dapetin size filenyya
            fseek(src_file, 0, SEEK_END); // move file pointer to end
            long file_size = ftell(src_file); // get the current position of the file pointer
            fseek(src_file, 0, SEEK_SET); // move file pointer back to start

            // alokasi memory buat file content
            char *content = malloc(file_size + 1);  
            fread(content, 1, file_size, src_file);
            content[file_size] = '\0';  // add null terminator

            // reverse the content and write to the output file
            for (long i = file_size - 1; i >= 0; i--) {
                fputc(content[i], out_file);
            }

            // free allocated memory lalu close the files
            free(content);
            fclose(src_file);
            fclose(out_file);
        }
    }

    // close directory after processing selesai
    closedir(dir_ptr);
}
```
Setelah kedua fungsi sebelumnya bekerja, kita akan run program script.sh dengan cara berikut.
```c
// FUNGSI execute script.sh
void execute_script() {
    // run script.sh
    system("./bahaya/script.sh");
}
```
Fungsi fuse kemudian kita implementasikan, dengan cara berikut. Kode dibawah ini akan memastikan bahwa permissions-nya sudah aman, dan dapat dijalankan.
```c
//============================//
// FUSE FUNCTIONS IMPLEMENTATION //
//============================//

static const char *base_path = "/";  // Base path for the filesystem

static int myfs_getattr(const char *path, struct stat *stbuf) {
    int res;
    char full_path[PATH_MAX_LENGTH];
    snprintf(full_path, sizeof(full_path), "root%s", path);  // Assuming 'root' is the root directory

    res = lstat(full_path, stbuf);
    if (res == -1)
        return -errno;

    return 0;
}

static int myfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;
    char full_path[PATH_MAX_LENGTH];
    snprintf(full_path, sizeof(full_path), "root%s", path);  // Assuming 'root' is the root directory

    dp = opendir(full_path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

static int myfs_open(const char *path, struct fuse_file_info *fi) {
    int res;
    char full_path[PATH_MAX_LENGTH];
    snprintf(full_path, sizeof(full_path), "root%s", path);  // Assuming 'root' is the root directory

    res = open(full_path, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

static int myfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd;
    int res;
    char full_path[PATH_MAX_LENGTH];
    snprintf(full_path, sizeof(full_path), "root%s", path);  // Assuming 'root' is the root directory

    fd = open(full_path, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

// FUSE operations structure
static struct fuse_operations myfs_oper = {
    .getattr = myfs_getattr,
    .readdir = myfs_readdir,
    .open    = myfs_open,
    .read    = myfs_read,
};
```
Terakhir, kita buat fungsi main yang akan execute setiap command yang ada pada kode program ini.
```c
//===================================//
//  FUNGSI MAIN + EKSEKUSI SCRIPT.SH  //
//===================================//

int main(int argc, char *argv[]) {

    // handle folder "gallery" folder untuk add watermark ke images
    handle_gallery_folder();

    // change permissions dari script.sh supaya bisa remove folder "gallery"
    if (chmod("bahaya/script.sh", 0755) < 0) {
        perror("chmod: script.sh");  // display error message
    }

    // handle files di folder "bahaya" buat reverse content dari files dengan prefix "test"
    reverse_test_files();

    // execute script.sh
    execute_script();

    // remove folder "gallery" dan "bahaya" folders bersama isi-isinya (Sesuai script.sh)
    system("rm -rf gallery bahaya");

    // Set up and run FUSE filesystem
    return fuse_main(argc, argv, &myfs_oper, NULL);
}
```

Berikut adalah cara untuk menjalankan programnya:
Pertama, compile program dengan cara dibawah:
```c
gcc -o pakefuse inikaryakita.c -D_FILE_OFFSET_BITS=64 -lfuse
```
Kedua, mount program fuse tersebut ke dalam sebuah mount point.
```c
sudo ./pakefuse ~/fuse_mount
```
Ketiga, terminal akan menampilkan hasil output sebagai berikut:
![Screenshot from 2024-05-24 22-54-09](https://github.com/ch0clat/Sisop-4-2024-MH-IT18/assets/151893499/40aeb1f4-2538-40bb-ba65-3aaf8d65e726)

`Gambar sebelum ada watermark:`

![Screenshot from 2024-05-20 17-43-11](https://github.com/ch0clat/Sisop-4-2024-MH-IT18/assets/151893499/972b6d08-8dd4-402e-8d4f-c9b6b1d7223d)

`Hasil gambar setelah ada watermark:`

![Screenshot from 2024-05-20 17-47-20](https://github.com/ch0clat/Sisop-4-2024-MH-IT18/assets/151893499/a7db0040-85ff-4602-abcf-1f49d29c94a6)

`Text sebelum di reverse:`

![Screenshot from 2024-05-20 17-44-08](https://github.com/ch0clat/Sisop-4-2024-MH-IT18/assets/151893499/bf4ed802-4405-4df7-a986-829a02e54007)

`Hasil text setelah di reverse:`

![Screenshot from 2024-05-20 17-48-08](https://github.com/ch0clat/Sisop-4-2024-MH-IT18/assets/151893499/03dc6a4c-4bf3-4fe3-a4fa-a9a5d5503666)

## Soal 2
pastibisa.c
```c
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
```
Program ini akan melakukan decoding melalui function - function yang telah di buat di atas. Function ini akan di panggil jika file mempunyai nama yang sesuai dengan dengan prefix yang sudah di tentukan. Case dia atas akan di panggil oleh `If` di bawah ini 

```c
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

    if (decoded_content[0] != '\0') {
        strncpy(buf, decoded_content, size);
    }

    log_message("SUCCESS", "read", fpath);
    close(fd);
    return res;
}
```
`res = pread(fd, buf, size, offset);` akan membaca file yang di buka dan menyimpan tulisan yang terbaca pada `buf`. file path akan di simpan pada `fpath` dan jika pada path tersebut terdapat prefix decode yang tertentu `buf` akan di decode melalui functions yang sudah terdefine di atas.  `if (decoded_content[0] != '\0')` akan melakukan pengecekan apakah decoded_content mempunyai isi atau tidak. jika decoded_content ini terisi `buf` akan di ganti dengan decoded content menggunakan `strncpy(buf, decoded_content, size);`. setelah itu `log_message("SUCCESS", "read", fpath);` akan melakukan loging.

```c
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
```
function ini akan melakukan logging jika function di panggil dari function lain. `FILE *fp = fopen("/home/ubuntu-sisop/sisop/logs-fuse.log", "a");` akan membukan file atau membuat file baru jika file tersebut tidak di temukan. `fprintf(fp, "[%s]::%s::[%s]::[%s]\n", status, timestamp, tag, additional_info);` akan menambahkan line baru sesuai dengan message yang di berikan dari function call dan mengisi waktu sesuai `strftime(timestamp, 80, "%d/%m/%Y-%H:%M:%S", info);`. 

untuk menggunakan kode ini compile yang harus digunakan adalah sebagi berikut :

`gcc -o pastibisa pastibisa.c -lssl -lcrypto -D_FILE_OFFSET_BITS=64`

dan untuk melakukan mounting :

`./pastibisa /tujuan/direktori/mount`

### SCREENSHOTS 

Sebelum decode :

![image](https://github.com/ch0clat/Sisop-4-2024-MH-IT18/assets/128571877/9014aa2a-4341-4b0d-a276-258305f094b5)


Sesudah decode :

![image](https://github.com/ch0clat/Sisop-4-2024-MH-IT18/assets/128571877/cf88b4e0-b02d-41ce-97d5-acd6c0c19b1a)

###REVISI 

<ul>
<li>folder "Rahasia-berkas" belum bisa di lock dengan password saat membuka folder tersebut. pada program ini password hanya bisa di input pada saat mounting.</li>
</ul>











