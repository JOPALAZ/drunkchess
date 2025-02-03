# Chess Web Implementation

## Overview
This project is a web-based chess implementation consisting of two main components:

1. **Chess Coordinator (C++)** - A standalone chess engine responsible for game logic, using a variation of the Minimax algorithm with dynamic piece weighting. It does not use any third-party libraries, relying only on the standard C++ library.
2. **Frontend & Backend (Django)** - A web interface and API layer that communicates with the chess coordinator through RESTful commands and handles infrastructure needs.

## Key Features
- **Algorithmic Chess AI**: Uses a custom Minimax-based algorithm instead of a neural network.
- **No External Dependencies (C++ part)**: The chess engine is designed to work with only the standard C++ library.
- **Django-Based Web Infrastructure**: The frontend and backend use Python's Django framework to manage interactions with the chess engine.
- **Easy Deployment**: The entire project can be launched using a single script.

## Requirements
### C++ Chess Coordinator
- C++ compiler (supporting C++17 or newer)
- CMake
- Standard C++ library

### Web Interface (Frontend & Backend)
- Python 3
- Django

## Installation & Running the Project
To build and run the entire project, execute the following command:
```bash
./autorun.sh
```
This script will compile the C++ chess engine, set up the Django server, and start the web interface.
