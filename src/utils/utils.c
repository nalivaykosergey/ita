#include "utils.h"

#include <stdio.h>
#include <stdlib.h>


#define ITA_DEBUG_LOG

#ifdef ITA_DEBUG_LOG
static inline void print_row(const char * str) {
    printf("%-15s",str);
}
#define ITA_DEBUG_PRINTF(...) printf(__VA_ARGS__)
#define ITA_DEBUG_PRINT_ROW(x) print_row((x))
#else
#define ITA_DEBUG_PRINTF(...) ;
#define ITA_DEBUG_PRINT_ROW(x) ;
#endif /* ITA_DEBUG_LOG */


const static size_t MIN_ARRAY_SIZE = 10;
static struct ita_main_t ita_main = {
        .sbs = NULL
};

/// USEFUL STUFF
static inline int is_bad_char(char ch) {
    return (ch < 65) || (ch > 91 && ch < 97) || (ch > 123);
}
static ITA_STATUS get_word_from_line (struct ita_word_t * word, const char * line) {
    struct ita_main_t * im = &ita_main;
    ITA_STATUS ret = ITA_OK;
    const uint8_t * stem;

    if (sscanf(line, "%s %d\n", word->word, &word->paradigm_idx) <= 0) {
        ret = ITA_BAD_LINE;
        goto done;
    }
    word->word_length = strlen(word->word);

    stem = sb_stemmer_stem(im->sbs, word->word, word->word_length);
    if (!stem) {
        ret = ITA_UNDEFINED_STEM;
        goto done;
    }

    word->stem_length = sb_stemmer_length(im->sbs);
    memcpy(word->stem, stem, word->stem_length);
done:
    return ret;
}

ITA_STATUS ita_init() {
    struct ita_main_t * im = &ita_main;
    im->sbs = sb_stemmer_new("eng", "ISO_8859_1");
    return ITA_OK;
}

ITA_STATUS ita_parse_words_from_csv(struct ita_word_t ** words, size_t * wlength, const char * filename) {
    struct ita_main_t * im = &ita_main;
    ITA_STATUS ret = ITA_OK;
    FILE * pfile = NULL;
    struct ita_word_t *pwords = NULL;
    char * line = NULL;
    size_t line_length;
    size_t iw_max_length = MIN_ARRAY_SIZE;
    size_t ci = 0; // current idx for array
    
    if (!im->sbs) {
        ret = ITA_UNINITED;
        goto done;
    }
    
    // READ PARSED FILE
    pfile = fopen(filename, "r");
    if (!pfile) {
        ret = ITA_FILE_NOT_FOUND;
        goto done;
    }

    // ALLOCATE DATA FOR WORDS
    pwords = (struct ita_word_t *) malloc (sizeof(struct ita_word_t) * (iw_max_length + 1));
    if (!words) {
        ret = ITA_BAD_ALLOCATION;
        goto resource_free;
    }

    // READ WORDS AND FILL ITA WORD STRUCTURE
    *wlength = ci;
    while (getline(&line, &line_length, pfile) != -1) {
        if (ci >= iw_max_length) {
            iw_max_length *= 2;
            pwords = (struct ita_word_t *)realloc(pwords, sizeof(struct ita_word_t) * (iw_max_length + 1));
            if (!pwords) {
                *wlength = -1;
                ret = ITA_BAD_ALLOCATION;
                goto resource_free;
            }
        }
        get_word_from_line(&pwords[ci], line);
        ++ci;
    }
    *wlength = ci;
    *words = pwords;
resource_free:
    if (line != NULL)
        free(line);
    fclose(pfile);
done:
    return ret;
}


ITA_STATUS ita_free_words(struct ita_word_t * words) {
    free(words);
    return ITA_OK;
}
ITA_STATUS ita_deinit() {
    struct ita_main_t * im = &ita_main;
    sb_stemmer_delete(im->sbs);
    return ITA_OK;
}

