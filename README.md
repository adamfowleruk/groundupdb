# GroundUpDb

Creating a database from the ground up in C++ for fun!

GroundUpDB is an MIT licensed open source database. It is being created in the 
open as part of a video blog live coding series on database design and
modern C++ by Adam Fowler. @adamfowleruk

Throughout modern C++ idioms will be used. Where a new feature of the latest
stable specification (Currently C++17) is available, it will be used. This
is quite different from a lot of existing open source databases.

Test Driven Development (TDD) will also be practiced throughout, and agile
user stories shall be created for each test before development or design
begins.

The YouTube playlist for this series can be found here: https://www.youtube.com/playlist?list=PLWoOSZbmib_cr7zRfAkPkoa9m2uYsYDug

## Getting started

Download QtCreator for your platform, clone the repo, and open the main 
groundupdb.pro project file in QtCreator. You can build the whole project
from here. Best to start with the command line interface (CLI) called
`groundupdb-cli`.

## Features

As the database is being created interactively with the community its
development is geared more by which database design and C++ features
people wish to learn rather than true customer or user driven.

Currently it has these user features:-

- Set a key-value pair
- Retrieve a value for a key

(Hey, you gotta start somewhere!)

And these administrative features:-

- Created a new empty database
- Delete a database and all of its content

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
licensed under the MIT license unless otherwise stated.

This project repository contains two header only files with their own
licenses for convenience of the programmer. These are:-

- groundupdb-cli/cxxopts.hpp - C++ command line options parser. 
Copyright (c) 2014, 2015, 2016, 2017 Jarryd Beck.
MIT licensed
- groundupdb-tests/catch.hpp - C++ TDD library. 
Copyright (c) 2020 Two Blue Cubes Ltd. All rights reserved.
Boost 1.0 license

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
