#include <iostream>
#include <sstream>
#include <iomanip>
#include <locale>
#include <string>
#include <ctime>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <filesystem>

#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <boost/program_options.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core/flat_buffer.hpp>

#include <cppcodec/base64_default_rfc4648.hpp>

#include "blockheader.hpp"
#include "file.hpp"

namespace asio = boost::asio;
using asio::ip::tcp;
namespace json = boost::json;
namespace po = boost::program_options;
namespace http = boost::beast::http;
using base64 = cppcodec::base64_rfc4648;

const std::string GENESIS_BLOCK_HASH = "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f";

std::string make_rpc_request(const std::string& method, const json::value& params, int id = 1) {
    json::object request;
    request["jsonrpc"] = "2.0";
    request["method"] = method;
    request["params"] = params;
    request["id"] = id;
    return serialize(request);
}

std::string rpc_call(const std::string& host, const std::string& port,
                     const std::string& user, const std::string& password,
                     const std::string& rpc_request) {
    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::socket socket(io_context);

    // Resolve and connect
    auto endpoints = resolver.resolve(host, port);
    asio::connect(socket, endpoints);

    // Construct the HTTP request
    std::ostringstream req_stream;
    req_stream << "POST / HTTP/1.1\r\n"
               << "Host: " << host << ":" << port << "\r\n"
               << "Authorization: Basic " << base64::encode(user + ":" + password) << "\r\n"
               << "Content-Type: application/json\r\n"
               << "Content-Length: " << rpc_request.size() << "\r\n"
               << "\r\n"
               << rpc_request;

    // Send the request
    boost::asio::write(socket, boost::asio::buffer(req_stream.str()));

    // Read the response
    boost::beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(socket, buffer, res);

    // Check the HTTP status code
    if (res.result() != boost::beast::http::status::ok) {
        std::cerr << "Error: HTTP " << res.result_int() << " - " << res.reason() << std::endl;
        std::cerr << "Response body: " << res.body() << std::endl;
        throw std::runtime_error("HTTP Response not OK.");
    }

    // Parse the JSON response
    try {
        auto json_response = boost::json::parse(res.body());
        // std::cout << "Parsed Response: " << json_response << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        std::cerr << "Response body: " << res.body() << std::endl;
        throw e;
    }

    return res.body();
}

std::string format_double(double value) {
    std::ostringstream oss;
    oss.imbue(std::locale("en_US.UTF-8")); // Use locale for comma formatting
    oss << std::fixed << std::setprecision(1) << value;
    return oss.str();
}

// Helper function to format UNIX timestamp into a more human-readable format
std::string format_time(time_t timestamp) {
    std::tm *gmt = gmtime(&timestamp); // Convert to GMT (UTC)
    std::ostringstream oss;
    oss << std::put_time(gmt, "%B %d %Y %H:%M:%S") << "Z"; // Month Day Year hr:mn:ssZ
    return oss.str();
}

std::string format_blockheader(const BlockHeader& bh) {
    std::ostringstream oss;
    oss << "Block Hash: " << bh.hash() << "\n";
    oss << "Height: " << bh.height() << "\n";
    oss << "Time: " << format_time(bh.time()) << "\n"; // Format time as a string
    oss << "Median Time: " << format_time(bh.mediantime()) << "\n"; // Format median time
    oss << "Bits: " << std::setw(8) << std::setfill('0') << std::hex << bh.bits() << "\n";
    oss << "Difficulty: " << format_double(bh.difficulty()) << "\n";
    oss << "Number of Transactions: " << std::dec << bh.nTx() << "\n";
    oss << "Next Block Hash: " << bh.nextblockhash() << "\n";
    return oss.str();
}

template <typename T, typename Pred>
T* if_in_vector(std::vector<T>& vec, Pred pred) {
    if (vec.empty()) return nullptr;
    auto it = std::find_if(vec.begin(), vec.end(), pred);
    return (it != vec.end()) ? &vec.back() : nullptr;
}

int main(int argc, char* argv[]) {
    std::vector<BlockHeader> block_headers;
    std::string cache_file = "";
    int max_blocks = 10; // Maximum blocks to fetch from the node

    try {
        // Parse command-line arguments
        std::string host, port, user, password, block_hash;
        po::options_description desc("Options");
        desc.add_options()
            ("help,h", "Display help message")
            ("cache", po::value<std::string>(&cache_file)->default_value(cache_file), 
                "Cached Bitcoin block header data (optional)")
            ("maxblocks,m", po::value<int>(&max_blocks)->default_value(max_blocks),
                "Maximum number of blocks to fetch from the node (optional)")
            ("host", po::value<std::string>(&host)->required(), "Bitcoin node host")
            ("port", po::value<std::string>(&port)->required(), "Bitcoin node port")
            ("user", po::value<std::string>(&user)->required(), "RPC username")
            ("password", po::value<std::string>(&password)->required(), "RPC password")
            ("blockhash", po::value<std::string>(&block_hash)->default_value(GENESIS_BLOCK_HASH), "Block hash (optional)");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }
        po::notify(vm);

        if (cache_file != "" && std::filesystem::exists(cache_file)) {
            std::cout << "reading " << cache_file << " ...\n";
            block_headers = read_csv(cache_file);
            std::cout << "------------------------------------------------------------------------\n\n";
        }

        BlockHeader *blockheader = if_in_vector(block_headers, [&block_hash](const BlockHeader& bh) {
            return bh.hash() == block_hash;
        });

        if (blockheader) block_hash = blockheader->nextblockhash();

        std::cout << "fetching data from node if necessary...\n";

        for (int i = 0; i < max_blocks; ++i) {
            if (block_hash.size() == 0) break;

            // Construct JSON-RPC request for `getblockheader`
            json::array params;
            params.emplace_back(block_hash);
            params.emplace_back(true);
            std::string rpc_request = make_rpc_request("getblockheader", params);

            // Perform the RPC call
            std::string rpc_response = rpc_call(host, port, user, password, rpc_request);

            // Parse response
            json::value response_json = json::parse(rpc_response);
            if (response_json.as_object().contains("error") && !response_json.at("error").is_null()) {
                throw std::runtime_error("RPC Error: " + serialize(response_json.at("error")));
            }

            json::object result = response_json.at("result").as_object();
            BlockHeader block_header(result);

            // Print block header
            std::cout << format_blockheader(block_header) << "\n";
            // std::this_thread::sleep_for(std::chrono::milliseconds(100));
            block_hash = block_header.nextblockhash();

            if (block_header.nextblockhash() != "")
                block_headers.push_back(block_header);
        }
        if (cache_file != "") {
            std::cout << "sorting...\n";
            std::sort(block_headers.begin(), block_headers.end());
            auto it = std::unique(block_headers.begin(), block_headers.end());
            block_headers.erase(it, block_headers.end());
        }
        std::cout << "writing file " << cache_file << "\n";
        write_csv(cache_file, block_headers);
        std::cout << "file written\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

