/*
 * json_parser.c
 *
 *  Created on: Apr 14, 2018
 *      Author: Delengowski & Szymczak
 */

#include <stdint.h>
#include <string.h>
#include <math.h>
#include "json_parser.h"

char *key;
char *json;
char *mystring;
uint16_t number;


char* json_find_key(char *key, char *json)
{
    char* address_of_null_term = key;
    uint8_t length_of_key;
    uint8_t num_correct_chars = 0;
    uint8_t open_brace_flag = 0; /* used to indicate being inside a key's list item */
    uint8_t quote_counter = 0; /* used to ignore second quote of a key that is not a struct */
    char first_character;
    first_character = *json; /* if the first character is not a '[' then we must ignore all keys within them */
    while(*address_of_null_term != '\0')
    {
        address_of_null_term++; /* increase address until end */
    }
    length_of_key = address_of_null_term - key;
    while(num_correct_chars != length_of_key)
    {
        if(first_character != '[')
        {
            if(*json == '[')
            {
                open_brace_flag = 1;
            }
            if(*json == ']')
            {
                open_brace_flag = 0;
            }
        }
        if((*key == *json) && (open_brace_flag == 0))
        {
            num_correct_chars++;
            key++;
            json++;
        }
        else
        {
            json++;
            key = key - num_correct_chars; /* point back to beginning if characters stop matching*/
            num_correct_chars = 0;
        }
    }
    while((*json != '{') && (*json != '[') && !(*json != '"'))
    {
        quote_counter++;
        json++;
    }
    return json;
}

// WARNING: ONLY WORKS FOR LISTS OF OBJECTS
char* json_find_list_item(uint16_t number, char*json)
{
	uint16_t curly_bracket_counter = 0;
	while(curly_bracket_counter != number)
	{
		if(*json == '{')
		{
			curly_bracket_counter++;
		}else if(*json == ']') {
		    return 0;
		}
		json++;
	}
	return json;
}

void json_get_string(char *json, char *mystring){
    int i= 0;
    
    while(*json != ':') json++;             /*increment pointer until pointing to ':' (string expected after this)*/
    
    json++;                                 /*this skips the " : "*/
    
    while(*json == ' ') json++;             /*if spaces exist, move past them*/
    
    json++;                                 /*this is used to skip the leading ' " '*/
    
    do {                             /*cycles through and stores characters at json memory to mystring memory*/
       if(*json == '"') break;         /*if we've hit the ' " ' indicating end of string, stop storing values to mystring*/
       *mystring = *json;
       i++;
       json++;
       mystring++;
    }while(1);
    *mystring = '\0';
}

unsigned int json_get_int(char* json){
    int i= 0;
    unsigned int TheInt = 0;        //integer to be returned.
    int imax = 0;                   //to keep track of max size of CharOfInt.
    int powten = 0;                 //used for decimal place placement when converting CharOfInt to TheInt.
    char CharOfInt[16];             //Temporarily holds the number in the form of a character array.

    while(*json != ':') json++;     //increment pointer until pointing to ':' (number is expect after this).

    json++;                         //increment once more to be pointing at either first number or a space.

    do {
        if(*json == ' '){           //if first character after ':' is a space, increment pointer until it's not.
            json++;
        }else{                      //cycles through and stores number characters to the array.
            if(*json == ',') break; //if we've hit the comma indicating the end of the number.
            if(*json == '.') break; //if we've hit the decimal point indicating the end of the integer portion.
            if(*json == '}') break; //if we hit the end of the object
            if(*json>='0' && *json <= '9') { // make sure we are dealing with numbers
                CharOfInt[i] = *json;
                i++;
            }
            json++;
        }
    }while(1);

    imax = i;
    for(i = imax; i > 0; i--){      //iteratively converts the array of chars to an int.
        powten = fast_pow_ten(i-1); //pow function used to get proper decimal place of the number.
        TheInt = TheInt + ((CharOfInt[imax-i]-'0') * powten); 
    }

    return TheInt;
}

int fast_pow_ten(int exp){    //takes exponent and returns 10^exp
    static int pow10[5] = {
        1, 10, 100, 1000, 10000 //only supports up to 10000
    };
    return pow10[exp];
}
