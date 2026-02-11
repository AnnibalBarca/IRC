*This project has been created as part of the 42 curriculum by almeekel and nagaudey.*

## Description

This repository contains a no-bonus implementation of an IRC server and client as required by the 42 school IRC project. The goal is to implement the core IRC protocol features so that multiple clients can connect to a server, join channels, exchange messages, and use the set of commands required by the project subject.

Core responsibilities include:

- TCP server that accepts multiple concurrent clients
- Basic command parsing and replies according to the IRC protocol used in the subject
- Support for common commands: PASS, NICK, USER, JOIN, PART, PRIVMSG, NOTICE, TOPIC, MODE, NAMES, LIST (see subject for exact requirements)
- Management of channels, users, and message routing

## Project structure

- `server/` — Server-side sources and headers
	- `src/` — server implementation files
	- `include/` — server headers
- `client/` — Simple client program to connect to the server
	- `src/`, `include/`
- `common/` — Shared constants, helpers, and data structures
- `docs/` — Documentation notes (ARCHITECTURE.md, USAGE.md)
- top-level `Makefile`, `README.md`, `.gitignore`

## Instructions

Compilation

Build the project:

	 - Run `make` in the root directory.

		 ```sh
		 make
		 ```

Running the server

- The server binary typically accepts at least a port (and possibly a password, depending on the subject). Example:

	```sh
	./server <PORT> <PASSWORD?>
	# e.g. ./server 6667 mysecret
	```

Running the client

- A simple client executable will connect to the server and allow sending IRC messages from stdin or a prompt:

	```sh
	./client <HOST> <PORT>
	# e.g. ./client 127.0.0.1 6667
	```

## Usage

- Example sequence for a new connection (client-side):

	1. Send `PASS <password>` if the server requires it
	2. Send `NICK <nickname>`
	3. Send `USER <username> 0 * :Real Name`
	*This project has been created as part of the 42 curriculum by almeekel and nagaudey.*

	## Project name and files to submit

	- Program name: `ircserv`
	- Files to submit: `Makefile`, all `*.{h,hpp}`, `*.cpp`, `*.tpp`, `*.ipp`, and an optional configuration file

	The top-level `Makefile` must provide the standard targets: `NAME` (or `all`), `clean`, `fclean`, `re`.

	## Short description

	You are required to develop an IRC server in C++98. The executable will be run as follows:

	```sh
	./ircserv <port> <password>
	```

	- `port`: the listening port where your IRC server accepts incoming TCP connections
	- `password`: the connection password required by any IRC client that connects to your server

	Important constraints from the subject:

	- Language standard: C++98 (pay attention to compatiblity and avoid newer language features)
	- You must NOT implement an IRC client or server-to-server communication
	- Forking is prohibited: the server must be single-process and use non-blocking I/O
	- Only one `poll()` (or equivalent such as `select()`, `epoll()`, `kqueue()`) instance must be used to handle all I/O (listening, read, write, etc.)

	The server must be capable of handling multiple clients simultaneously without hanging. All file descriptors must be non-blocking.

	## External functions allowed

	The subject lists the allowed system calls / C library functions you may use (typical POSIX networking and signal calls):

	- socket, close, setsockopt, getsockname, getprotobyname, gethostbyname, getaddrinfo, freeaddrinfo,
	- bind, connect, listen, accept, htons, htonl, ntohs, ntohl, inet_addr, inet_ntoa, inet_ntop,
	- send, recv, signal, sigaction, sigemptyset, sigfillset, sigaddset, sigdelset, sigismember,
	- lseek, fstat, fcntl, poll (or equivalent)

	Libft: n/a (no Libft requirement)

	## Requirements

	- Handle multiple clients concurrently with non-blocking sockets
	- Use a single `poll()` (or equivalent) loop to handle all I/O operations
	- Use TCP/IP (IPv4 or IPv6)
	- Choose one reference IRC client (see "Reference client" below) and ensure it can connect and operate with your server without errors
	- Implement the following core features (minimum):
	  - Authentication with the server using the provided `password` (PASS command)
	  - Setting nickname (`NICK`)
	  - Setting username (`USER`)
	  - Joining a channel (`JOIN`)
	  - Sending and receiving private messages (`PRIVMSG`) and channel messages
	  - Channel/user management: channels must forward messages from one client to all other joined clients
	  - Have operators and regular users; implement operator-specific commands for channel operators:
	    * `KICK` — eject a client from the channel
	    * `INVITE` — invite a client to a channel
	    * `TOPIC` — change or view channel topic
	    * `MODE` — support channel mode flags: `i` (invite-only), `t` (topic restricted to ops), `k` (channel key), `o` (give/take operator), `l` (user limit)

	## Reference client

	Several IRC clients exist; you must choose one as the reference client that will be used for evaluation. Common, easy-to-use choices are:

	- `weechat` (terminal client)
	- `irssi` (terminal client)
	- `HexChat` (GUI)
	- `netcat`/`telnet` for raw protocol testing (minimal)

	Replace the line below with your chosen reference client before submission:

	Reference client chosen for this project: `<YOUR_REFERENCE_CLIENT_HERE>`

	Make sure your chosen client can authenticate and perform the required operations (NICK, USER, JOIN, PRIVMSG, operator commands) without errors.

	## Build & run (instructions to match the subject)

	Compilation

	From the project root run:

	```sh
	make
	```

	This must produce the `ircserv` executable at the project root (or explain how to find it in your `Makefile` and `README`). Your `Makefile` should also implement `clean`, `fclean` and `re` targets.

	Running the server

	```sh
	./ircserv <PORT> <PASSWORD>
	# e.g. ./ircserv 6667 mysecret
	```

	Testing with the reference client

	Using your chosen reference client (replace `<YOUR_REFERENCE_CLIENT_HERE>`), connect to the server and perform the handshake:

	1. PASS <password> (if required)
	2. NICK <nickname>
	3. USER <username> 0 * :Real Name
	4. JOIN #channel
	5. PRIVMSG #channel :Hello everyone!

	You can also test with `telnet` or `nc` for raw IRC messages when debugging, but the reference client chosen by you must work for evaluation.

	## Additional notes and grading cautions

	- Using non-blocking sockets with a single `poll()` (or equivalent) loop is mandatory — attempting to perform socket I/O without the poll loop will result in a failing grade per the subject
	- Forking or spawning processes to handle clients is prohibited
	- Ensure correct parsing of CRLF-terminated IRC messages and proper reply formatting according to the RFC/subject
	- Keep code clean, modular, and well-commented — readability matters for grading

	## Resources

	- RFC 1459 — Internet Relay Chat Protocol
	- RFC 2810, RFC 2811, RFC 2812 — IRC command replies and behaviour
	- POSIX socket programming references and `poll()` man pages
	- 42 intranet subject page (read the subject for grading details)

	## How AI was used

	- AI assistance was limited to repository scaffolding, drafting documentation (this README), and suggesting implementation approaches. All production code implementing server behaviour should be written and reviewed by the project authors.

	## Submission checklist

	- Replace the placeholder 42 logins at the top of this file with your actual logins
	- Replace `<YOUR_REFERENCE_CLIENT_HERE>` with the client you chose for evaluation
	- Ensure `ircserv` builds with `make` from the project root and that the `Makefile` exposes `clean`, `fclean`, and `re` targets
	- Include only the allowed files and required source files in your final submission

	---


