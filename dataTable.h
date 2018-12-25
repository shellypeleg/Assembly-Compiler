/*
 * =========================================================================================
 * name        : Assembler/dataTable.h
 * author      : Lior Peleg (308541457), Shelly Peleg (205417116)
 * =========================================================================================
 */

#ifndef DATATABLE_H_
#define DATATABLE_H_

#define DATA_ARR_SIZE 500 /*max size for data table rows*/

/*---------------------------------------------------------------*/
/*Get the current Data Counter (DC) number*/
int GetDC();

/*---------------------------------------------------------------*/

/*Set Data Counter (DC) value to zero*/
void ResetDC();

/*---------------------------------------------------------------*/

/*Save a number to the data table. If insertion was successful return 1, otherwise 0*/

int SaveNumToDataTable(char *str_num);

/*---------------------------------------------------------------*/

/*Save a string to the data table. If insertion was successful return 1, otherwise 0*/

int SaveStringToDataTable(char *str);

/*---------------------------------------------------------------*/

/*This method writes the data table adresses and data to the object output file
 *Input - the most current IC address */
void WriteDataToFile(int address);



#endif