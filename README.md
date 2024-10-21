# Battleship Game

This project implements a simple multiplayer Battleship game using TCP sockets in C. Players take turns firing shots at each other's ships, and the game tracks hits and misses.

## Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Game Flow](#game-flow)
- [Code Structure](#code-structure)
- [License](#license)

## Features

- Multiplayer gameplay using TCP sockets.
- Simple text-based interface for interaction.
- Players can place their ships randomly on the grid.
- Track hits and misses on a grid representation of the game board.
- End game detection with win/lose messages.

## Requirements

- C compiler (gcc or similar)
- Basic understanding of C programming and socket programming.
- Linux or macOS environment (or any POSIX-compliant system).

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/battleship-game.git
   cd battleship-game
   ```

2. Compile the server and client:
   ```bash
   gcc -o server server.c
   gcc -o client client.c
   ```
2.bis
  Or just use the compiled files.

## Usage

1. **Start the Server**:
   Run the server program in your terminal, providing an ID and port number:
   ```bash
   ./battleship_serveurt.exe <server_id> <port>
   ```
   Example:
   ```bash
   ./battleship_clientt.exe 50 8080
   ```

2. **Start the Client**:
   Open another terminal window (or multiple windows for multiple clients) and run the client program, providing a client ID, server address, and port number:
   ```bash
   ./client <client_id> <server_address> <port>
   ```
   Example:
   ```bash
   ./client 51 127.0.0.1 8080
   ```

3. Follow the prompts in the client terminal to input your shot coordinates.

## Game Flow

1. Each player connects to the server using the client program.
2. Players take turns entering coordinates to attack their opponent's ships.
3. The server responds with whether the shot was a hit or a miss.
4. The game continues until one player sinks all the opponent's ships.

## Code Structure

- **server.c**: Contains the server logic, handles incoming client connections, and manages the game state.
- **client.c**: Contains the client logic, interacts with the user, and sends shot coordinates to the server.
- **README.md**: Documentation for the project.
