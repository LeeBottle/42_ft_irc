*This project has been created as part of the 42 curriculum by byeolee, hugim, sanghul2.*

# Description
This project is an IRC (Internet Relay Chat) server implemented in compliance with the C++ 98 standard. It operates based on TCP/IP (v4 or v6) communication, allowing external IRC clients to connect to the server and utilize actual chat services. Excluding server-to-server federation features, this project aims for a single-server architecture that efficiently handles requests from multiple clients.

## Project Goals & Key Features

### Non-blocking I/O & Single Multiplexing
Creating processes via `fork()` is strictly prohibited. To prevent overall performance degradation and resource waste, all sockets and file descriptors (FDs) are controlled in non-blocking mode. A single `poll()` (or an equivalent function such as `select`, `kqueue`, `epoll`, etc.) is used to centrally and concurrently handle all I/O events, including `listen`, `read`, and `write`.

### Data Integrity Guarantee
The implementation perfectly anticipates scenarios where commands are split into multiple packets (Partial Data) due to low-bandwidth environments or network latency. Received data fragments are fully aggregated into each client's specific buffer and then parsed and executed in complete command units (`\n` or `\r\n`).

### Partial Implementation of IRC Protocol Specification
Designed with reference to the official IRC protocol to ensure no connection errors with the reference client, the server centrally provides the following essential features required for a chat server.

## Detailed Supported Features

### User Management & Authentication
Server connection password verification, user authentication, nickname and username configuration, and private messaging (1:1 DM) transmission/reception.

### Channel (Room) System
Channel creation and joining (JOIN), and broadcasting (forwarding) messages to all users belonging to a specific channel.

### Privilege Separation
Separation of privileges between Channel Operators and Regular Users.

### Operator-Only Commands
- KICK: Forcefully eject a specific user from the channel.
- INVITE: Invite a specific user to the channel.
- TOPIC: Change and view the topic of the channel.
- MODE: Support for changing channel modes:
	- i (Invite-only): Restrict/allow entry only to invited users.
	- t (Topic restriction): Restrict/permit the use of the `TOPIC` command to operators only.
	- k (Channel key): Set/remove a channel entry password.
	- o (Operator privilege): Grant/take away operator privileges to/from other users.
	- l (User limit): Restrict/remove the maximum number of users allowed in the channel.








# Instructions

## 1. Prerequisites

This project is written based on the C++ 98 standard. For proper building, execution, and testing, the following environment and tools must be prepared on the system

- Compilation and Build Tools
	- GNU Make utility
	- c++ or g++ compiler supporting the C++ 98 standard

- Client Programs for Network Testing
	- nc (netcat) utility for testing socket connections and split packet transmissions (Partial Data)
	- Standard IRC reference client (Irssi) for verifying command compatibility and actual IRC communication specifications



## 2. Compilation

Use the following commands defined in the Makefile at the repository root path:

- Build Commands
	```
	make
	```
	- Compiles the source code and generates the `ircserv` executable file according to the rules defined in the Makefile

	```
	make all
	```
	- Performs the entire project build, identical to the make command

- Clean and Rebuild Commands
	```
	make clean
	```
	- Deletes the object files (.o) generated during the compilation process
	```
	make fclean
	```

	- Deletes both the object files and the generated `ircserv` executable file
	```
	make re
	```
	- Completely deletes previous build outputs (fclean) and restarts the compilation from the beginning (make)


## 3. Execution

The command format and arguments to run the program after compilation is complete:

- Execution Command Format
	```
	./ircserv <port> <password>
	```

- Arguments Configuration
	- port: The port number where the IRC server will listen for connection requests
	- password: The password required for client authentication when connecting to the server

- Major Exception Handling Items
	- Argument count error: If the arguments entered at program execution are insufficient or exceeded, prints an error message and terminates
	- Port number validity: Exception handling for inputs out of the port range (less than 0 or greater than 65535) or when non-numeric characters are included
	- Socket binding failure: Blocks execution if the port is already in use by the system or when accessing a port number without proper privileges
	- System call error: Safely terminates after releasing resources if socket creation, non-blocking configuration (fcntl), or listen registration fails



## 4. Testing

### irssi
Standard IRC reference client

