/******************************************************
 * modexp.c
 * Author: Tyler Johnston
 * Purpose: Interactive program to calculate solutions
 *          to modular exponentiation problems.
 * Last revision: April 15, 2015
*******************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ARG_SIZE (long)pow(2,(8*sizeof(long)-1))-1

/* Text color macros */
#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define BOLDRED "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define YELLOW  "\x1b[33m"

/* Data type used to store info about a base-10 number
 * and it's binary format */
typedef struct DecBinNumber {
    long num;
    int length;
    int *binNumArr;
    int binNumArrLength;
} DecBinNumber;

void modexp();
int length(long x);
DecBinNumber *createDecBinNumber(long num);
int *convertNumToBinArr(long num, int length);
long calcModExp(long num, long exp, long modNum);
void printDirections(void);
void printLicense(void);
long safeScanModExpArg(void);
int checkModExpNum(long num);
int isPossibleLongSquareOverflow(long x);
void printFeedback(long val);
int isNum(char *buffer);
char *trimwhitespace(char *str);

/* Main */
int main(void)
{
    printLicense();
    printDirections();
    modexp();
    return 0;
}

/** Function: modexp()
 *  Purpose:  Provides an interactive interface for solving
 *            modular exponentiation problems.
*/
void modexp()
{
    long num, exp, modNum;
    while(1) {
        while(1) {
            printf("Enter your a: ");
            num = safeScanModExpArg();
            if (checkModExpNum(num)) {
                if (isPossibleLongSquareOverflow(num)) {
                    printf(BOLDRED "[ERROR]: Cannot compute solution with a = %ld as it\nwould result in multiplicative overflow.\n" RESET, num);
                    printf(BOLDRED "Try using a smaller number for a such that a^2 does not exceed\n%ld.\n" RESET, MAX_ARG_SIZE);
                    continue;
                } else {
                    break;
                }
            }
            printFeedback(num);
        }
        while(1) {
            printf("Enter your k: ");
            exp = safeScanModExpArg();
            if (checkModExpNum(exp))
                break;
            printFeedback(exp);
        }
        while(1) {
            printf("Enter your n: ");
            modNum = safeScanModExpArg();
            if (checkModExpNum(modNum)) {
                if (isPossibleLongSquareOverflow(modNum)) {
                    printf(BOLDRED "[ERROR]: Cannot compute solution with n = %ld as it\nwould result in multiplicative overflow.\n" RESET, num);
                    printf(BOLDRED "Try using a smaller number for n such that n^2 does not exceed\n%ld.\n" RESET, MAX_ARG_SIZE);
                    continue;
                }
                if (modNum == 0) {
                    printf(BOLDRED "[ERROR]: n cannot be 0! Retry using a valid integer.\n" RESET);
                    continue;
                }
                break;
            }
            printFeedback(modNum);
        }
        printf("\na = %ld, k = %ld, n = %ld\n\n", num, exp, modNum);
        printf("\nSolving x ≡ (%ld^%ld) mod %ld...\n\n",
               num, exp, modNum);
        printf(BOLDGREEN"\nThe answer is x = %ld.\n\n" RESET,
               calcModExp(num,exp,modNum));
    }
}

/** Function: calcModExp(long num, long exp, long modNum)
 *  Purpose: Calculates x in the following equation:
 *           x ≡ (a^k) mod n.
 *  Notes: Implements the chart algorithm discussed in MATH 4190,
 *         Spring 2015 at Clemson University. Also has the side
 *         of printing the corresponding chart.
*/
long calcModExp(long num, long exp, long modNum)
{
    int i;
    long x = 0, prevPower = num, currPower = prevPower;
    DecBinNumber *number = createDecBinNumber(exp);
    char buffer[200];
    printf("i\tb_i\tpower=%ld^(2^i) mod %ld    x\n",num,modNum);
    sprintf(buffer,"power=%ld^(2^i) mod %ld", num, modNum);
    for (i = 0; i < number->binNumArrLength; i++) {
        if (i > 0)
            currPower = (prevPower * prevPower) % modNum;
        if (number->binNumArr[i] == 1) {
            if (i == 0) {
                x = prevPower % modNum;
            } else {
                if (x == 0)
                    x = currPower;
                else
                    x = (x * currPower) % modNum;
            }
        }
        if (i > 0)
            prevPower = currPower;
        if (x != 0) {
            printf("%d\t%d\t%ld%*ld\n", i, number->binNumArr[i],
                   currPower, (int)strlen(buffer) + 4, x);
        } else {
            printf("%d\t%d\t%ld%*c\n", i, number->binNumArr[i],
                   currPower, (int)strlen(buffer) + 4, '-');
        }
    }
    free(number->binNumArr);
    number->binNumArr = NULL;
    free(number);
    number = NULL;
    return x;
}

