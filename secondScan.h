/*
 * =========================================================================================
 * name        : Assembler/secondScan.h
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#ifndef SECONDSCAN_H_
#define SECONDSCAN_H_

/* This method scans the assembly file for the second time and converts each instruction to its memory words
 * Input: fp - a pointer to the start of the assembly file
 *        IC - the instructions counter
 * Output: 0 - compiling failed.
 *         1 - success compiling.
 */

int SecondScan(FILE *fp, int IC);

#endif