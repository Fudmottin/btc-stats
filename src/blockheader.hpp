#ifndef _BLOCKHEADER_H
#define _BLOCKHEADER_H

#include <string>
#include <cstdint>
#include <ctime>
#include <boost/json.hpp>

class BlockHeader {
public:
    explicit BlockHeader(const boost::json::object& json);
    BlockHeader(
        uint32_t height, uint32_t nTx, uint32_t bits, double difficulty,
        std::time_t time, std::time_t mediantime, 
        std::string hash, std::string nextblockhash
    );

    ~BlockHeader();

    const std::string& hash() const noexcept;
    uint32_t height() const noexcept;
    std::time_t time() const noexcept;
    std::time_t mediantime() const noexcept;
    uint32_t bits() const noexcept;
    double difficulty() const noexcept;
    uint32_t nTx() const noexcept;
    const std::string& nextblockhash() const noexcept;

private:
    uint32_t height_;
    uint32_t nTx_;
    uint32_t bits_;
    double difficulty_;
    std::time_t time_;
    std::time_t mediantime_;
    std::string hash_;
    std::string nextblockhash_;
};

#endif //_BLOCKHEADER_H

