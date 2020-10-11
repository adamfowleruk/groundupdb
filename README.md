# GroundUpDb

Creating a database from the ground up in C++ for fun!

GroundUpDB is an Apache 2.0 licensed open source database. It is being created in the 
open as part of a video blog live coding series on database design and
modern C++ by Adam Fowler. @adamfowleruk

Throughout modern C++ idioms will be used. Where a new feature of the latest
stable specification (Currently C++17) is available, it will be used. This
is quite different from a lot of existing open source databases.

Test Driven Development (TDD) will also be practiced throughout, and agile
user stories shall be created for each test before development or design
begins.

The YouTube playlist for this series can be found here: https://www.youtube.com/playlist?list=PLWoOSZbmib_cr7zRfAkPkoa9m2uYsYDug

## Why use GroundUpDB?

GroundUpDB aims to be a high speed, modern implementation of the latest database-relevant algorithms. It aims to be configurable for a range of application and data safety use cases. Multiple abstraction layers over the low-level key-value store provide multi-model capability without sacrificing performance.

The database aims to run on every platforms from an 8-bit microcontroller for edge IoT applications all the way up to multi-node clusters hosted in the cloud, with multiple tenant organisations and applications. The design is highly componentised for this reason.

The database is still young, but I hope to have a proof of concept on a multi-model database with query support that runs on multiple operating systems during the second half of 2020.

### Current performance

In embedded mode compiled for release these are the most recent performance results:-

|Store Type|Operation|Qty|Measure|
|---|---|---|---|
|Memory|SET|1,619,040|Ops/sec|
|Memory|GET|4,390,390|Ops/sec|
|Memory|GET 1000 Keys in a bucket|1.124|ms
|Memory|QUERY for keys in named bucket|0.719|ms
|Memory cached File store (default)|SET|1,260.07|Ops/sec|
|Memory cached File store (default)|GET|3,575,260|Ops/sec|
|File store|SET|741.8|Ops/sec|
|File store|GET|417,80.5|Ops/sec|

## Getting started

You can either build with CMake or QtCreator. Either way executable and library files will be found under the relevant subdirectories for each target within the build folder.

Where `cd groundupdb` appears below, this should always be read as 'move to the top level folder in this repo' :)

### Building Dependencies

Currently Google's highwayhash, which is referenced herein as a sub-module, and must be built before GroundupDB as follows:

```sh
cd groundupdb
git submodule update --init --recursive
cd highwayhash
make lib/libhighwayhash.a
cd ..
```

### Building withCMake

```sh
cd groundupdb
cmake -B ./build
cmake --build ./build --config Debug --target all -j
cd build
```

### Building with QtCreator

Download QtCreator for your platform, clone the repo, and open the main 
groundupdb.pro project file in QtCreator. You can build the whole project
from here. 

```sh
cd ../build*
```

### Using the Command Line Interface (CLI)

Best to start with the command line interface (CLI) called
`groundupdb-cli`.

```sh
cd groundupdb-cli
# Create a database
./groundupdb-cli -n mydb -c
# Set a key
./groundupdb-cli -n mydb -s -k "My key" -v "My amazing value"
# Get a key
./groundupdb-cli -n mydb -g -k "My key"
> My amazing value
# List CLI usage and all other commands
./groundupdb-cli
```

### Running the tests

There are a range of functional and performance benchmark tests included. Run them with this command from your build output folder:-

```sh
cd groundupdb-tests
./groundupdb-tests
```

### Running performance tests

There are a set of standard baseline performance tests. 

WARNING: These will thrash your hard drive when testing the FileKeyValueStore class.

To run these tests execute the following from the groundupdb-tests build folder:-

```sh
./groundupdb-tests "Measure basic performance"
```

They will take around 3-5 minutes to run on a decent system.

## Features

As the database is being created interactively with the community its
development is geared more by which database design and C++ features
people wish to learn rather than true customer or user driven.

Currently it has these user features:-

- (All keys below are unicode strings - you can even use smilies!)
- (All set operations also allow the specification of which bucket to store the key within)
- Set a key-value pair (Any key type (HashedKey class)->Any value type (EncodedValue class))
- Set a key-value pair (Any key type (HashedKey class)->set-of-any value type (EncodedValue class))
- Retrieve an EncodedValue for a HashedKey key
- Retrieve a set-of-EncodedValue value for a HashedKey key
- Query the database for all keys in a named (string) bucket

And these administrative features:-

- Created a new empty database
- Delete a database and all of its content
- Bucket indexing support (term list)

These data safety and storage features are present:-

- Specify a memory-cached file store (default, safe data, balanced speed), pure in memory store (fastest, ephemeral data store like Redis), or pure file store (safest, slowest)
- Strongly consistent file kv store (can be used as a data store or a query index store)
- Strongly consistent in-memory kv store (can be used as a data store or a query index store, and as a read cache for an underlying key-value store, such as the file kv store)

## Future roadmap

There strictly isn't one, but there are a few design principles
I've decided to follow:-

- Multi-model - Will support a range of NoSQL, relational, and graph models
- Advanced querying - Including full text search, geospatial, even DNA 
sequences!
- Enterprise grade (eventually) - Designing for ACID to be added easily, and 
deep security features
- Cloud ready - Will be scale-out and scale-up, which you will be able to 
choose for your needs. Will support being ran in K8S (when I get around to it)
- Will keep up with the latest developments - Both in modern C++ specifications
and in the latest database theory and needs
- Configurable for a wide range of needs - From single threaded embedded to
web-scale 😉 use cases, and eventually consistent high speed to ACID high
durability

Fundamentally if people want me to discuss a particular topic, and are willing
to send me nice things on Patreon https://www.patreon.com/adamfowleruk then I'll build it in!

## License & Copyright

All works are copyright Adam Fowler 2020 unless otherwise stated. Code is
licensed under the Apache 2.0 license unless otherwise stated.

See the NOTICE file for details on dependencies and their licenses.

Thank you to these awesome open source developers for their work!
As programmers we stand on the shoulder of giants!

## Contributing

I do accept open source contributions from people who do not participate
in my blog. Ideally though all contributions are 'optional extras'
or security fixes that won't hamper the future direction of
the database's evolution.

See the CONTRIBUTING file for more information.

## Support

This project is done as a labour of love in my very limited spare time.
I do, however, have some paid-for support options on my Patreon page:
https://www.patreon.com/adamfowleruk

Otherwise, support is on a best-efforts basis, and best requested via
GitHub issues.
