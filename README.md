Gelf UDP to TCP log router
==========================

Presents a Gelf-UDP server that routes all messages to a Gelf-TCP log input (possibly via SSL).

The target use case is when the log source cannot access the graylog server securely via UDP, so this program listens locally for gelf-udp messages and sends them remotely via gelf-tcp + SSL.
