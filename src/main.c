#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils/utils.h"


int main(int argc, char ** argv) {

    size_t l;
    struct ita_word_t * words = NULL;
    char * FILE_PATH1;
    char * FILE_PATH2;

    if (argc == 3) {
        FILE_PATH1 = argv[1];
        FILE_PATH2 = argv[2];
    } else {
        fprintf(stderr, "file name does not exist\n");
        exit(~0);
    }

    ita_init();

    /// TASK 1
    printf("Task 1:\n");
    ita_parse_words_from_csv(&words, &l, FILE_PATH1);

    for (size_t i = 0; i < l; ++i) {
        printf("(class=%u)word: %s (len = %zu), stem: %s (len = %zu)\n",
               words[i].paradigm_idx,
               words[i].word,  words[i].word_length,
               words[i].stem, words[i].stem_length);
    }

    double n3 = ita_perf_calculation(words, l, ITA_SW);
    ita_free_words(words);



    /// TASK 2
    printf("\n==================================\n");
    printf("Task 2:\n");
    printf("MWC = %f\n", ita_mwc_calculation(FILE_PATH2));
    printf("ICF = %f\n", ita_icf_calculation(FILE_PATH2));

    ita_deinit();

    return 0;
}