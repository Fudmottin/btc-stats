# btc-stats

`btc-stats` is a command-line utility that interfaces with a Bitcoin Core-compatible
full node to gather and display blockchain statistics. The program can be used to
track various blockchain metrics such as hash rate, block times, and more.

## Dependencies

To build and run `btc-stats`, the following dependencies are required:

### 1. **C++20 or later**
   This project is written in C++20, so you'll need a compiler that supports it (e.g., GCC, Clang).

### 2. **CMake**
   CMake is required to configure and build the project.

   - Install CMake using Homebrew:
     
```bash
brew install cmake
```

### 3. **Boost Libraries**
   This project uses the following Boost libraries:
   - Boost.Asio (for networking)
   - Boost.JSON (for JSON parsing)
   - Boost.ProgramOptions (for command-line argument parsing)
   - Boost.Beast (for handling HTTP requests)

You can install Boost via Homebrew:
```bash
brew install boost
```

### 4. **Bitcoin Core Full Node (for RPC access)**
This program requires access to a running Bitcoin Core-compatible full node. The node must have the following enabled:

- RPC enabled
- user/password for RPC access
  
You can run your own Bitcoin Core node or use an external provider, but make sure the RPC interface is accessible and that you have the necessary credentials.

## Building the Project

Clone the repository.

```bash
cd btc-stats
mkdir build
cd build
cmake ..
make
```

This will compile the project and create an executable named btc-stats.

## Running the Program

To run the program, you need to pass the required command-line arguments:

./btc-stats --host <bitcoin_node_host> --port <rpc_port> --user <rpc_user> --password <rpc_password> [--blockhash <block_hash>]

Arguments:

--host: The host of your Bitcoin Core node (e.g., 127.0.0.1 or an external IP).

--port: The RPC port of your Bitcoin Core node (default: 8332 for mainnet).

--user: The RPC username for authentication.

--password: The RPC password for authentication.

--blockhash (optional): The block hash to query for stats (defaults to the genesis block if not provided).


## Example:

./btc-stats --host 127.0.0.1 --port 8332 --user myrpcuser --password myrpcpassword --blockhash 000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f

If the --blockhash argument is not provided, the program will default to the genesis block hash.

## License

This project is licensed under the GPL v3 License - see the LICENSE file for details.


