#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define TEXT_SIZE 200000  // Note, the longer the text the more likely you will get a good 'decode' from the start.
#define ALEN 26         // Number of chars in ENGLISH alphabet
#define CHFREQ "ETAONRISHDLFCMUGYPWBVKJXQZ" // Characters in order of appearance in English documents.
#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

/* Program developed for NWEN243, Victoria University of Wellington
Author: Kris Bubendorfer, this extended version (c) 2015 & Raphael Namay 2018
LAB: 2

This program applies a basic frequency analysis on a cyphertext.  It has been extened over the 2014 version to
solve polyalphabetic cyphers - by brute force.  In this case, it applies the frequency analysis for different
numbers of n keys (polyalphabetic Caeser).  Obviously it will need a cypher of about n times
the typical length for a monoalphabetic cypher.

Program is used like this:

Compile:  gcc -o crack crack.c

Test file (ctext): JWRLS, XSSH PZK JH HES BJFV, UZU (this is not a realistic length piece of cypher text)

crack n

Argument:

n number of keys to try

---

% cat ctext | crack 1
ALICE, MEET YOU AT THE PARK, BOB   <-- of course it won't be this correct.  Don't worry about that for the -d option.
AMFDE, UEET LNH AT TIE RASC, ONO   <-- this is what it really looks like, a larger sample is better, this is short.


*/

char upcase(char ch){
  if(islower(ch))
  ch -= 'a' - 'A';
  return ch;
}


char* fixtext(char* s){
  int i, j;
  char plain[TEXT_SIZE]; // assume key < length of alphabet, local array on stack, will go away!

  for(i = 0, j = 0; i < strlen(s); i++){
    if(isalpha(s[i])){
      plain[j++] = upcase(s[i]);
    }
  }
  plain[j] = '\0';
  return strcpy(s, plain);
}

void swap(char *xp, char *yp) //taken from Internet
{
    char temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void bubbleSort(char arr[], int n) //taken from Internet
{
   int i, j;
   for (i = 0; i < n-1; i++)

       // Last i elements are already in place
       for (j = 0; j < n-i-1; j++)
           if (arr[j] > arr[j+1])
              swap(&arr[j], &arr[j+1]);
}

int analyzeFrequencies(char subtext[]) { //partially taken from Internet
  int array[255] = {0}; // initialize all elements to 0

int i, max, index;

for(i = 0; subtext[i] != 0; i++)
{
  ++array[subtext[i]];
}

bubbleSort(subtext, strlen(subtext));

// Find the letter that was used the most
max = array[0];
index = 0;
for(i = 0; subtext[i] != 0; i++)
{
    if( array[subtext[i]] > max)
    {
        max = array[subtext[i]];
        index = i;
    }
}


printf("The most frequent character is: %c \n", subtext[index]);
printf("Which gives a shift of: %ld\n", strchr(CHFREQ, subtext[index]) - CHFREQ);
return (strchr(CHFREQ, subtext[index]) - CHFREQ);
//return subtext[index];
}

int main(int argc, char **argv){

  // first allocate some space for our input text (we will read from stdin).

  char* text = (char*)malloc(sizeof(char)*TEXT_SIZE+1);
  char ch;
  int n, i;

  if(argc > 1 && (n = atoi(argv[1])) > 0); else{ fprintf(stderr,"Malformed argument, use: crack [n], n > 0\n"); exit(-1);} // get the command line argument n

  // Now read TEXT_SIZE or feof worth of characters (whichever is smaller) and convert to uppercase as we do it.
  // Added: changed to count frequencies as we read it in

  for(i = 0, ch = fgetc(stdin); i < TEXT_SIZE && !feof(stdin); i++, ch = fgetc(stdin)){
    text[i] = (ch = (isalpha(ch)?upcase(ch):ch));
  }
  text[i] = '\0'; // terminate the string properly.

  /* At this point we have two things,
  *   1. The input cyphertext in "text"
  *   2. The maximum number of keys to try (n) - we'll be trying 1..n keys.
  *
  * What you need to do is as follows:
  *   1. create a for-loop that will check key lengths from 1..n
  *   2. for each i <= n, spit the cypher text into i sub-texts.  For i = 1, 1 subtext, for i = 2, 2 subtexts, of alternating characters etc.
  *   3. for each subtext:
  *          a. count the occurance of each letter
  *          b. then map this onto the CHFREQ, to create a map between the sub-text and english
  *          c. apply the new map to the subtext
  *   4. merge the subtexts
  *   5. output the 'possibly' partially decoded text to stdout.  This will only look OK if i was the correct number of keys
  *
  * what you need to output (sample will be provided) - exactly:
  * i maps -> stderr
  * i 'possible' translations
  *
  * You would be wise to make seperate functions that perform various sub-tasks, and test them incrementally.  Any other approach will likely
  * make your brain revolt.  This isn't a long program, mine is 160 lines, with comments (and written in a very verbose style) - if yours is
  * getting too long, double check you're on the right track.
  *
  */

  // Your code here...
  char keyArray[n];
  int keyArrayIndex = 0;

  //printf("original text: %s\n", text);
  char* text2[TEXT_SIZE];
  strcpy(text2, text);
  char* fixedText = fixtext(text2);
  //printf("tidied text: %s\n\n", fixedText);
  int subtextIndex = 0;
  char *subtext;

  int index, j;

  for (index = 0; index < n; index++) {
    printf("Subtext %d: ", index+1);
    subtext = calloc(strlen(fixedText)/n+1, sizeof(char));
    subtextIndex = 0;

    for (j = index; j < strlen(fixedText); j+=n) {
      subtext[subtextIndex++] = fixedText[j];
    }

    subtext[subtextIndex] = '\0';
    printf("%s\n", subtext);
    keyArray[keyArrayIndex++] = analyzeFrequencies(subtext);
    free(subtext);
    printf("\n");

  }

  for (keyArrayIndex = 0; keyArrayIndex < n; keyArrayIndex++) { //for each shift in keyArray
int idx;
int count = 0;
for (idx = keyArrayIndex; idx < strlen(text); idx+=n) {
  if (text[idx] >= 'A' && text[idx] <= 'Z') {
    while (count < keyArray[keyArrayIndex]) {
      if (text[idx] == 'A') {
        text[idx] = ('Z' + 1);
      }
  text[idx]--;
  count++;
}
}
}


}

printf("%s", text);

}