#### Connection & Authentication Test
```
/connect <host> <port> <password>

```
- Successful Case
	- Handshake completed and user/nickname registration successful upon matching password
- Exception & Error Handling
	- When an incorrect password is entered: The server rejects the connection, returns `ERR_PASSWDMISMATCH (464)`, and blocks access
	- When attempting to connect with a nickname already existing on the server: Returns `ERR_NICKNAMEINUSE (433)` and requests a nickname change


#### User Management & Authentication
```
/join #<channel>
```
- Successful Case
	- Channel creation or entry is successful, and message broadcasting operates normally
- Exception & Error Handling
	- When accessing with an invalid channel name format (e.g., entering without #): Returns `ERR_NOSUCHCHANNEL (403)`
	- When a user not joined in the channel attempts to send a message to that channel: Returns `ERR_CANNOTSENDTOCHAN (404)`

```
/part #<channel>
```
- Successful Case
	- Successfully exits the corresponding channel, and a part message is broadcast to other users in the channel
- Exception & Error Handling
	- When the target channel does not exist: Returns `ERR_NOSUCHCHANNEL (403)`
	- When the channel exists but the user is not joined in that channel: Returns `ERR_NOTONCHANNEL (442)`
	- When a command argument (channel name) is missing: Returns `ERR_NEEDMOREPARAMS (461)`

```
/msg <nickname> <message>
```
- Successful Case
	- The message is normally displayed in the individual window of the target user, establishing a 1:1 DM
- Exception & Error Handling
	- When the target to send the message (nickname) does not exist on the server: Returns `ERR_NOSUCHNICK (401)`
	- When the message content is empty or the recipient is not specified: Returns `ERR_NORECIPIENT (411)` or `ERR_NOTEXTTOSEND (412)`

```
/nick <new_nickname>
```
- Successful Case
	- The nickname currently in use is changed to a new nickname, and the change is notified to other users in the participating channels
- Exception & Error Handling
	- When the nickname to change to is already being used by another user on the server: Returns `ERR_NICKNAMEINUSE (433)`
	- When the nickname contains invalid characters that cannot be used (e.g., spaces, unavailable special characters, etc.): Returns `ERR_ERRONEUSNICKNAME (432)`
	- When a command argument (new nickname) is missing: Returns `ERR_NONICKNAMEGIVEN (431)`

#### Channel Operator-Only Commands Test (KICK, INVITE, TOPIC)

- Successful Operator Case
```
/kick #<channel> <user_name>
/invite <user_name> #<channel>
/topic #<channel> <new_topic>
```
- Exception & Error Handling (Insufficient Privileges)
	- Verifies receipt of the `ERR_CHANOPRIVSNEEDED (482)` error response when a regular user attempts the above commands
- Exception & Error Handling (Missing Target)
	- Returns `ERR_NOSUCHNICK (401)` when attempting to KICK or INVITE a non-existent user
- Exception & Error Handling (Missing Channel)
	- Returns `ERR_NOSUCHCHANNEL (403)` when executing a command targeted at a non-existent channel
- Exception & Error Handling (Missing KICK Target)
	- Returns `ERR_USERNOTINCHANNEL (441)` when attempting to kick a user who does not exist in the channel
- Exception & Error Handling (Duplicate INVITE)
	- Returns `ERR_USERONCHANNEL (443)` when inviting a user who is already joined in the channel



#### Channel Mode (MODE) Changes & Rule Violation Test

- Invite-Only Mode
	```
	/mode #<channel> +i
	/mode #<channel> -i
	```
	- Successful Case: Allowed channel entry only for invited users after an operator configures it
	- Exception & Error Handling: Returns `ERR_INVITEONLYCHAN (473)` when a regular user who has not received an invitation (INVITE) attempts to enter a channel in the +i state


- Topic Restriction Mode
	```
	/mode #<channel> +t
	/mode #<channel> -t
	```
	- Successful Case: Only operators can modify the channel topic when +t is configured
	- Exception & Error Handling: Returns `ERR_CHANOPRIVSNEEDED (482)` when a regular user without privileges attempts to change the topic via /topic in the +t state


- Channel Key (Password) Mode
	```
	/mode #<channel> +k <password>
	/mode #<channel> -k 
	```
	- Successful Case: Allows entry only for users who enter the correct password
	- Exception & Error Handling: Returns `ERR_BADCHANNELKEY (475)` when attempting to enter a password-configured channel with an incorrect password


- User Limit Mode
	```
	/mode #<channel> +l <number>
	/mode #<channel> -l 
	```
	- Successful Case: Restricts the number of concurrent users within the channel to a maximum of 
	- Exception & Error Handling: Returns `ERR_CHANNELISFULL (471)` when attempting an additional entry into a full channel (+l limit exceeded)


- Operator Privilege Grant/Revocation
	```
	/mode #ft_irc +o <user_name>
	/mode #ft_irc -o <user_name>
	```
	- Successful Case: Allows granting or revoking operator grade to/from another user
	- Exception & Error Handling: Returns `ERR_USERNOTINCHANNEL (441)` when attempting to grant privileges to a user who does not exist within the channel


### nc
Manual command input and raw data verification client

#### Connection & Authentication Test
```
nc -C <host> <port> 
PASS <password>, NICK <nick>, USER <user> 0 - :user
```
- Successful Case
	- Handshake completed and `001 Welcome` message successfully received upon matching password
- Exception & Error Handling
	- When transmitting NICK/USER after entering an incorrect password: The server rejects the connection, returns `ERR_PASSWDMISMATCH (464)`, and blocks access
	- When attempting to transmit a nickname already existing on the server: Returns `ERR_NICKNAMEINUSE (433)` and requests re-entry of a new nickname

#### Channel Entry & Message Transmission Test
```
JOIN #<channel>
PRIVMSG #<channel> :<message>
```
- Successful Case
	- Channel creation or entry is successful, and the receipt of PRIVMSG data broadcasting to that corresponding channel operates normally
- Exception & Error Handling
	- When accessing with an invalid channel name format (e.g., entering `JOIN ft_irc` without #): Returns `ERR_NOSUCHCHANNEL (403)`
	- When a user not joined in the channel attempts to transmit a PRIVMSG to that channel: Returns `ERR_CANNOTSENDTOCHAN (404)`

```
PART #<channel>
```
- Successful Case
	- Successfully exits the corresponding channel, and a part message is broadcast to other users in the channel
- Exception & Error Handling
	- When the target channel does not exist: Returns `ERR_NOSUCHCHANNEL (403)`
	- When the channel exists but the user is not joined in that channel: Returns `ERR_NOTONCHANNEL (442)`
	- When a command argument (channel name) is missing: Returns `ERR_NEEDMOREPARAMS (461)`

```
PRIVMSG <nickname> :<message>
```
- Successful Case
	- A PRIVMSG message in the form of raw data is transmitted to the target user
- Exception & Error Handling
	- When the target to send the message (nickname) does not exist on the server: Returns `ERR_NOSUCHNICK (401)`
	- When the message content is empty or the recipient is not specified: Returns `ERR_NORECIPIENT (411)` or `ERR_NOTEXTTOSEND (412)`


```
NICK <new_nickname>
```
- Successful Case
	- The nickname currently in use is changed to a new nickname, and the change is notified to other users in the participating channels
- Exception & Error Handling
	- When the nickname to change to is already being used by another user on the server: Returns `ERR_NICKNAMEINUSE (433)`
	- When the nickname contains invalid characters that cannot be used (e.g., spaces, unavailable special characters, etc.): Returns `ERR_ERRONEUSNICKNAME (432)`
	- When a command argument (new nickname) is missing: Returns `ERR_NONICKNAMEGIVEN (431)`

#### Channel Operator-Only Commands Test (KICK, INVITE, TOPIC)

- Successful Operator Case
```
KICK #<channel> <user>
INVITE <user_name> #<channel>
TOPIC #<channel> <new_topic>
```
- Exception & Error Handling (Insufficient Privileges)
	- Verifies receipt of the `ERR_CHANOPRIVSNEEDED (482)` error response when a regular user attempts the above commands
- Exception & Error Handling (Missing Target)
	- Returns `ERR_NOSUCHNICK (401)` when attempting to KICK or INVITE a non-existent user
- Exception & Error Handling (Missing Channel)
	- Returns `ERR_NOSUCHCHANNEL (403)` when executing a command targeted at a non-existent channel
- Exception & Error Handling (Missing KICK Target)
	- Returns `ERR_USERNOTINCHANNEL (441)` when attempting to kick a user who does not exist in the channel
- Exception & Error Handling (Duplicate INVITE)
	- Returns `ERR_USERONCHANNEL (443)` when inviting a user who is already joined in the channel



#### Channel Mode (MODE) Changes & Rule Violation Test

- Invite-Only Mode
	```
	MODE #<channel> +i
	MODE #<channel> -i
	```
	- Successful Case: Allowed channel entry only for invited users after an operator configures it
	- Exception & Error Handling: Returns `ERR_INVITEONLYCHAN (473)` when a regular user who has not received an INVITE attempts a JOIN into a channel in the +i state


- Topic Restriction Mode
	```
	MODE #<channel> +t
	MODE #<channel> -t
	```
	- Successful Case: Only operators can modify the channel topic when +t is configured
	- Exception & Error Handling: Returns `ERR_CHANOPRIVSNEEDED (482)` when a regular user without privileges attempts to change the topic via TOPIC in the +t state

- Channel Key (Password) Mode
	```
	MODE #<channel> +k <password>
	MODE #<channel> -k 
	```
	- Successful Case: Allows entry only for users who enter the correct password
	- Exception & Error Handling: Returns `ERR_BADCHANNELKEY (475)` when attempting to enter a password-configured channel with an incorrect password or without a password

- User Limit Mode
	```
	MODE #<channel> +l <number>
	MODE #<channel> -l 
	```
	- Successful Case: Restricts the number of concurrent users within the channel to a maximum of 
	- Exception & Error Handling: Returns `ERR_CHANNELISFULL (471)` when attempting an additional JOIN into a full channel (+l limit exceeded)

- Operator Privilege Grant/Revocation
	```
	MODE #<channel> +o <user_name>
	MODE #<channel> -o <user_name>
	```
	- Successful Case: Allows granting or revoking operator grade to/from another user
	- Exception & Error Handling: Returns `ERR_USERNOTINCHANNEL (441)` when attempting to grant privileges to a user who does not exist within the channel

## 5. Network Edge Cases & Advanced Handling
Handling Abnormal Client Termination
- When a user forcefully blocks a socket via `ctrl+C` or a network disconnection occurs, the `poll()` event loop detects this to safely `close` the socket
- Maintains integrity by automatically handling departure (`QUIT`) from all channels the user was joined in
Abnormal Payload Defense
- When large-scale malicious data without a specified newline character (`\n`) flows in indefinitely, or when abnormal buffer overflow attack attempts occur
- Packets exceeding a certain byte specification
- Exception handling that considers it an invalid command, clearing the buffer or forcefully dropping the connection

# Resources

## IRC protocol references

| Subject | Resource | Purpose |
| --- | --- | --- |
| IRC Original Protocol | [RFC 1459 - Internet Relay Chat Protocol](https://www.rfc-editor.org/rfc/rfc1459.html) | Foundational baseline for IRC message formats, commands, and numeric replies |
| IRC Original Protocol (IETF) | [RFC 1459 - Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459) | Referenced to understand the standard architecture, basic message formats, and client-server communication protocols of the IRC protocol. Learned the basic operations of KICK, INVITE, TOPIC, MODE, and the standard specifications of Numeric Replies |
| IRC architecture | [RFC 2810 - Internet Relay Chat: Architecture](https://www.rfc-editor.org/rfc/rfc2810.html) | Used for understanding IRC client/server/channel structures |
| IRC channel management | [RFC 2811 - Internet Relay Chat: Channel Management](https://www.rfc-editor.org/rfc/rfc2811.html) | Used for verifying concepts of channels, channel modes, operators, and invite-only |
| IRC client protocol | [RFC 2812 - Internet Relay Chat: Client Protocol](https://www.rfc-editor.org/rfc/rfc2812.html) | Used for verifying client-server command formats such as PASS/NICK/USER/JOIN/PRIVMSG/MODE |
| IRC server protocol | [RFC 2813 - Internet Relay Chat: Server Protocol](https://www.rfc-editor.org/rfc/rfc2813.html) | Used as a reference for server-to-server interaction. Not part of the ft_irc mandatory implementation |
| Modern IRC specification | [Modern IRC Client Protocol](https://modern.ircdocs.horse/) | Referenced to understand the protocol specifications used by modern IRC clients after RFC 1459 and to identify the differences |
| IRCv3 capability negotiation | [IRCv3 - Capability Negotiation](https://ircv3.net/specs/extensions/capability-negotiation.html) | Used for understanding `CAP LS` and `CAP END` sent by irssi during initial connection |
| Irssi Chat Client | [Irssi](https://irssi.org/documentation/settings/) | Used for verifying setting configurations on the official IRSSI website |

## Linux / POSIX system call references

| Category | Function | Resource |
| --- | --- | --- |
| socket creation | `socket()` | [Linux man page - socket(2)](https://man7.org/linux/man-pages/man2/socket.2.html) |
| socket option | `setsockopt()` | [Linux man page - setsockopt(2)](https://man7.org/linux/man-pages/man2/setsockopt.2.html) |
| bind/listen/accept | `bind()`, `listen()`, `accept()` | [bind(2)](https://man7.org/linux/man-pages/man2/bind.2.html), [listen(2)](https://man7.org/linux/man-pages/man2/listen.2.html), [accept(2)](https://man7.org/linux/man-pages/man2/accept.2.html) |
| client connect | `connect()` | [Linux man page - connect(2)](https://man7.org/linux/man-pages/man2/connect.2.html) |
| socket address verification | `getsockname()` | [Linux man page - getsockname(2)](https://man7.org/linux/man-pages/man2/getsockname.2.html) |
| address verification | `getaddrinfo()`, `freeaddrinfo()` | [Linux man page - getaddrinfo(3)](https://man7.org/linux/man-pages/man3/getaddrinfo.3.html) |
| address verification, legacy | `gethostbyname()` | [Linux man page - gethostbyname(3)](https://man7.org/linux/man-pages/man3/gethostbyname.3.html) |
| protocol DB | `getprotobyname()` | [Linux man page - getprotoent(3)](https://man7.org/linux/man-pages/man3/getprotoent.3.html) |
| byte order conversion | `htons()`, `htonl()`, `ntohs()`, `ntohl()` | [Linux man page - byteorder(3)](https://man7.org/linux/man-pages/man3/htons.3.html) |
| IP address conversion | `inet_addr()`, `inet_ntoa()`, `inet_ntop()` | [inet(3)](https://man7.org/linux/man-pages/man3/inet.3.html), [inet_ntop(3)](https://man7.org/linux/man-pages/man3/inet_ntop.3.html) |
| data receive | `recv()` | [Linux man page - recv(2)](https://man7.org/linux/man-pages/man2/recv.2.html) |
| data send | `send()` | [Linux man page - send(2)](https://man7.org/linux/man-pages/man2/send.2.html) |
| event multiplexing | `poll()` | [Linux man page - poll(2)](https://man7.org/linux/man-pages/man2/poll.2.html) |
| non-blocking fd configuration | `fcntl()` | [Linux man page - fcntl(2)](https://man7.org/linux/man-pages/man2/fcntl.2.html) |
| signal handling | `signal()`, `sigaction()` | [signal(2)](https://man7.org/linux/man-pages/man2/signal.2.html), [sigaction(2)](https://man7.org/linux/man-pages/man2/sigaction.2.html) |
| signal set manipulation | `sigemptyset()`, `sigfillset()`, `sigaddset()`, `sigdelset()`, `sigismember()` | [Linux man page - sigsetops(3)](https://man7.org/linux/man-pages/man3/sigsetops.3.html) |
| fd close | `close()` | [Linux man page - close(2)](https://man7.org/linux/man-pages/man2/close.2.html) |
| file offset / file info | `lseek()`, `fstat()` | [lseek(2)](https://man7.org/linux/man-pages/man2/lseek.2.html), [fstat(2)](https://man7.org/linux/man-pages/man2/fstat.2.html) |

## Additional references

| Subject | Resource | Purpose |
| --- | --- | --- |
| Network Programming | [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/split/) | Referenced to understand the fundamentals of C/C++ based socket programming and TCP/IP networks, and to learn how to use `socket()`, `bind()`, `listen()`, `accept()`, `recv()`, `send()`, and `poll()`. |

## Use of AI
### Translation
Generative AI was utilized in the process of translating the final README.md document of this project from Korean to English and smoothing the overall context.
