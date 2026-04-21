# webserv-core

A minimal, event-driven HTTP server implemented in **C++ (C++98)**, built around a **single-threaded reactor architecture** using `epoll` (Linux) with a `poll` fallback for portability.

This project explores **low-level network programming**, **I/O multiplexing**, and **modular system design**, with a focus on building a clean and extensible foundation for performance-oriented systems.

---

## Overview

`webserv-core` implements a **non-blocking, event-driven server** capable of handling multiple concurrent connections efficiently within a single thread.

Key features:

* Non-blocking socket I/O
* Event-driven architecture (epoll / poll)
* Single-threaded reactor pattern
* Incremental HTTP/1.0–1.1 parsing
* Modular routing layer
* Explicit connection lifecycle management

The project prioritizes:

* architectural clarity
* deterministic execution
* clean separation of concerns

---

## Architecture

The system is structured into three main layers:

### Network Layer

* `EventLoop` — central event dispatcher
* `Poller` — epoll/poll abstraction
* `Connection` — per-client state and buffering

### HTTP Layer

* `HttpParser` — incremental request parsing
* `HttpService` — routing and response generation
* `HttpServerHandler` — bridge between network and HTTP

### Application Layer

* entry point (`main`)
* signal handling
* server configuration

This layered design enables independent evolution of networking, protocol handling, and application logic.

---

## Request Lifecycle

1. Server socket listens for incoming connections
2. `EventLoop` waits for I/O readiness events
3. New connections are accepted and registered
4. Incoming data triggers read events
5. `HttpParser` processes buffered data incrementally
6. `HttpService` generates a response based on routing rules
7. Response is written asynchronously
8. Connection is closed or reused (keep-alive)

---

## Supported Routes

* `GET /` → basic welcome message
* `GET /health` → health check endpoint
* `POST /echo` → returns request body
* `DELETE /resource` → returns 204 No Content

---

## Design Decisions

### Event-driven architecture

A single-threaded reactor model is used to:

* eliminate synchronization overhead
* reduce context switching
* maintain predictable execution flow

This approach is commonly used in high-performance network servers.

---

### Separation of concerns

* Network handling is isolated from protocol logic
* HTTP parsing is independent from routing
* Connection state is managed explicitly

This improves maintainability and testability.

---

### Portability

The `Poller` abstraction allows switching between:

* `epoll` (Linux)
* `poll` (fallback)

This keeps the event model consistent across platforms.

---

## Performance Notes

The current implementation focuses on **correctness and architectural clarity**, while keeping the design compatible with future optimizations.

The architecture is designed to support:

* reduced syscall overhead (event batching)
* improved buffer management
* reduced dynamic allocations
* tighter event loop execution

Baseline performance can be measured using tools such as `wrk` or `ab`.

---

## Testing

The project includes unit, integration, and stress tests covering:

* HTTP parsing
* connection lifecycle
* event loop behavior
* concurrent client handling
* edge cases (partial reads, disconnects)

### Network tests

```bash
make -C tests/network
./tests/network/bin/network_tests
```

### HTTP tests

```bash
make -C tests/http
./tests/http/bin/http_tests
```

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

Quick tests:

```bash
curl -v http://127.0.0.1:8080/
curl -v http://127.0.0.1:8080/health
curl -v -X DELETE http://localhost:8080/resource
curl -v -X POST http://localhost:8080/echo -d "hello world"
```

---

## Docker Environment

A containerized development environment is provided:

```bash
docker compose up -d --build
docker compose exec webserv-dev bash
make
./webserv 8080 1024
```

---

## Limitations

This project is intentionally scoped as a **minimal, educational implementation**.

Current limitations:

* single-threaded execution model
* no TLS support
* basic routing layer
* no custom memory allocator
* limited performance tuning

These trade-offs were made to prioritize **clarity, modularity, and correctness** before introducing complexity.

---

## Key Learnings

* Designing an event-driven architecture using the Reactor pattern
* Implementing non-blocking I/O with `epoll` and `poll`
* Abstracting OS-level multiplexing mechanisms
* Managing connection state and lifecycle explicitly
* Building an incremental HTTP parsing pipeline
* Structuring a system with clear separation of concerns
* Understanding trade-offs between simplicity and performance
* Preparing a codebase for future low-latency optimizations

---

## Author

Developed as a final project at 42 school, with a focus on **system design, low-level programming, and modular architecture**.

Ilia Chpakovski
[ilia.chpakovski@gmail.com](mailto:ilia.chpakovski@gmail.com)
