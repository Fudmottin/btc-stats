#ifndef _FILE_H
#define _FILE_H

#include <string>
#include <vector>
#include "blockheader.hpp"

// Writes a vector of BlockHeader objects to a CSV file.
// Replaces the file if it already exists.
void write_csv(const std::string& filename, const std::vector<BlockHeader>& headers);

// Reads a CSV file and returns a vector of BlockHeader objects.
std::vector<BlockHeader> read_csv(const std::string& filename);

#endif //_FILE_H

