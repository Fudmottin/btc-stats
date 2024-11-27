#include "blockheader.hpp"

BlockHeader::BlockHeader(const boost::json::object& json)
    : height_(json.at("height").as_int64()),
      nTx_(json.at("nTx").as_int64()),
      bits_(std::stoul(json.at("bits").as_string().c_str(), nullptr, 16)), // Parse hexadecimal string to uint32_t
      time_(json.at("time").as_int64()),
      mediantime_(json.at("mediantime").as_int64()),
      hash_(json.at("hash").as_string()),
      nextblockhash_(json.contains("nextblockhash") ? json.at("nextblockhash").as_string() : "") 
{
    boost::json::value difficulty_value = json.at("difficulty");

    // Try to handle difficulty as a double.
    double difficulty = 0;
    if (difficulty_value.is_double()) {
        difficulty = difficulty_value.as_double();
    } else if (difficulty_value.is_int64()) {
        difficulty = static_cast<double>(difficulty_value.as_int64());
    } else {
        throw std::runtime_error("Difficulty value is neither an integer nor a double.");
    }
    difficulty_ = difficulty;
}

BlockHeader::~BlockHeader() = default;

const std::string& BlockHeader::hash() const noexcept {
    return hash_;
}

uint32_t BlockHeader::height() const noexcept {
    return height_;
}

std::time_t BlockHeader::time() const noexcept {
    return time_;
}

std::time_t BlockHeader::mediantime() const noexcept {
    return mediantime_;
}

uint32_t BlockHeader::bits() const noexcept {
    return bits_;
}

double BlockHeader::difficulty() const noexcept {
    return difficulty_;
}

uint32_t BlockHeader::nTx() const noexcept {
    return nTx_;
}

const std::string& BlockHeader::nextblockhash() const noexcept {
    return nextblockhash_;
}

