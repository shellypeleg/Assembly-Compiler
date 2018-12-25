/*
 * =========================================================================================
 * name        : Assembler/dataTable.c
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dataTable.h"
#include "instructions.h"
#include "outputFiles.h"

#define QUOTES '\"'

#define MAX_NUM 511 /*the max number that can be represented by 10 bits*/
#define MIN_NUM -512 /*the min number that can be represented by 10 bits*/

int dataTable[DATA_ARR_SIZE]; /*An array to hold all the data - numbers and characters*/
int DC = 0; /*data counter*/


/*---------------------------------------------------------------*/
/*Get the current Data Counter (DC) number*/
int GetDC()
{
    return DC;
}
/*---------------------------------------------------------------*/

/*Set Data Counter (DC) value to zero*/
void ResetDC()
{
    DC = 0;
}
/*---------------------------------------------------------------*/

/*Save a number to the data table. If insertion was successful return 1, otherwise 0*/

int SaveNumToDataTable(char *str_num)
{
    int hasError = false; /*error flag*/

    /*check if data table exists*/
    if(dataTable != NULL)
    {   
        /*check if there's enough spcae to insert more data*/
        if(DC < DATA_ARR_SIZE)
        {
            int num; /*will hold the number*/
            char *end_ptr; /*will be the next character after the numerical value in str_num*/

            /*assign the numerical value from str_num to num*/
            num = (int)strtol(str_num, &end_ptr, 10); 

            /*check if the number is in the valid range*/
            if(num <= MAX_NUM && num >= MIN_NUM)
            {
                /*check if after the number there's a whitespace symbol. if not - save the number in the data table*/
                if((end_ptr!=NULL && end_ptr!= str_num && IsRowEmpty(end_ptr)) || end_ptr == NULL)
                {
                    dataTable[DC] = num;
                    DC++;
                }
                
                else /*the given string is probably not a number*/
                {
                    hasError = true;
                    fprintf(stderr,"Error: unresolved parameter: %s. Expecting integer.\n",str_num);
                }
            }

            else /*number is not in range*/
            {
                hasError = true;
                fprintf(stderr, "Error: the number %s is not between %d - %d.\n",str_num, MAX_NUM,MIN_NUM);
            }
        }

        else /*no more space in the data table*/
        {
            hasError = true;
            fprintf(stderr, "Error: no more space in the data table, can't add more values.\n");
        }
    }

    else /*data table error*/
        hasError = true;

    return !hasError;
}
/*---------------------------------------------------------------*/

/*Save a string to the data table. If insertion was successful return 1, otherwise 0*/

int SaveStringToDataTable(char *str)
{
    int hasError = false; /*Error flag*/

    /*check if data table exists*/
    if(dataTable != NULL)
    {
        int stringEnd = strlen(str) - 1;

        /*remove the \n from the end*/
        if(str[stringEnd] == '\n')
            str[stringEnd] = '\0';

        /*Check if the string starts and ends with quotation marks*/
        if(str[0] == QUOTES && str[strlen(str)-1] == QUOTES)
        {
            /*do as long as we didn't reach the second quotation marks or data table is not full*/
            for(str++; *str!='"' && DC<DATA_ARR_SIZE; str++, DC++)
            {
                dataTable[DC] = *str;
            }

            dataTable[DC] = '\0'; /*the last character of the string should be null character*/
            DC++;

            /*if data table is full*/
            if(DC==DATA_ARR_SIZE)
            {
                hasError = true;
                fprintf(stderr,"Error: no more space in the data table, can't add more values.\n");
            }
        }

        else /*Invalid string - doesn't start and ends with a quotation marks*/
        {
            hasError = true;
            fprintf(stderr, "Error: unresolved parameter: %s. Expecting a string.\n",str);
        }
    }

   return !hasError; 
}

/*---------------------------------------------------------------*/

/*This method writes the data table adresses and data to the object output file
 *Input - the most current IC address */
void WriteDataToFile(int address)
{
    int i; /*indexing integer*/

    for(i = address; i<address+DC; i++)
        ObjectWriteLine(i,dataTable[i-address]);
}