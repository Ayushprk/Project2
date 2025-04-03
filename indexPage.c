/* File: indexPage.c */
/* Author: Britton Wolfe */
/* Date: September 3rd, 2010 */

/* This program indexes a web page, printing out the counts of words on that page */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h> // Added for isalpha and tolower

#define BUFFER_SIZE 8192 // Define BUFFER_SIZE
#define MAX_WORD_LEN 256 // Define MAX_WORD_LEN

/* TODO: structure definitions */
typedef struct TrieNode
{
  int count;
  struct TrieNode *children[26];
} TrieNode;

TrieNode *createTrieNode()
{
  TrieNode *node = malloc(sizeof(TrieNode));
  if (node)
  {
    node->count = 0;
    for (int i = 0; i < 26; i++)
    {
      node->children[i] = NULL;
    }
  }
  return node;
}

/* NOTE: int return values can be used to indicate errors (typically non-zero)
   or success (typically zero return value) */

int addWordOccurrence(const char *word, const int wordLength, TrieNode *root)
{
  TrieNode *current = root;
  for (int i = 0; i < wordLength; i++)
  {
    int idx = word[i] - 'a';
    if (current->children[idx] == NULL)
    {
      current->children[idx] = createTrieNode();
    }
    current = current->children[idx];
  }
  current->count++;
  return 0;
}

void printTrieContentsHelper(TrieNode *node, char *buffer, int depth)
{
  if (!node)
    return;
  if (node->count > 0)
  {
    buffer[depth] = '\0';
    printf("%s: %d\n", buffer, node->count);
  }
  for (int i = 0; i < 26; i++)
  {
    if (node->children[i])
    {
      buffer[depth] = 'a' + i;
      printTrieContentsHelper(node->children[i], buffer, depth + 1);
    }
  }
}

void printTrieContents(TrieNode *root)
{
  char buffer[MAX_WORD_LEN];
  printTrieContentsHelper(root, buffer, 0);
}

int freeTrieMemory(TrieNode *node)
{
  if (!node)
    return 0;
  for (int i = 0; i < 26; i++)
  {
    if (node->children[i])
    {
      freeTrieMemory(node->children[i]);
    }
  }
  free(node);
  return 0;
}

int getText(const char *srcAddr, char *buffer, const int bufSize);

int main(int argc, char **argv)
{
  /* TODO: write the (simple) main function

  /* argv[1] will be the URL to index, if argc > 1 */
  if (argc > 1)
  {
    indexPage(argv[1]);
  }
  else
  {
    fprintf(stderr, "Usage: %s <URL>\n", argv[0]);
  }
  return 0;
}

/* TODO: define the functions corresponding to the above prototypes */

void indexPage(const char *url)
{
  char buffer[BUFFER_SIZE];
  TrieNode *root = createTrieNode();
  char command[1024];
  snprintf(command, sizeof(command), "curl -s \"%s\" | python3 getText.py", url);

  FILE *pipe = popen(command, "r");
  if (!pipe)
  {
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n", command);
    return;
  }

  int bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE - 1, pipe);
  buffer[bytesRead] = '\0';
  pclose(pipe);

  printf("%s\n", url);
  int i = 0;
  while (buffer[i] != '\0')
  {
    while (buffer[i] && !isalpha(buffer[i]))
      i++;
    char word[MAX_WORD_LEN];
    int len = 0;
    while (buffer[i] && isalpha(buffer[i]) && len < MAX_WORD_LEN - 1)
    {
      word[len++] = tolower(buffer[i++]);
    }
    if (len > 0)
    {
      word[len] = '\0';
      addWordOccurrence(word, len, root);
    }
  }

  printTrieContents(root);
  freeTrieMemory(root);
}

/* You should not need to modify this function */
int getText(const char *srcAddr, char *buffer, const int bufSize)
{
  FILE *pipe;
  int bytesRead;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

  pipe = popen(buffer, "r");
  if (pipe == NULL)
  {
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n",
            buffer);
    return 0;
  }

  bytesRead = fread(buffer, sizeof(char), bufSize - 1, pipe);
  buffer[bytesRead] = '\0';

  pclose(pipe);

  return bytesRead;
}
