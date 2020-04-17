# Server & client apps

## Building
 * build & install mbedtls dependency (used for sockets & md5 hash):
   ```shell
   $ git clone https://github.com/ARMmbed/mbedtls.git
   $ cd mbedtls
   $ mkdir build
   $ cd build
   $ cmake ..
   $ # make # but in this case you have to pass path's to the apps' cmake
   $ sudo make install ### -j16
   ```
 * build the apps:
   ```shell
   $ mkdir build
   $ export LIBRARY_PATH=/usr/local/lib ### optional
   $ cmake .. ### -DCMAKE_C_FLAGS="-I /usr/local/include"
   $ make
   ```

## Running
 * server with 2 optional positional arguments:
   1. bind-interface defaulting to `127.0.0.1`
   2. listening port defaulting to `8016`
   ```shell
   $ ./apps/server/iteco-server 0.0.0.0 2020
   $ ./apps/server/iteco-server # will bind to 127.0.0.1:8016
   ```
 * client with 1 required and 2 optional positional arguments
   1. path to data csv file
   2. server address defaulting to `127.0.0.1`
   3. server port defaulting to `8016`
   ```shell
   $ ./apps/client/iteco-client /path/to/data.csv
   $ ./apps/client/iteco-client /path/to/data.csv 10.13.50.1 2020
   ```

## Client
Reads a CSV file containing packets with integer sets of data -- each line is a candidate for a custom protocol's packet -- and sends that data to the server.
There is an interval of 10ms between each of the procol's packet sending.

## Server
Consists of 2 threads synced by a conditional vararible & mutex pair, and working around a packets buffer (let's call one packet in the buffer a section):
 * The socket's worker: polls the data from the socket into a free section, marks that section as filled with data and wakes up the packeter worker, so it would serve it.
 * The packeter's worker: awaits for signals from the socket's worker, and when signalled parses packets from the filled sections, checks those packets, and prints to the console its packets' verification verdict.

## The protocol
The protocol uses the TCP transport, uses network data ordering, and consists of two logical parts:
 * The header with metadata, like MD5 hash, time, payload size, etc.
 * The payload: set of 2-bytes signed integers.

### Header
Consistst of several fields in network order
 1. md5 hash -- 16 bytes;
 2. timestamp with microseconds precision -- 8 bytes, uint64_t;
 3. packet number -- 4 bytes, uint32_t;
 4. payload size (number or 2-bytes words) -- 2 bytes, uint16_t;
 5. reserved 2 bytes, unused for now.

### Payload
Consists of 2-byte integers, which are passed in network order.
For now the maximum number is limited to 1600 integers.