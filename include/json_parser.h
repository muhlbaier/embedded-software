/*
 * json_parser.h
 *
 *  Created on: Apr 14, 2018
 *      Author: Delengowski & Szymczak
 */

#ifndef _JSON_PARSER_H_
#define _JSON_PARSER_H_

#include <stdint.h>

/**
 * @author Matt Delengowksi
 * @date Apr 14, 2018
 * @brief Parses a JSON string for the string pointed to by key 
 *
 * @param  key  A pointer to a string that is a key within a JSON string
 * @param  json A pointer to the JSON string for which to parse for keys
 * @return      Returns the location of the first '[' after the key has been found in json
 */
char* json_find_key(char *key, char *json);

/**
 * @brief for finding the nth list item in the json key
 * @param  number The nth list item in the json key
 * @param  json   The json key being passed in
 * @return        returns a char pointer, pointing to the nth list item in the json key
 */
char* json_find_list_item(uint16_t number, char*json);

/* @author Stephen Szymczak
 * @date Apr 14, 2018
 * @brief This function gets the string associated with a list item from a JSON string
 *
 * This function goes through a json string until a ' : ' is found, then gets
 * the string following the list item, parsing out the ' " ' at beginning and end.
 * Fills a character array with the desired string.
 *
 * @param json A pointer to the JSON string for which to parse for the string.
 * @param mystring A pointer to a user defined string which is populated with the
 * string extraced from json
 */
void json_get_string(char* json, char* mystring);

/* @author Stephen Szymczak
 * @date Apr 14, 2018
 * @brief This function gets the integer associated with a list item from a JSON string
 * 
 * This function goes through a json  string until a ' : ' is found, then checks for and skips
 * spaces. It is assumed the next value after either ':' or ' ' will be a number.
 * The function gets the number. Only unsigned ints are valid (0-65535).
 * 
 * @param json A pointer to the JSON string for which to parse for numbers.
 * @return Returns the integer associated with JSON string in question.
 * 
 */
unsigned int json_get_int(char* json);


#endif /* JSON_PARSER_H */