double ita_perf_calculation(struct ita_word_t * words, size_t words_length, ITA_PERF_INDICATOR indicator) {
    int noe = 0; // noe = num of elements
    int noe_with_diff_stem = 0;
    double result;

    if (words_length <= 0)
        return -1;

    if (indicator > ITA_SW)
        return -1;

    if (indicator == ITA_SW) {
        double oi = ita_perf_calculation(words, words_length, ITA_OI);
        double ui = ita_perf_calculation(words, words_length, ITA_UI);
        double res = oi / ui;

        ITA_DEBUG_PRINTF("SW = OI/UI = %f\n", res);
        return res;
    }

    if (indicator == ITA_UI) {
        ITA_DEBUG_PRINTF("\nDEBUGLOG: Current Table for Understemming Index (UI)\n");
    } else {
        ITA_DEBUG_PRINTF("\nDEBUGLOG: Current Table for Overstemming Index (OI)\n");
    }

    ITA_DEBUG_PRINT_ROW("");
    for (size_t i = 0; i < words_length; ++i) {
        ITA_DEBUG_PRINT_ROW(words[i].word);
    }
    ITA_DEBUG_PRINTF("\n");

    for (size_t i = 0; i < words_length; ++i) {
        struct ita_word_t * word1 = &words[i];
        ITA_DEBUG_PRINT_ROW(word1->word);
        for (size_t j = 0; j < words_length; ++j) {
            struct ita_word_t * word2 = &words[j];
            if (word1 == word2) {
                ITA_DEBUG_PRINT_ROW("<>");
                continue;
            }
            /// CASE UI
            if (indicator == ITA_UI) {
                if (word1->paradigm_idx != word2->paradigm_idx) {
                    ITA_DEBUG_PRINT_ROW("X");
                    continue;
                }
                if (strcmp(word1->stem, word2->stem) == 0) {
                    ++noe_with_diff_stem;
                    ITA_DEBUG_PRINT_ROW("+");
                }
                else {

                    ITA_DEBUG_PRINT_ROW("-");
                }
            //// CASE OI
            } else {
                if (word1->paradigm_idx == word2->paradigm_idx) {
                    ITA_DEBUG_PRINT_ROW("X");
                    continue;
                }
                if (strcmp(word1->stem, word2->stem) != 0) {
                    ++noe_with_diff_stem;
                    ITA_DEBUG_PRINT_ROW("+");
                }
                else {

                    ITA_DEBUG_PRINT_ROW("-");
                }
            }
            ++noe;
        }
        ITA_DEBUG_PRINTF("\n");
    }
    result = 1 - (double )noe_with_diff_stem / noe;

    if (indicator == ITA_UI) {
        ITA_DEBUG_PRINTF("UI = ");
    } else {
        ITA_DEBUG_PRINTF("OI = ");
    }

    ITA_DEBUG_PRINTF("1 - %d / %d = %f\n", noe_with_diff_stem, noe, result);

    return result;
}

static ITA_STATUS parse_file_and_add_to_sets(const char * filename, SimpleSet * words, SimpleSet * stems) {
    struct ita_main_t * im = &ita_main;
    FILE  * pfile;
    char buffer[64];
    size_t str_len;
    ITA_STATUS ret = ITA_OK;
    int status;

    // READ PARSED FILE
    pfile = fopen(filename, "r");
    if (!pfile) {
        ret = ITA_FILE_NOT_FOUND;
        goto done;
    }

    while (fscanf(pfile, " %63s", buffer) == 1) {
        char * bptr = buffer;
        str_len = strlen(bptr);

        while (str_len > 0 && is_bad_char(bptr[str_len - 1])) {
            bptr[str_len - 1] = '\0';
            --str_len;
        }
        if (str_len <= 0)
            continue;

        while (str_len > 0 && is_bad_char(bptr[0])) {
            ++bptr;
            --str_len;
        }
        if (str_len <= 0)
            continue;


        if (str_len < 6 || str_len >= 64)
            continue;

        status = set_add(words, bptr);
        if (status == 0) {
            const uint8_t * stem = sb_stemmer_stem(im->sbs, bptr, str_len);
            if (stem)
                set_add(stems, stem);
        }
    }
    fclose(pfile);
done:
    return ret;
}


static void ita_perf_calculation_text(const char * filename, size_t * N, size_t * S) {
    SimpleSet words;
    SimpleSet stems;

    set_init(&words);
    set_init(&stems);

    parse_file_and_add_to_sets(filename, &words, &stems);

    *N = set_length(&words);
    *S = set_length(&stems);

    set_destroy(&words);
    set_destroy(&stems);
}

double ita_mwc_calculation(const char * filename) {
    size_t N, S;
    ita_perf_calculation_text(filename, &N, &S);
    return (double)N / S;
}
double ita_icf_calculation(const char * filename) {
    size_t N, S;
    ita_perf_calculation_text(filename, &N, &S);
    return ((double)N - S) / N;
}
