/* spacecode.c */

/**
 *
 * @author __MadHatter (alias used on https://www.reddit.com/r/dailyprogrammer)
 *
 * http://www.reddit.com/r/dailyprogrammer/comments/38fjll/20150603_challenge_217_intermediate_space_code/
 *
 */
 
/*
  Encryption and Planet Systems:
  
  Omicron V: will take and invert the 5th bit. ( 0001 0000) That is the bit location in the byte where we invert the bit.
  Hoth: Takes the value of the ASCII character and adds 10 to it.
  Ryza IV: Takes the value of the ASCII character and subtracts 1 to it.
  Htrae: reverses the characters.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LEN 128
#define INT_LEN 8
#define WORD_LIST "words.txt"

int   binarySearch     (char **dictionary, char *key, int min, int max);
void  clearString      (char *s, int size);
long  getNumberOfLines (const char *filename);
int   isMessageEnglish (char **dictionary, long n_words, char *message);
int   isWordFound      (char **dictionary, long n_words, char *word);
int   nextInt          (char *s, int *startIndex);
char* nextString       (char *s, int *startIndex);
char* toLower          (char *s);

char* decryptOmnicronV (char *message);
char* decryptHoth      (char *message);
char* decryptRyzaIV    (char *message);
char* decryptHtrae     (char *message);

int main (void)
{
  int i;
  int value;
  int index;
  int formatIndex;
  int numberOfMessages;
  long numberOfWords;
  FILE *fp;
  char line[BUF_LEN];
  char formatted[BUF_LEN];
  char **dictionary;
  char *decryptedMessage;
  char *message;
  char *messages[] = {
    " 71 117  48 115 127 125 117  48 121 126  48  96 117 113 115 117 ",
    " 97 111  42 109 121 119 111  42 115 120  42 122 111 107 109 111 ",
    " 86 100  31  98 110 108 100  31 104 109  31 111 100  96  98 100 ",
    " 101  99  97 101 112  32 110 105  32 101 109 111  99  32 101  87 ",
    " 84 113 121 124 105  48  64  98 127 119  98 113 125 125 117  98  48 121  99  48  99  96 105 121 126 119  48 127 126  48 101  99 ",
    " 78 107 115 118 -125  42  90 124 121 113 124 107 119 119 111 124  42 115 125  42 125 122 -125 115 120 113  42 121 120  42 127 125 ",
    " 67  96 104 107 120  31  79 113 110 102 113  96 108 108 100 113  31 104 114  31 114 111 120 104 109 102  31 110 109  31 116 114 ",
    " 115 117  32 110 111  32 103 110 105 121 112 115  32 115 105  32 114 101 109 109  97 114 103 111 114  80  32 121 108 105  97  68 ",
    " 86 121  98 117  48 100 120 117  48  93 121  99  99 124 117  99 ",
    " 80 115 124 111  42 126 114 111  42  87 115 125 125 118 111 125 ",
    " 69 104 113 100  31 115 103 100  31  76 104 114 114 107 100 114 ",
    " 115 101 108 115 115 105  77  32 101 104 116  32 101 114 105  70 "
  };
  
  numberOfMessages = 12;
  
  /* Initialize dictionary. */
  numberOfWords = getNumberOfLines (WORD_LIST);
  dictionary = (char **) malloc (sizeof (char *) * (unsigned int) numberOfWords);
  for (i = 0; i < numberOfWords; i++) {
    dictionary[i] = (char *) malloc (sizeof (char) * BUF_LEN + 1);
    clearString (dictionary[i], BUF_LEN);
  }
  
  /* Read words from dictionary. */
  fp = fopen (WORD_LIST, "r");
  i = 0;
  while (fp != 0
      && fgets (line, sizeof (line), fp) != 0
      && ferror (fp) == 0
      && feof (fp) == 0) {
    if ((int) strlen (line) > 1) {
      line[strlen (line) - 1] = '\0'; /* remove nextline char */
      strcat (dictionary[i], line);
      i++;
    }
  }
    
  /* Loop through messages. */
  for (i = 0; i < numberOfMessages; i++) {
    message = messages[i];
    clearString (formatted, BUF_LEN);
    formatIndex = 0;
    index = 1;
    
    /* Parse message into integers and convert into characters. */
    while (index < (int) strlen (message)) {
      value = nextInt (message, &index);
      if (value != 0) {
        formatted[formatIndex++] = (char) value;
      }
    }
        
    /* Decrypt message and determine origin. */    
    if (isMessageEnglish (dictionary, numberOfWords, decryptedMessage = decryptOmnicronV (formatted)) != 0) {
      printf ("%-10s : %s\n", "Omnicron V", decryptedMessage);
    }
    else if (isMessageEnglish (dictionary, numberOfWords, decryptedMessage = decryptHoth (formatted)) != 0) {
      printf ("%-10s : %s\n", "Hoth", decryptedMessage);
    }
    else if (isMessageEnglish (dictionary, numberOfWords, decryptedMessage = decryptRyzaIV (formatted)) != 0) {
      printf ("%-10s : %s\n", "Ryza IV", decryptedMessage);
    }
    else if (isMessageEnglish (dictionary, numberOfWords, decryptedMessage = decryptHtrae (formatted)) != 0) {
      printf ("%-10s : %s\n", "Htrae", decryptedMessage);
    }
    else {
      printf ("[%02d] %s : %s\n", i, "**UNKNOWN**", message);
    }
  }
  
  return 0;
}

/* 
 * Binary Search Algorithm: Deferred detection of equality
 * http://en.wikipedia.org/wiki/Binary_search_algorithm
 */
