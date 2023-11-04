#ifndef ITA_UTILS_H
#define ITA_UTILS_H

#include <stdint.h>
#include <string.h>
#include <libstemmer.h>
#include "set.h"

/* simple ita (intellectual text analyzing) library impl */

typedef enum ITA_STATUS {
    ITA_UNINITED,
    ITA_FILE_NOT_FOUND,
    ITA_BAD_ALLOCATION,
    ITA_BAD_LINE,
    ITA_UNDEFINED_STEM,
    ITA_OK
} ITA_STATUS;
typedef enum ITA_PERF_INDICATOR {
    ITA_OI = 0, // OVERSTEMMING
    ITA_UI, // UNDERSTEMMING
    ITA_SW //
} ITA_PERF_INDICATOR;

struct ita_main_t {
    struct sb_stemmer *sbs;
};

struct ita_word_t {
    uint32_t paradigm_idx; // unique word group identifier
    uint8_t word[64]; // real word from the sample
    uint8_t stem[64]; // stemmed part of that word
    size_t word_length;
    size_t stem_length;
};

/* MAIN INIT FUNCTION. MUST BE CALLED FIRST */
ITA_STATUS ita_init();
ITA_STATUS ita_deinit();


/* Get words from CSV file and make ita words
 * file format: word paradigm_idx
 * returns ITA_OK if no errors
 * */
ITA_STATUS ita_parse_words_from_csv(struct ita_word_t ** words, size_t * wlength, const char * filename);

/* After all words calculations you need to deallocate words
 * */
ITA_STATUS ita_free_words(struct ita_word_t * words);


/* UI/OI/SW calculation
*/
double ita_perf_calculation(struct ita_word_t * words, size_t words_length, ITA_PERF_INDICATOR indicator);

double ita_mwc_calculation(const char * filename);
double ita_icf_calculation(const char * filename);


#endif /* ITA_UTILS_H */
