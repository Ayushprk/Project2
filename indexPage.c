/* File: indexPage.c */
/* Author: Britton Wolfe */
/* Date: September 3rd, 2010 */

/* This program indexes a web page, printing out the counts of words on that page */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>  // For isalpha and tolower

#define ALPHABET_SIZE 26
#define BUFFER_SIZE 300001
#define MAX_WORD_LEN 256

typedef struct TrieNode {
  int count;
  struct TrieNode* children[ALPHABET_SIZE];
} TrieNode;

int indexPage(const char* url, TrieNode* root);
int addWordOccurrence(const char* word, const int wordLength, TrieNode* root);
void printTrieContents(TrieNode* root);
int freeTrieMemory(TrieNode* node);
int getText(const char* srcAddr, char* buffer, const int bufSize);

int main(int argc, char** argv){
  // TODO: Handle command-line argument
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <URL>\n", argv[0]);
    return 1;
  }

  // TODO: Allocate root node
  TrieNode* root = malloc(sizeof(TrieNode));
  if (!root) {
    fprintf(stderr, "ERROR: Memory allocation failed\n");
    return 1;
  }

  root->count = 0;
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    root->children[i] = NULL;
  }

  // TODO: Call indexPage
  if (indexPage(argv[1], root) != 0) {
    fprintf(stderr, "ERROR: Failed to index page.\n");
    freeTrieMemory(root);
    return 1;
  }

  // TODO: Print trie contents
  printTrieContents(root);

  // TODO: Free trie memory
  freeTrieMemory(root);

  return 0;
}

int indexPage(const char* url, TrieNode* root)
{
  // TODO: Retrieve content from URL using getText
  char buffer[BUFFER_SIZE];
  int bytesRead = getText(url, buffer, sizeof(buffer));

  if (bytesRead <= 0) {
    fprintf(stderr, "ERROR: failed to retrieve content from %s\n", url);
    return 1;
  }

  printf("%s\n", url);

  int i = 0;
  while (buffer[i] != '\0') {
    while (buffer[i] && !isalpha(buffer[i])) i++;

    char word[MAX_WORD_LEN];
    int len = 0;

    while (buffer[i] && isalpha(buffer[i]) && len < MAX_WORD_LEN - 1) {
      word[len++] = tolower(buffer[i++]);
    }

    if (len > 0) {
      word[len] = '\0';
      addWordOccurrence(word, len, root);
      printf("\t%s\n", word);
    }
  }

  return 0;
}

int addWordOccurrence(const char* word, const int wordLength, TrieNode* root)
{
  // TODO: Add a word occurrence to the trie
  TrieNode* current = root;
  for (int i = 0; i < wordLength; i++) {
    int idx = word[i] - 'a';
    if (idx < 0 || idx >= ALPHABET_SIZE) continue;

    if (current->children[idx] == NULL) {
      current->children[idx] = malloc(sizeof(TrieNode));
      if (!current->children[idx]) return 1;

      current->children[idx]->count = 0;
      for (int j = 0; j < ALPHABET_SIZE; j++) {
        current->children[idx]->children[j] = NULL;
      }
    }

    current = current->children[idx];
  }

  current->count++;
  return 0;
}

void printTrieContentsHelper(TrieNode* node, char* buffer, int depth)
{
  if (node->count > 0) {
    buffer[depth] = '\0';
    printf("%s: %d\n", buffer, node->count);
  }

  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (node->children[i]) {
      buffer[depth] = 'a' + i;
      printTrieContentsHelper(node->children[i], buffer, depth + 1);
    }
  }
}

void printTrieContents(TrieNode* root)
{
  // TODO: Set up buffer and call recursive helper
  char buffer[MAX_WORD_LEN];
  printTrieContentsHelper(root, buffer, 0);
}

int freeTrieMemory(TrieNode* node)
{
  // TODO: Recursively free all trie nodes
  if (!node) return 0;

  for (int i = 0; i < ALPHABET_SIZE; i++) {
    if (node->children[i]) {
      freeTrieMemory(node->children[i]);
    }
  }

  free(node);
  return 0;
}

int getText(const char* srcAddr, char* buffer, const int bufSize){
  FILE *pipe;
  int bytesRead;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

  pipe = popen(buffer, "r");
  if(pipe == NULL){
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n", buffer);
    return 0;
  }

  bytesRead = fread(buffer, sizeof(char), bufSize-1, pipe);
  buffer[bytesRead] = '\0';

  pclose(pipe);

  return bytesRead;
}
