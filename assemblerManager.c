/*
 * =========================================================================================
 * name        : Assembler/assemblerManager.c
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "firstScan.h"
#include "secondScan.h"
#include "outputFiles.h"
#include "symbolTable.h"
#include "dataTable.h"
#include "instructions.h"

#define BASE_CODE_ADDRESS 100

int IC = 0; /*Instructions counter*/

/*---------------------------------------------------------------*/

/*For getting the input file name, without the folder path before, if exists*/
void GetOutputFilesName(char *file) 
{
    char *index = strrchr(file, '/'); /*pointer to the last presence of '/' in the file's name*/

    if(!index)
        index = file; /*only the name was received without a folder path*/
    else
        index++; /*point to the start of the file name*/

    SetFilesName(index);
}

/*---------------------------------------------------------------*/

/*  This function gets a list of assembly files and compiles them
 *  Returns: 1 - The compiling was successfull
 *           0 - Compiling failed, an error was recieved during the compiling process  */

void AssemblyCompiler(char **filesList)
{
    bool hasError = false; /*Error flag*/
    char fname[MAX_FILENAME_LENGTH + FILENAME_EXT_LENGTH]; /*Final file name, including the extension*/
    FILE *ifp;
    
    /*Do on all the input assembly files*/
    while (*filesList != NULL)
    {
        hasError = false;
        IC = BASE_CODE_ADDRESS; 
        ResetDC();
        InitializeSymbolTable();

        /*Open file in read mode*/
        strcpy(fname, *filesList);
        GetOutputFilesName(fname);
        strcat(fname, asmExt);
        ifp = fopen(fname, "r");
        
        /*If opening failed*/
        if(ifp == NULL)
        {
            fprintf(stderr, "Error: Unable to open the file: %s.\nExiting the program.\n", fname);
            exit(1);
        }

        fprintf(stdout,"---------------------------------------------------------\n");
        fprintf(stdout,"\t\tCompiling %s\t\t\n",fname);
        fprintf(stdout,"---------------------------------------------------------\n");

        /*First scan*/
        fprintf(stdout,"\n--First Scan--\n\n");
        if(FirstScan(ifp, &IC)) /*do only if the first scan was successful*/
        {   
            
            UpdateSymbolTable(IC); /*update the table with the correct addresses*/
            rewind(ifp); /*points back to the start of the file*/
            
            /*Second scan*/
            fprintf(stdout,"\n--Second Scan--\n\n");
            if(SecondScan(ifp, BASE_CODE_ADDRESS)) /*do only if the second scan was successful*/
            {   
                ObjectWriteLine(IC, GetDC()); /*first line in .ob file is __IC last count__DC last count__ */
                WriteEncodingsToFile(BASE_CODE_ADDRESS, IC); /*write the rest encoded instructions to the output files*/
                WriteDataToFile(IC); /*write the rest of the encoded data to the output files*/
            }

            else /*second scan failed*/
            {
                fprintf(stderr, "\nError: An error was found in the second scan.\n");
                hasError = true;
            }
        }

        else /*first scan failed*/
        {
            fprintf(stderr, "\nError: An error was found in the first scan.\n");
            hasError = true;
        }

        CloseOpenFiles();
        FreeSymbolTable();
        
        /*If an error was found during first/second scan - delete output files*/
        if(hasError)
        {
            fprintf(stderr, "\nNote: Error found in the input assembly file %s, therefore deleting its output files.\n",fname);
            DeleteOutputFiles();
            fprintf(stderr,"\n*********************************************************************\n");
            fprintf(stderr,"Summary: Critical Error! Cannot compile the given file - %s.\n",fname);
            fprintf(stderr,"*********************************************************************\n");
        }
        else
        {
            fprintf(stdout,"\n\t****COMPILING %s IS DONE.****\t\t\n\n",fname);
        }

        filesList++; /*go to the next input file*/
    }

}


