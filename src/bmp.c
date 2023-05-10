#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

uint32_t *open_bmp(char *path) {
    // check if file exists
    FILE *file = fopen(path, "rb");
    if (!file) {
        printf("File %s not found\n", path);
        return NULL;
    }
    
    // open file
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    uint8_t *file_content = malloc(file_size);
    fread(file_content, 1, file_size, file);
    fclose(file);

    // check if file is a bmp
    if (file_content[0] != 'B' || file_content[1] != 'M') {
        free(file_content);
        printf("File %s is not a bmp\n", path);
        return NULL;
    }

    // get image data
    int width = *(int *)(file_content + 18);
    int height = *(int *)(file_content + 22);
    int offset = *(int *)(file_content + 10);
    int size = *(int *)(file_content + 34);
    uint8_t *data = file_content + offset;

    if (width <= 0 || height <= 0) {
        free(file_content);
        printf("File %s has invalid dimensions\n", path);
        return NULL;
    }

    int factor = size / (width * height);

    // copy image data to buffer
    if (factor != 3 && factor != 4) {
        free(file_content);
        printf("File %s has invalid pixel format\n", path);
        return NULL;        
    }

    uint32_t *output = malloc(width * height * sizeof(uint32_t));

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) { 
            uint32_t color = data[(j * width + i) * factor] |
                            (data[(j * width + i) * factor + 1] << 8) |
                            (data[(j * width + i) * factor + 2] << 16);

            if (factor == 4 && data[(j * width + i) * factor + 3] == 0) continue;
            output[width * height - (i + j * width + 1)] = color;
        }
    }

    free(file_content);
    return output;
}
