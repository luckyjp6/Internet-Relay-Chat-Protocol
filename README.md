# Internet Relay Chat Protocol
The IRC document：https://www.rfc-editor.org/rfc/rfc1459.html.  
This repo impliment the IRC server.

## Usage
Use instruction ```make``` to start the IRC server.
You may use any IRC client application to interact with our IRC server. For example, Wee Enhanced Environment for Chat (Weechat, https://weechat.org/) is the application we used to test the server.

## Impliment Instructions
- NICK: Set a nickname.
- USER: Register new user.
- PING: Indicate that the connection is still alive.
- LIST: Query channel information.
- JOIN: Join a channel.
- PART: Leave a channel.
- TOPIC: Query channel topic.
- NAMES: Query all clients' nickname in some channel.
- USERS: Query all users' information.
- PRIVMSG: Send private message to another user.

## Introduction to each file
- main.cpp: Use poll to achieve concurrent server for handling multiple clients. Handle clients' requests.
- errno_func.h/.cpp: Deal with different kinds of error detecting and also print error message to the client if needed.
- function.h/.cpp: basic data structure shared across the program.
- print_msg.h/.cpp: Print the messages the client request.
