*This project has been created as part of the 42 curriculum by almeekel and nagaudey.*

# Description

This repository contains an IRC server implementation in C++98 as required by the 42 school IRC project. This server implements the core functionalities of an IRC server, allowing multiple clients to connect, communicate in channels, and exchange private messages using standard IRC clients like irssi, WeeChat, or HexChat.
## Core Features

- **Networking**: TCP server on configurable port with password authentication
- **Concurrency**: Multiple clients handled simultaneously via non-blocking sockets and single `poll()` loop
- **Commands Implemented**: 
  - **Authentication**: `PASS` (server password)
  - **Registration**: `NICK` (nickname), `USER` (username)
  - **Channels**: `JOIN` (join channel), `QUIT` (disconnect), `PRIVMSG` (messaging)
  - **Channel Operators**: `KICK` (remove user), `INVITE` (invite user), `TOPIC` (set topic), `MODE` (channel modes)
  - **Supported MODE flags**: `i` (invite-only), `t` (topic restricted to ops), `k` (channel password), `o` (operator), `l` (user limit)
- **Channel Management**: Case-insensitive channel names (normalized to uppercase), message broadcast to all members, operator privileges
- **Error Handling**: Proper IRC reply codes for authentication failures, invalid parameters, permission issues

# Instructions

This is a 42 school project implementing a single-process, non-blocking IRC server.

## Compilation & Execution

### Building

From the project root:

```bash
make              # Builds ircserv executable
make clean        # Removes object files
make fclean       # Removes all generated files
make re           # Full rebuild
```

### Running the Server

```bash
./ircserv <PORT> <PASSWORD>
# Example:
./ircserv 6667 pass
```

- `<PORT>`: Listening port (typically 6667 or 1234)
- `<PASSWORD>`: Password required by all clients to authenticate via `PASS`

### Testing with netcat (Raw Protocol)

For quick protocol testing:

```bash
printf 'PASS pass\r\nNICK mynick\r\nUSER myuser 0 * :My Name\r\nJOIN #mychannel\r\nPRIVMSG #mychannel :Hello!\r\n' | nc 127.0.0.1 6667
```

## Reference Client

**Chosen reference client: `irssi` (terminal-based IRC client)**

### Why irssi?

- Lightweight, terminal-based, easy to use
- Widely available on Linux/macOS
- Supports all tested commands: PASS, NICK, USER, JOIN, PRIVMSG, TOPIC, KICK, INVITE, MODE
- Standard choice for IRC server validation

### Installing irssi

```bash
# On Ubuntu/Debian
sudo apt-get install irssi

# On macOS
brew install irssi

# On Fedora/RHEL
sudo dnf install irssi
```

### Connecting with irssi

1. Start irssi:
   ```bash
   irssi
   ```

2. In irssi, connect to your server:
   ```
   /connect 127.0.0.1 6667 secretpassword
   ```
   Or manually set credentials then connect:
   ```
   /set irc_nick mynick
   /set irc_user_name myuser
   /set irc_realname "My Real Name"
   /connect 127.0.0.1 6667 secretpassword
   ```

3. Join a channel:
   ```
   /join #mychannel
   ```

4. Send a message:
   ```
   /msg #mychannel Hello everyone!
   ```

5. Operator commands (if you have channel op):
   ```
   /topic #mychannel New topic
   /kick #mychannel username
   /invite username #channel
   /mode #mychannel +o username
   ```

## Protocol Compliance

This implementation is based on **RFC 1459** (Internet Relay Chat Protocol):
- Case-insensitive channel names (normalized to uppercase internally)
- Proper CRLF line termination (`\r\n`)
- Standard IRC reply codes (001, 401, 403, 404, 411–412, 421, 431–433, 441–443, 461–462, 464, 471–475, 482)
- Message routing: channel broadcasts, private messages, operator notifications

	## Implementation Notes

	### Architecture

	- **Single `poll()` loop**: All I/O (client accept, recv, send) managed centrally in `Server::run()`
	- **Non-blocking sockets**: All file descriptors set to `O_NONBLOCK` via `fcntl()`
	- **Buffering**: Partial IRC commands accumulated per-client until complete line received (CRLF)
	- **Channel case-folding**: Channel names normalized to uppercase on lookup (RFC 1459)
	- **Operator privileges**: First joiner becomes operator; MODE +o/−o updates operator status

	## Resources & References

	- **RFC 1459** — Internet Relay Chat Protocol (primary reference for this implementation)
	- RFC 2810, RFC 2811, RFC 2812 — Extended IRC RFCs
	- POSIX socket programming and `poll()` documentation
	- irssi IRC client (reference client for testing)

	## References for Comparison

	To check protocol compliance, compare behavior with reference IRC servers:

	- **ngIRCd** — https://github.com/ngircd/ngircd (lightweight C server, good for comparing basic commands)
	- **InspIRCd** — https://github.com/inspircd/inspircd (modern modular server)
	- **UnrealIRCd** — https://www.unrealircd.org/ (full-featured, production server)

	## Additional Notes

	- No forking (single-process, non-blocking I/O)
	- No external libraries beyond POSIX (C++98 standard)
	- Channel names are case-insensitive per RFC 1459
	- All error/success replies follow IRC protocol codes
	- Tested and validated with multiple concurrent clients

	## Grading Checklist

	- ✓ Makefile builds with `make`, produces `ircserv` at project root
	- ✓ Single `poll()` loop (verified in Server.cpp)
	- ✓ All fcntl() calls use only `F_SETFL, O_NONBLOCK`
	- ✓ Multiple concurrent clients without blocking
	- ✓ Partial commands don't deadlock other clients
	- ✓ Abrupt client disconnect doesn't crash server
	- ✓ All required commands implemented: PASS, NICK, USER, JOIN, PRIVMSG, TOPIC, MODE, KICK, INVITE, QUIT
	- ✓ Channel operator privileges enforced
	- ✓ Case-insensitive channel names (normalized uppercase)
	- ✓ Message broadcast to all channel members
	- ✓ Proper IRC reply codes for errors and success

	---


