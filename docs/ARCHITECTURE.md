# ARCHITECTURE.md

# Architecture Overview

This document describes the architecture of a modular HTTP server implemented in C++, designed around a **single-threaded, event-driven model** using `epoll` (Linux) with a `poll` fallback for portability.

The system emphasizes:

* deterministic execution
* low-overhead I/O handling
* clear separation of concerns
* extensibility toward performance-oriented improvements

---

# High-Level Architecture

The server follows a **Reactor pattern**, where I/O events are multiplexed and dispatched through a central event loop:

```
        ┌──────────────┐
        │   main.cc    │
        └──────┬───────┘
               │
               ▼
        ┌───────────────┐
        │  EventLoop    │
        └──────┬────────┘
               │
       ┌───────┴────────┐
       ▼                ▼
    Poller      Connection Registry
      │
      ▼
  NetworkEvent stream
      │
      ▼
  HttpServerHandler
      │
      ▼
  HttpParser → HttpService
      │
      ▼
  HttpResponse
```

This design enables efficient handling of multiple concurrent connections using non-blocking I/O.

---

# Event Loop Model

The `EventLoop` is the core execution unit of the server.

Responsibilities include:

* waiting for I/O events via the `Poller`
* dispatching events to connection handlers
* managing lifecycle of connections
* coordinating read/write operations

Execution model:

```cpp
while (running)
    runOnce(timeout);
```

### Design Rationale

* **Single-threaded execution** avoids synchronization overhead (locks, contention)
* **Deterministic control flow** simplifies reasoning and debugging
* **Low-latency potential** by minimizing context switching and thread scheduling

This model is inspired by high-performance network servers where predictable latency is preferred over parallel complexity.

---

# Poller Abstraction

The `Poller` provides a thin abstraction over OS-level I/O multiplexing mechanisms:

* `epoll` (Linux) for scalable event notification
* `poll` as a portable fallback

Responsibilities:

* registering file descriptors
* updating interest masks (read/write)
* removing descriptors
* translating OS events into internal `NetworkEvent` objects

### Design Goals

* isolate platform-specific logic
* allow future extensions (e.g., kqueue)
* keep event handling consistent across backends

---

# Connection Model

Each client is represented by a `Connection` object.

A connection encapsulates:

* socket file descriptor
* input/output buffers
* connection state (read/write readiness, closed state)

Responsibilities:

* incremental reading from socket
* buffering partial requests
* writing responses asynchronously
* managing connection lifecycle

### Notes

The design supports:

* non-blocking I/O
* partial reads/writes
* persistent connections (keep-alive)

---

# HTTP Layer

## HttpParser

Transforms raw TCP streams into structured HTTP requests.

Features:

* request line parsing
* header parsing
* Content-Length handling
* keep-alive detection

The parser operates incrementally, allowing partial buffer processing.

---

## HttpService

Implements application-level logic:

* routing based on method and path
* generation of `HttpResponse`

This layer is intentionally minimal and modular, allowing extension without impacting the networking core.

---

# Request Lifecycle

1. Client connects → accept event
2. `Connection` object is created
3. File descriptor registered in `Poller`
4. Read event triggered
5. Data buffered and parsed (`HttpParser`)
6. Request handled (`HttpService`)
7. Response generated
8. Write event triggered
9. Connection closed or reused (keep-alive)

---

# Design Principles

## Separation of Concerns

The system is structured into distinct layers:

* **Network layer** → EventLoop, Poller, Connection
* **Protocol layer** → HTTP parsing
* **Application layer** → request handling

This improves maintainability and allows independent evolution of components.

---

## Event-Driven Design

* avoids blocking operations
* scales with number of file descriptors
* enables efficient concurrency without threads

---

## Deterministic Execution

* no shared-state concurrency
* predictable control flow
* easier debugging and profiling

---

# Trade-offs

## Single-threaded architecture

Pros:

* no locking overhead
* simpler reasoning
* predictable latency characteristics

Cons:

* limited CPU parallelism
* requires careful design for scalability

---

## Minimal HTTP implementation

The HTTP layer is intentionally lightweight:

* focuses on correctness and integration
* avoids premature complexity

---

## Performance Scope

The current implementation prioritizes:

* architectural clarity
* correctness
* modularity

Advanced optimizations (zero-copy, custom allocators, etc.) are **intentionally deferred**.

---

# Performance Considerations & Future Work

The architecture is designed to support further performance-oriented improvements:

### I/O & Event Loop

* reduce syscall overhead (event batching)
* tighter event loop control (reduced indirection)
* improved readiness handling

### Memory Management

* reduce dynamic allocations in hot paths
* introduce memory pooling strategies
* improve cache locality of connection structures

### Networking

* optimize buffer management
* explore zero-copy techniques
* refine write batching

---

# Possible Extensions

The current design can be extended toward a more complete server:

* configuration system (Nginx-style)
* session management
* routing abstraction
* middleware pipeline

---

# Design Philosophy

This project follows a staged approach:

1. Build a correct, minimal, end-to-end system
2. Ensure architectural clarity and modularity
3. Identify performance-critical paths
4. Incrementally optimize toward low-latency constraints

This methodology reflects real-world system design, where correctness and structure precede aggressive optimization.
