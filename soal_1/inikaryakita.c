#include <dirent.h> // FUNGSI directory
#include <stdio.h> // FUNGSI standard I/O
#include <stdlib.h> // FUNGSI general utilities
#include <string.h> // FUNGSI string manipulation
#include <sys/stat.h> // FUNGSI file status
#include <sys/types.h> // FUNGSI system data types
#include <unistd.h> // FUNGSI POSIX API

#define PATH_MAX_LENGTH 512  // define max length untuk paths agar memory allocation aman dan tidak overflow pada buffernya.


    //====================================================================================================//
   //    void digunakan dalam fungsi dibawah untuk specify bahwa fungsi tsb tidak perlu return value.    //
  //====================================================================================================//
  
  

void watermark_and_move(const char *img_path); // FUNGSI add watermark dan memindahkan gambar
void handle_gallery_folder(); // FUNGSI handle files di folder "gallery"
void reverse_test_files(); // FUNGSI reverse content dari files dengan prefix "test"

// FUNGSI execute script.sh
void execute_script();


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
    if ((directory = opendir("")) != NULL) {
        
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

// FUNGSI execute script.sh
void execute_script() {
    // run script.sh
    system("./bahaya/script.sh");
}

                  //===================================//
                 //  FUNGSI MAIN + EKSEKUSI SCRIP.SH  //
                //===================================//
int main() {

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

    return 0;  
}
