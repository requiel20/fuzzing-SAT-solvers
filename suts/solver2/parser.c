#include "parser.h"
#include "debugPrinter.h"

void badFormatted(char *message, char *details) {
    printf("ERROR: the file is badly formatted. %s%s\n", message, details);
    exit(EXIT_FAILURE);
}

int isSeparator(int character) {
    if (character == '\n')
        return 1;
    if (character == '\t')
        return 1;
    if (isspace(character))
        return 1;
    if (character == '\r')
        return 1;
    if (character == EOF)
        return 1;
    return 0;
}

void skipLine(FILE *inputFile) {
    int readChar;
    do {
        readChar = fgetc(inputFile);
    } while (readChar != '\n' && readChar != EOF);
    printDebug("Skipped a comment\n");
}

void checkStartSequence(FILE *inputFile) {
    char *fileStart = "p cnf ";
    int readChar;

    do {
        readChar = fgetc(inputFile);
        if (readChar == (int) *fileStart) {
            fileStart++;
        } else {
            if (readChar == 'c') {
                readChar = fgetc(inputFile);
                if (isSeparator(readChar)) {
                    if (readChar != '\n')
                        skipLine(inputFile);
                    fileStart = "p cnf ";
                    continue;
                } else {
                    badFormatted("The file must start with", "p cnf ");
                }
            } else {
                badFormatted("The file must start with", "p cnf ");
            }
        }
    } while (*fileStart);
}

void readUntilSpace(FILE *inputFile, int *lastReadChar, char *readNumberChars) {
    int i = 0;
    int readChar;
    do {
        readChar = fgetc(inputFile);
        if (readChar == 'c')
            skipLine(inputFile);
        else
            *(readNumberChars + i) = (char) readChar;
        i++;
    } while (i < MAX_NUMBER_LENGTH && !isSeparator(readChar));

    //while(isSeparator(readChar, inputFile)) {
    //    readChar = fgetc(inputFile);
    //}

    *lastReadChar = readChar;
    
    if (i >= MAX_NUMBER_LENGTH)
        badFormatted("This parser only accepts numbers of length in characters up to: ", MAX_NUMBER_LENGTH_C);

}

int readNumberUntilSpace(FILE *inputFile, int *lastReadChar) {
    int readNumber;
    char *readNumberChars = malloc(MAX_NUMBER_LENGTH * sizeof(char));

    readUntilSpace(inputFile, lastReadChar, readNumberChars);

    if(*lastReadChar == 13) {
        fgetc(inputFile);
    }

    readNumber = strtol(readNumberChars, NULL, 0);
    printDebugInt("Read number was: ", readNumber); 
   
    /* Will fail at this point if
     *    strtol failed
     *    strtol did not attempt (this would not trigger a strtol fail)
     * */
    if (readNumber == 0 && (errno == EINVAL || errno == ERANGE || !isdigit(readNumberChars[0])))
        badFormatted("Expected a number, read: ", readNumberChars);

    free(readNumberChars);
    return readNumber;
}

bool makeOutput(FILE *inputFile, V output) {
    V literals;
    Var tempVar;
    C tempClause = NULL;
    int lastReadChar = 0;
    int readInt;
    unsigned int maxLitEncountered = 0;

    for (unsigned int i = 0; i < numberOfClauses; i++) {
        literals = VECTORinit();
        do {
            readInt = readNumberUntilSpace(inputFile, &lastReadChar);
            printDebugInt("read  ", readInt);
            if (abs(readInt) < 0 || (unsigned int) abs(readInt) > numberOfLiterals) {
                badFormatted("not a valid literal", "");
            }

            if (readInt != 0) {
                tempVar = VARinit((unsigned int) abs(readInt), readInt > 0);
                VECTORadd(literals, tempVar);
                if (maxLitEncountered < tempVar->id)
                    maxLitEncountered = tempVar->id;
                printDebug("added\n");
            }

            if (!isSeparator(lastReadChar)) {
                badFormatted("", "");
            } else if (lastReadChar == EOF) {
                if (i < numberOfClauses - 1) {
                    badFormatted("Could not find enough clauses", "");
                } else {
                    break;
                }
            }

        } while (readInt != 0);
        printDebugInt("creating clause of length: ", VECTORtotal(literals));
        if (CLAUSEnew(literals, false, &tempClause)) {
            if (tempClause != NULL) {
                printDebug("adding clause\n");
                VECTORadd(output, tempClause);
            } else {
                printDebug("unit clause enqueued, not adding\n");
            }
        } else {
            printDebug("Top level conflict detected by parser\n");
            return false;
        }

    }

    if (maxLitEncountered != numberOfLiterals)
        badFormatted("Not all the literals were encountered", "\n");

    if (lastReadChar != EOF)
        printDebugInt("WARNING: EOF not reached. Number of clauses parsed parsed correctly: ",
                      numberOfClauses);
    return true;
}

int readHeaderParameter(FILE *inputFile) {
    int lastReadChar;
    int parameter = readNumberUntilSpace(inputFile, &lastReadChar);
    if (!isSeparator(lastReadChar))
        badFormatted("", "");
    else if (lastReadChar == EOF)
        badFormatted("Could not find number of clauses parameter", "");
    return parameter;
}

FILE *PARSERinit(char *path) {
    FILE *inputFile;

    inputFile = fopen(path, "r");

    checkStartSequence(inputFile);

    return inputFile;
}

void PARSEheader(FILE *inputFile) {

    numberOfLiterals = readHeaderParameter(inputFile);

    numberOfClauses = readHeaderParameter(inputFile);
}

V PARSEformula(FILE *inputFile) {
    V output = VECTORinit();

    if (makeOutput(inputFile, output)) {

        printFormula(output);

        return output;
    } else {
        printDebug("Top level conflict detected by parser");
        return NULL;
    }
}
