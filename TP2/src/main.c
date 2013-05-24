#include <stdio.h>
#include <string.h>

#include "lsb.h"
#include "common.h"
#include "file.h"

int main(int argc, char *argv[]) {

    // Quick & dirty params:
    //  in file
    //  carrier
    assert(argc > 3);

    size_t n = atoi(argv[1]);
    assert(n == 1 || n == 4);

    if (strcmp(argv[2], "e") == 0) {
        assert(argc == 5);
        return embed(n, argv[3], argv[4]);
    } else {
        assert(argc == 4);
        return extract(n, argv[3]);
    }
}

int extract(size_t n, const char* carrierName) {
    struct data* image = read_file(carrierName);
    struct data* output = malloc(sizeof(struct data));

    lsb_extract(image, output, n);

    size_t extractedSize = *((size_t*) output->bytes);

    char* extension = (char*) (output->bytes + sizeof(size_t) + extractedSize);
    char* filename = malloc(sizeof(char) * (strlen("extracted.") + strlen(extension) + 1));
    strcpy(filename, "extracted.");
    strcat(filename, extension);

    FILE* out = fopen(filename, "w");
    fwrite(output->bytes + sizeof(size_t), sizeof(unsigned char), extractedSize, out);
    fclose(out);

    free_data(image);
    free_data(output);

    return 0;
}


int embed(size_t n, const char* carrierName, const char* inputName) {

    struct data* image = read_file(carrierName);
    struct data* rawInput = read_file(inputName);

    char* extension = strrchr(inputName, '.');
    if (extension) {
        extension += 1;
    }
    prepare_data(rawInput, extension);

    size_t bitCapacity = lsb_bit_capacity(image->len, n);

    if (bitCapacity < rawInput->len * 8) {
        printf("The carrier is not big enough. It can only carry %u bytes.\n", bitCapacity / 8);

        free_data(image);
        free_data(rawInput);

        return 1;
    }

    lsb_embed(image, rawInput, n);

    FILE* out = fopen("out", "w");
    fwrite(image->bytes, sizeof(unsigned char), image->len, out);
    fclose(out);

    free_data(image);
    free_data(rawInput);

    return 0;
}

