# Inter Process Chat Application Using IPC

## Project Overview
The Inter Process Chat Application is a Linux-based project that demonstrates Inter-Process Communication (IPC) using named pipes (FIFOs). It also includes a web interface built with HTML, CSS, and JavaScript connected to a C-based HTTP backend.

## Objectives
- To understand Inter-Process Communication in Linux.
- To implement communication between processes using named pipes (FIFOs).
- To develop a simple web-based chat interface.
- To connect the web interface to a C backend.

## Technologies Used
- **C Programming:** Backend and IPC implementation.
- **Linux Ubuntu:** Development and execution environment.
- **Named Pipes (FIFOs):** Communication between terminal chat processes.
- **HTML:** Web page structure.
- **CSS:** User interface styling.
- **JavaScript:** Frontend message handling.
- **libmicrohttpd:** HTTP server library.
- **Git and GitHub:** Version control and project hosting.

## Project Structure
```text
IPC_Chat_Application/
├── index.html
├── style.css
├── script.js
├── .gitignore
└── backend/
    ├── chat.c
    ├── chat_backup.c
    ├── server.c
    └── server_backup.c
```

## Setup Requirements
- Ubuntu or another compatible Linux environment
- GCC compiler
- Python 3
- libmicrohttpd development library
- pkg-config

Install the required backend dependencies on Ubuntu:

```bash
sudo apt update
sudo apt install gcc python3 pkg-config libmicrohttpd-dev
```

## How to Compile the Backend

From the project directory, run:

```bash
gcc -Wall -Wextra backend/server.c -o backend/server $(pkg-config --cflags --libs libmicrohttpd)
gcc backend/chat.c -o backend/chat
```

## How to Run the Web Application

**Terminal 1 — Start the website:**

```bash
cd ~/IPC_Chat_Application
python3 -m http.server 8000
```

Open this address in your browser:

http://localhost:8000

**Terminal 2 — Start the C HTTP backend:**

```bash
cd ~/IPC_Chat_Application
./backend/server
```

## How to Run the Terminal Chat

Run each command in a separate terminal.

**Terminal 3:**

```bash
cd ~/IPC_Chat_Application
./backend/chat 1
```

**Terminal 4:**

```bash
cd ~/IPC_Chat_Application
./backend/chat 2
```

The terminal chat uses named pipes to demonstrate IPC between the two processes.

## Testing
- Verified that the website loads in the browser.
- Verified that messages submitted through the website reach the C HTTP backend.
- Tested terminal communication between User 1 and User 2 using named pipes.

**Note:** Website-to-backend communication and terminal FIFO chat were tested as separate paths. Automatic forwarding of website messages into the User 2 terminal has not been confirmed.

## Team Details
- **Team:** G-10
- **T. Meghana Sai** — 2520030245
- **K. Akshitha Reddy** — 2520030233

## Future Enhancements
- Integrate website messages with the FIFO chat system.
- Improve message delivery and error handling.
- Add timestamps and message history.
- Improve the user interface.

## Repository
GitHub: https://github.com/meghana0707/Inter-Process-Chat-Application-Using-IPC
