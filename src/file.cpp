#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "file.hpp"

void write_csv(const std::string& filename, const std::vector<BlockHeader>& headers) {
    // Ensure file path is valid.
    boost::filesystem::path filepath(filename);
    if (!filepath.is_absolute()) {
        filepath = boost::filesystem::absolute(filepath);
    }

    // Open file for writing, overwriting if it exists.
    std::ofstream file(filepath.string());
    if (!file.is_open()) {
        throw std::ios_base::failure("Failed to open file for writing: " + filepath.string());
    }

    // Write CSV header.
    file << "height,nTx,bits,difficulty,time,mediantime,hash,nextblockhash\n";

    // Write each BlockHeader to the file.
    for (const auto& header : headers) {
        file << header.height() << ',' 
             << header.nTx() << ',' 
             << header.bits() << ',' 
             << std::setprecision(std::numeric_limits<double>::max_digits10) << header.difficulty() << ',' 
             << header.time() << ',' 
             << header.mediantime() << ',' 
             << '"' << header.hash() << '"' << ',' 
             << '"' << header.nextblockhash() << '"' << '\n';
    }

    file.close();
}

std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> result;
    std::istringstream stream(line);
    std::string token;
    bool in_quotes = false;

    while (std::getline(stream, token, ',')) {
        if (!in_quotes && token.front() == '"') {
            in_quotes = true;
            token.erase(0, 1); // Remove leading quote
        }

        if (in_quotes && token.back() == '"') {
            in_quotes = false;
            token.pop_back(); // Remove trailing quote
        }

        if (!in_quotes) {
            boost::replace_all(token, "\"\"", "\""); // Unescape quotes
            result.push_back(token);
        } else {
            result.back() += "," + token; // Handle commas inside quoted fields
        }
    }

    return result;
}

std::vector<BlockHeader> read_csv(const std::string& filename) {
    boost::filesystem::path filepath(filename);
    if (!filepath.is_absolute()) {
        filepath = boost::filesystem::absolute(filepath);
    }

    std::ifstream file(filepath.string());
    if (!file.is_open()) {
        throw std::ios_base::failure("Failed to open file for reading: " + filepath.string());
    }

    std::vector<BlockHeader> headers;
    std::string line;

    // Skip the header
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::vector<std::string> tokens = split_csv_line(line);
        if (tokens.size() != 8) {
            throw std::runtime_error("Malformed CSV line: " + line);
        }

        try {
            uint32_t height = boost::lexical_cast<uint32_t>(tokens[0]);
            uint32_t nTx = boost::lexical_cast<uint32_t>(tokens[1]);
            uint32_t bits = boost::lexical_cast<uint32_t>(tokens[2]);
            double difficulty = boost::lexical_cast<double>(tokens[3]);
            std::time_t time = boost::lexical_cast<std::time_t>(tokens[4]);
            std::time_t mediantime = boost::lexical_cast<std::time_t>(tokens[5]);
            std::string hash = tokens[6];
            std::string nextblockhash = tokens[7];

            headers.emplace_back(
                height, nTx, bits, difficulty, time, mediantime,
                std::move(hash), std::move(nextblockhash)
            );
        } catch (const boost::bad_lexical_cast& e) {
            throw std::runtime_error("Error parsing CSV line: " + line + " (" + e.what() + ")");
        }
    }

    file.close();
    return headers;
}

