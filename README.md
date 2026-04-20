# webserv-core

A minimal HTTP server implemented in C++ (C++98) using an event-driven architecture based on epoll (Linux) with a poll fallback for portability.

This project was built as a learning exercise to explore low-level network programming, event-driven systems, and HTTP protocol handling.

---

## Overview

The server implements a single-threaded reactor pattern:

- Non-blocking sockets
- Event-driven I/O multiplexing (epoll / poll)
- Minimal HTTP/1.0–1.1 request parsing
- Simple routing layer
- Basic connection lifecycle management

The goal of the project is to build a functional HTTP server while keeping the design simple, readable, and maintainable.

---

## Architecture Summary

The system is structured into three main layers:

- **Network Layer**
  - EventLoop
  - Poller (epoll/poll abstraction)
  - Connection management

- **HTTP Layer**
  - HttpParser
  - HttpService (routing logic)
  - HttpServerHandler (bridge between network and HTTP)

- **Application Layer**
  - main entry point
  - signal handling
  - server configuration

---

## Request Lifecycle

1. Server socket listens for incoming connections
2. EventLoop receives readiness events via Poller
3. New client connections are accepted and registered
4. Incoming data triggers read events
5. HttpParser parses the raw buffer into an HttpRequest
6. HttpService generates an HttpResponse based on routing rules
7. Response is written back to the client
8. Connection is closed or kept alive depending on headers

---

## Supported Routes

The server currently supports the following endpoints:

- `GET /` → basic welcome message
- `GET /health` → health check endpoint
- `POST /echo` → returns request body
- `DELETE /resource` → returns 204 No Content

---

## Design Choices

### Event-driven model
The server is based on a single-threaded event loop using epoll for efficient I/O multiplexing.

### Separation of concerns
- Network handling is separated from HTTP logic
- HTTP parsing is isolated from routing
- Connection state is managed independently

### Portability
A poll-based fallback is provided for non-Linux systems.

---

## Run

```bash
./webserv [port] [max_clients]
```

Example:

```bash
make
./webserv 8080 1024
```

Fast test:

```bash
curl -v http://127.0.0.1:8080/
curl -v http://127.0.0.1:8080/health
curl -v -X DELETE http://localhost:8080/resource
curl -v -X POST http://localhost:8080/echo \
  -d "hello world"
curl -v -X PUT http://localhost:8080/
curl -v http://localhost:8080/doesnotexist
```

---

## Testing

The project includes unit, integration, and stress tests covering:

HTTP parsing
- Connection lifecycle
- Event loop behavior
- Multiple client handling
- Edge cases (partial reads, disconnects)

Network tests :

```bash
make -C tests/network
./tests/network/bin/network_tests
```

Http tests :

```bash
make -C tests/http
./tests/http/bin/http_tests
```

--- 

## Docker environnement

The project includes a single-container development environment.

```bash
docker compose up -d --build
docker compose exec webserv-dev bash
make
./webserv 8080 1024
```

---

## Limitations

This is a learning-oriented implementation and does not aim to be production-ready.

Known limitations:

- No multi-threading
- Basic HTTP routing only
- No TLS support
- Simple memory management (no custom allocator)
- Limited performance optimizations

---

## Key Learnings

- Designing and implementing a single-threaded event-driven architecture based on the Reactor pattern
- Understanding low-level I/O multiplexing mechanisms (epoll and poll) and their trade-offs
- Building an abstraction layer over OS-specific polling mechanisms to ensure portability
- Managing socket lifecycle and connection state in a non-blocking networking model
- Separating concerns between network handling, protocol parsing, and application logic
- Implementing a minimal HTTP/1.1 request parsing pipeline (headers, body, keep-alive)
- Coordinating a full request/response lifecycle through an event-driven system
- Balancing simplicity and performance in a low-level C++ network server design
- Identifying architectural trade-offs between single-threaded event loops and multi-threaded designs

---

## Author

This project was developed at the end of the 42 school curriculum as a way to apply and
demonstrate my understanding of software architecture design and separation of
responsibilities in a system previously studied during the program.

Ilia Chpakovski - ilia.chpakovski@gmail.com
