#include "file.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>

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

std::vector<BlockHeader> read_csv(const std::string& filename) {
    // Ensure file path is valid.
    boost::filesystem::path filepath(filename);
    if (!filepath.is_absolute()) {
        filepath = boost::filesystem::absolute(filepath);
    }

    // Open file for reading.
    std::ifstream file(filepath.string());
    if (!file.is_open()) {
        throw std::ios_base::failure("Failed to open file for reading: " + filepath.string());
    }

    std::vector<BlockHeader> headers;
    std::string line;
    
    // Skip the CSV header line.
    std::getline(file, line);

    // Parse each line into a BlockHeader object.
    while (std::getline(file, line)) {
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens, line, boost::is_any_of(","));

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
            std::string hash = boost::algorithm::trim_copy(tokens[6]);
            std::string nextblockhash = boost::algorithm::trim_copy(tokens[7]);

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

