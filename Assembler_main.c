/*
*********************************************************************************************************************************

Name         : Assembler_main.c
Authors      : Lior Peleg (308541457) & Shelly Peleg (205417116)
Copyrights   : Lior Peleg & Shelly Peleg
Description  : Our project receives simple programs written in a basic assembly language (configured for this project only)
               and converts each row into a pattern of bits, simulating an assembler.
               The pattern of bits is presented in the output files in a strange 32 base letters:
               ! @ # $ % ^ & * < > a b c d e f g h i j k l m n o p q r s t u v
               Each row in the output files contains the address & encoding for the assembly instructions, in this 32 base.

*********************************************************************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "assemblerManager.h"

int main(int argc, char *argv[]) /* argv- list of file names*/
{
    char **filesList = argv; /*pointer to the first argument in argv*/

    filesList++; /*The first item recieved in argv is the program name*/

    if(*filesList == NULL)
    {
        fprintf (stderr,"Error: Compiler must get at least one file to compile!\n");
        exit(1);
    }
    
    else
        AssemblyCompiler(filesList);

    return 0;
}