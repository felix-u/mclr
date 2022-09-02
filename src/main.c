#include <stdlib.h>
#include <stdio.h>
#include <sysexits.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "args.c"
#include "colour.c"

#define STB_DS_IMPLEMENTATION
#include "../libs/stb_ds-v0.67/stb_ds.h"


const char *HELP_TEXT =
"    -h, --help                Display this help information and exit.\n"
"    -i, --input <FILE>        Supply input file.\n"
"    -o, --output <FILE>       Supply output location.\n"
"    -d, --dither <STR>        Specify dithering algorithm, or \"none\" to disable\n"
"                                  (default is \"floyd-steinberg\").\n"
"    -s, --swap                Invert image brightness, preserving hue and\n"
"                              saturation.\n"
"    -p, --palette <STR>...    Supply palette as whitespace-separated colours.\n";


int main(int argc, char **argv) {

    // Help flag

    char **help_arg = (char *[]){"-h", "--help"};
    BoolFlagReturn help = args_isPresent(argc, argv, help_arg);
    if (help.is_present) {
        printf("%s\n", HELP_TEXT);
        return(EXIT_SUCCESS);
    }


    // Required arguments

    char **input_arg = (char *[]){"-i", "--input"};
    char *input_path = args_singleValueOf(argc, argv, input_arg);
    if (input_path == NULL) {
        printf("ERROR: Must provide path to input file.\n");
        exit(EX_USAGE);
    }

    char **output_arg = (char *[]){"-o", "--output"};
    char *output_path = args_singleValueOf(argc, argv, output_arg);
    if (output_path == NULL) {
        printf("ERROR: Must provide output location.\n");
        exit(EX_USAGE);
    }

    char **palette_arg = (char *[]){"-p", "--palette"};
    MultipleValReturn palette_return =
        args_multipleValuesOf(argc, argv, palette_arg);
    if (palette_return.offset == 0 || palette_return.end == 0 ||
        palette_return.end - palette_return.offset < 2)
    {
        printf("ERROR: Must provide at least two (2) palette colours.\n");
        exit(EX_USAGE);
    }


    // Optional arguments

    char **dither_arg = (char *[]){"-d", "--dither"};
    BoolFlagReturn dither_is_present = args_isPresent(argc, argv, dither_arg);
    char *dither_alg = NULL;
    if (dither_is_present.is_present) {
        dither_alg = args_singleValueOf(argc, argv, dither_arg);
        if (dither_alg == NULL) {
            printf("ERROR: `dither` flag requires argument.\n");
            exit(EX_USAGE);
        }
    }

    char **swap_arg = (char *[]){"-s", "--swap"};
    BoolFlagReturn swap = args_isPresent(argc, argv, swap_arg);


    // Convert palette hex strings to an array of RGB

    int palette_len = palette_return.end - palette_return.offset;
    RGB palette[palette_len];

    for (int i = 0; i < palette_len; i++) {
        RGBCheck rgb_get = hexStrToRGB(argv[i + palette_return.offset]);
        if (rgb_get.valid == true) {
            RGB rgb_put = {rgb_get.r, rgb_get.g, rgb_get.b};
            palette[i] = rgb_put;
        }
        else {
            printf("ERROR: \"%s\" is not a valid hex colour.\n",
                   argv[i + palette_return.offset]);
            exit(EX_USAGE);
        }
    }


    return EXIT_SUCCESS;
}