int
binarySearch (char **dictionary, char *key, int min, int max)
{
  int mid;
  
  while (min < max) {
    mid = (max + min) / 2;
    if (strcmp (dictionary[mid], key) < 0) {
      min = mid + 1;
    }
    else {
      max = mid;
    }
  }
  
  if ((max == min) && (strcmp (dictionary[min], key) == 0)) {
    return min;
  }
  else {
    return -1;
  }
}

void
clearString (char *s, int size)
{
  int i;
  for (i = 0; i < size; i++) {
    s[i] = '\0';
  }
}

long
getNumberOfLines (const char *filename)
{
  long n;
  char line[BUF_LEN];
  FILE *fp;
  
  n = 0;
  fp = fopen (filename, "r");
  while (fp != 0
      && fgets (line, sizeof (line), fp) != 0
      && ferror (fp) == 0
      && feof (fp) == 0) {
    if ((int) strlen (line) > 1) {
      n++;
    }
  }
  
  return n;
}

/*
 * Message is considered English if at least 30% of the words 
 * are found in the American-English dictionary... just like in America :D
 */
int
isMessageEnglish (char **dictionary, long n_words, char *message)
{
  int index;
  int len;
  int numberOfWordsFound;
  int numberOfWords;
  float match;
  char *nextWord;
  
  len = (int) strlen (message);
  index = 0;
  numberOfWordsFound = 0;
  numberOfWords = 0;
  while (index < len) {
    nextWord = nextString (message, &index);
    if (isWordFound (dictionary, n_words, toLower (nextWord)) != 0) {
      numberOfWordsFound++;
    }
    numberOfWords++;
  }
  
  match = (float) ((float) numberOfWordsFound / (float) numberOfWords);
  
  return ((match >= (float) 0.3) ? 1 : 0);
}

int
isWordFound (char **dictionary, long n_words, char *word)
{
  if (binarySearch (dictionary, word, 0, (int) (n_words - 1)) >= 0) {
    return 1;
  }
  return 0;
}

int
nextInt (char *s, int *startIndex)
{
  int i;
  int index;
  char c;
  char value[INT_LEN];
  
  /* Initialize value to hold integer characters. */
  clearString (value, INT_LEN);
  
  /* Read characters until a space is reached. */
  index = 0;
  while ((c = s[(*startIndex)++]) != ' ') {
    value[index++] = c;
  }
  
  /* Convert characters to integer. */
  i = (int) strtol (value, 0, 10);
  
  /* Update starting index. */
  startIndex++;
  
  /* Return converted integer. */
  return i;
}

char*
nextString (char *s, int *startIndex)
{
  int index;
  int len;
  char c;
  char word[BUF_LEN];
  char *newWord;
   
  /* Read characters until a space is reached. */
  clearString (word, BUF_LEN);
  index = 0;
  while ((c = s[(*startIndex)++]) != ' ' && c != '\0') {
    word[index++] = c;
  }
  
  /* Update starting index. */
  startIndex++;
  
  /* Create new word. */
  len = (int) strlen (word);
  newWord = (char *) malloc (sizeof (char) * (unsigned int) (len + 1));
  for (index = 0; index < len; index++) {
    newWord[index] = word[index];
  }
  newWord[index] = '\0';
  
  /* Return next word. */
  return newWord;
}

char*
toLower (char *s)
{
  int i;
  int len;
  int lower;
  char *lowerS;
  
  len = (int) strlen (s);
  lowerS = (char *) malloc (sizeof (char) * (unsigned int) (len + 1));
  
  for (i = 0; i < len; i++) {
    lower = (int) s[i];
    lower = tolower (lower);
    lowerS[i] = (char) lower;
  }
  lowerS[i] = '\0';
  
  return lowerS;
}

char*
decryptOmnicronV (char *message)
{
  int i;
  int len;
  char c;
  char bit;
  char *decryptedMessage;
  
  len = (int) strlen (message);
  bit = (char) 16;
  decryptedMessage = (char *) malloc (sizeof (char) * (unsigned int) (len + 1));
  
  for (i = 0; i < len; i++) {
    c = message[i];
    c = c ^ bit;
    decryptedMessage[i] = c;
  }
  decryptedMessage[i] = '\0';
  
  return decryptedMessage;
}

char*
decryptHoth (char *message)
{
  int i;
  int len;
  int value;
  char c;
  char *decryptedMessage;
  
  len = (int) strlen (message);
  decryptedMessage = (char *) malloc (sizeof (char) * (unsigned int) (len + 1));
  
  for (i = 0; i < len; i++) {
    c = message[i];
    value = (int) c;
    value -= 10;
    c = (char) value;
    decryptedMessage[i] = c;
  }
  decryptedMessage[i] = '\0';
  
  return decryptedMessage;
}

char*
decryptRyzaIV (char *message)
{
  int i;
  int len;
  int value;
  char c;
  char *decryptedMessage;
  
  len = (int) strlen (message);
  decryptedMessage = (char *) malloc (sizeof (char) * (unsigned int) (len + 1));
  
  for (i = 0; i < len; i++) {
    c = message[i];
    value = (int) c;
    value++;
    c = (char) value;
    decryptedMessage[i] = c;
  }
  decryptedMessage[i] = '\0';
  
  return decryptedMessage;
}

char*
decryptHtrae (char *message)
{
  int i;
  int j;
  int len;
  char c;
  char *decryptedMessage;
  
  len = (int) strlen (message);
  decryptedMessage = (char *) malloc (sizeof (char) * (unsigned int) (len + 1));
  
  i = (int) (len - 1);
  j = 0;
  while (i >= 0) {
    c = message[i--];
    decryptedMessage[j++] = c;
  }
  decryptedMessage[j] = '\0';
  
  return decryptedMessage;
}
