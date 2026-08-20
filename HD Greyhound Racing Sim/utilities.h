#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
using std::string;

/**
 * Read a string from the user
 *
 * @param prompt the message to show the user
 * @returns the string entered
 */
string read_string(string prompt);

/**
 * Read an integer from the user
 *
 * @param prompt the message to show the user
 * @returns the integer entered
 */
int read_integer(string prompt);

/**
 * Read an yes no value from the user
 *
 * @param prompt the message to show the user
 * @returns the boolean value of yes or no entered
 */
bool read_boolean(string prompt);

/**
 * Read a double from the user
 *
 * @param prompt the message to show the user
 * @returns the double entered
 */
double read_double(string prompt);

#endif