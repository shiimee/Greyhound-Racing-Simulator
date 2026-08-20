#include "utilities.h"
#include "splashkit.h"

string read_string(string prompt)
{
  write(prompt);
  return read_line();
}

int read_integer(string prompt)
{
  string line = read_string(prompt);
  while (!is_integer(line))
  {
    write_line("Please enter a whole number.");
    line = read_string(prompt);
  }
  return to_integer(line);
}

bool read_boolean(string prompt)
{
  string line = to_lowercase(read_string(prompt));

  while (line != "y" && line != "n")
  {
    write("Enter [y/n]: ");
    line = to_lowercase(read_line());
  }

  if (line == "y")
  {
    return true;
  }
  else
  {
    return false;
  }
}

double read_double(string prompt)
{
  string line = read_string(prompt);
  while (!is_double(line))
  {
    write_line("Please enter a double.");
    line = read_string(prompt);
  }
  return to_double(line);
}