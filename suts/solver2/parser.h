#ifndef _PARSER_
#define _PARSER_

#include <ctype.h>
#include <errno.h>
#include "vector.h"
#include "var.h"

#define MAX_NUMBER_LENGTH 10
#define MAX_NUMBER_LENGTH_C "10"

void badFormatted(char *message, char *details);

int isSeparator(int character);

void checkStartSequence(FILE *inputFile);

bool makeOutput(FILE *inputFile, V output);

int readHeaderParameter(FILE *inputFile);

void readUntilSpace(FILE *inputFile, int *lastReadChar, char *readNumberChars);

int readNumberUntilSpace(FILE *inputFile, int *lastReadChar);

V PARSEformula(FILE *inputFile);

FILE *PARSERinit(char *path);

void PARSEheader(FILE *inputFile);

#endif
