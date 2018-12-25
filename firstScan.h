/*
 * =========================================================================================
 * name        : Assembler/firstScan.h
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#ifndef FIRSTSCAN_H_
#define FIRSTSCAN_H_

#include "common.h"

/*
    This function receives a label and checks if it is a saved word.
    The function returns 0 if the label is NOT a saved word.
    The function returns 1 if the label IS a saved word.
*/
int IsSavedWord(char *label);

/*---------------------------------------------------------------*/
/*This method checks if a given label is a valid label -
 * combines from only lettes and numbers and not reaches the length of 30*/

int IsValidLabel(char * label, int rowNumber);

/*---------------------------------------------------------------*/

/*A method for parsing Directive references: .data, .string, .struct, .extern, .entry
 * Input:
 *      directiveName   - .string / .data / .struct / .extern / .entry
 *      label           - label name
 *      IC              - instruction counter
 *      rowNumber       - the number of the row from which we got the directive reference
 * Output: 
 *      0 - Compiling failed.
 *      1 - Success compiling. 
 * */

bool DirectiveReference(char *directiveName, char *label, int *IC, int rowNumber);

/*---------------------------------------------------------------*/

/* This method scans the assembly file for the first time and counts how many instruction memory words each row takes
 * Input: fp - a pointer to the start of the assembly file
 *        IC - the instructions counter
 * Output: 0 - compiling failed.
 *         1 - success compiling.
 */

int FirstScan(FILE *fp, int *IC);


#endif