/** Function: createDecBinNumber(long num)
 *  Purpose:  Creates a new DecBinNumber * object given a number,
 *            designated by num.
*/
DecBinNumber *createDecBinNumber(long num)
{
    int l = length(num);
    int binNumArrLength = floor(log2(num)) + 1;
    DecBinNumber *new =
        (DecBinNumber*)malloc(sizeof(DecBinNumber));
    new->num = num;
    new->length = l;
    new->binNumArr = convertNumToBinArr(num,binNumArrLength);
    new->binNumArrLength = binNumArrLength;
    return new;
}

/** Function: convertNumToBinArr(int num, int length)
 *  Purpose: Converts a base-10 number designated by num into
 *           its binary format in the form of an array.
 *  Notes: The first element of the array is the least significant
 *         bit of the binary number, the last element of the
 *         array is the most significant bit of the binary number.
*/
int *convertNumToBinArr(long num, int length)
{
    int i, *binArr = (int *)malloc(sizeof(int)*length);
    long n = num, d = 2, q, r;
    for (i = 0; i < length; i++) {
        q = n / d;
        r = n % d;
        *(binArr + i) = r;
        n = q;
    }
    return binArr;
}

/** Function: length(int x)
 *  Purpose: Calculates the length of a number designated by x.
*/
int length(long x) {
    if (x < 10) {
        return 1;
    } else {
        return 1 + length(x/10);
    }
}

/*******Below are functions that are not important in solving
 *******modular exponentiation problems, and are more involved
 *******in the fomatting, feedback, and exception handling of the
 *******program. Study if you want to gain more insight about the
 *******how the program handles I/O and exception handling.
*/
long safeScanModExpArg(void)
{
    long inputError = -2, lengthError = -3, scanError = -4, invalidIntError = -5;
    long x = inputError;
    int maxTotalLength = length(MAX_ARG_SIZE) - 1;
    char buffer[2048], buffer2[2048];
    if (fgets(buffer2, sizeof(buffer), stdin) != NULL) {
        strcpy(buffer,trimwhitespace(buffer2));
        if (strcmp(buffer,"-1") == 0)
            exit(0);
        if (!isNum(buffer))
            return inputError;
        if (sscanf(buffer, "%ld", &x) == 1) {
            if (strlen(buffer)-1 > maxTotalLength) {
                return lengthError;
            }
            return (x > -2) ? x : invalidIntError;
        } else {
            return inputError;
        }
    } else {
        return scanError;
    }
}

int isNum(char *buffer)
{
    int i;
    int length = strlen(buffer);
    for (i = 0; i < length; i++) {
        if (!isdigit(buffer[i]))
            return 0;
    }
    return 1;
}

int checkModExpNum(long num)
{
    if (num > -1)
        return 1;
    else if (num < -1)
        return 0;
    else
        exit(0);
}

int isPossibleLongSquareOverflow(long x)
{
    long test = x * x;
    if (x != 0 && test/x != x) {
        return 1;
    }
    return 0;
}

void printDirections()
{
    printf("============================[DIRECTIONS]============================\n");
    printf("HOW TO USE: Enter the respective non-negative integer\nnumbers to the equation: x ≡ (a^k) mod n in the\nprompts below.\n\n");
    printf("*** TO QUIT: enter -1 at any of the prompts. ***\n");
    printf("\nSee \"readme.txt\" for additinal information about this program.\n");
    printf("====================================================================\n\n");
}

void printLicense()
{
    printf("Modular Exponentiation Calculator (Version 1):\nCalculates solutions to modular exponentiation problems of\nthe form: x ≡ (a^k) mod n.\n");
    printf("Copyright (C) 2016 Tyler Johnston.\n\n");
    printf("This program is free software; you can redistribute it\nand/or modify it under the terms of the GNU General Public\nLicense as published by the Free Software Foundation; either\nversion 3 of the License, or (at your option) any later version.\n");
    printf("This program is distributed in the hope that it will be\nuseful, but WITHOUT ANY WARRANTY; without even the implied\nwarranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\nSee the GNU General Public License for more details.\n\n");
}

void printFeedback(long val)
{
    int maxLength = length(MAX_ARG_SIZE) - 1;
    if (val == -3) {
        printf( BOLDRED "[ERROR]: Entered integer is too large.\nTry again using an integer with length no longer than %d.\n" RESET, maxLength);
    } else if (val == -4) {
        printf(BOLDRED "[ERROR]: Scan Error! Catastrophic!!! Exiting program!\n" RESET);
    } else if (val == -5) {
        printf(BOLDRED "[ERROR]: Entered integer is negative.\nTry again using a valid non-negative integer.\n" RESET);
    } else {
        printf(BOLDRED "[ERROR]: Entered input is invalid.\nTry again using a valid non-negative integer.\n" RESET);
    }
}

char *trimwhitespace(char *str)
{
    char *end;
    while(isspace(*str)) str++;
    if(*str == 0)
        return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;
    *(end+1) = 0;
    return str;
}
