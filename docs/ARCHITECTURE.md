# 📄 ARCHITECTURE.md

# Architecture Overview

This document describes the internal architecture of the webserv-core HTTP server.

The system is designed around a single-threaded event-driven model using epoll (Linux) or poll (fallback) for I/O multiplexing.

---

# 🧠 High-Level Design

The architecture follows a **reactor pattern**:
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
---

# 🔁 Event Loop Model

The EventLoop is the central component of the system.

It is responsible for:

- Waiting for I/O events using `Poller`
- Dispatching events to the appropriate handlers
- Managing server sockets and client connections
- Coordinating lifecycle of connections

The loop runs in a single thread:

```cpp
while (running)
    runOnce(timeout);
```
This design ensures deterministic execution and avoids concurrency complexity.

---

# ⚡ Poller Abstraction

The Poller component abstracts OS-level I/O multiplexing:

- Uses epoll on Linux
- Uses poll as fallback

Responsibilities:

- Register file descriptors
- Modify interest (read/write events)
- Remove file descriptors
- Wait for events and translate them into `NetworkEvent`

This abstraction allows the system to remain portable while maintaining a consistent event model.

---

# 🔌 Connection Model

Each client connection is represented by a `Connection` object.

A Connection contains:

- Socket file descriptor
- Read buffer
- Write buffer
- State flags (read/write interest, closed state)

Responsibilities:

- Reading incoming data
- Storing partially received requests
- Writing responses back to the client
- Tracking connection state

---

# 🌐 HTTP Layer

The HTTP layer is composed of:

## HttpParser

Responsible for parsing raw TCP input into structured HTTP requests.

Features:

- Request line parsing
- Header parsing
- Content-Length handling
- Keep-alive detection

## HttpService

Implements routing logic based on:

- HTTP method
- Request target

It produces an `HttpResponse` based on predefined rules.

---

# 🔄 Request Flow

The full request lifecycle is:

1. Client connects to server socket  
2. EventLoop receives accept event  
3. Connection object is created  
4. Connection is registered in Poller  
5. Data arrives → read event triggered  
6. HttpParser processes buffer  
7. HttpService generates response  
8. Response is written back  
9. Connection is closed or reused (keep-alive)

---

# ⚙️ Design Decisions

## Single-threaded model

The system uses a single event loop to avoid:

- race conditions
- locking overhead
- thread synchronization complexity

This simplifies the design while still allowing high concurrency via non-blocking I/O.

---

## Epoll-based multiplexing

epoll is used on Linux for efficient scalability with large numbers of file descriptors.

A poll fallback is provided for portability.

---

## Separation of concerns

The architecture is divided into:

- Network layer (EventLoop, Poller)
- Protocol layer (HTTP parsing and routing)
- Application layer (main entry point)

This improves readability and maintainability.

--- 

# ⚖️ Trade-offs

## Simplicity over performance tuning

The implementation prioritizes clarity over:

- zero-copy parsing
- custom allocators
- advanced buffering strategies

## No multi-threading

The system is intentionally single-threaded to reduce complexity.

## Basic HTTP implementation

Only a minimal subset of HTTP is supported.

---

# 📌 Current Status & Future Improvements

This project is intentionally kept in a **minimal and modular state** in order to focus on the core concepts of event-driven networking and HTTP processing.

The current implementation prioritizes:
- clarity of the reactor pattern (epoll/poll-based event loop)
- separation between network handling and HTTP logic
- a minimal HTTP layer to demonstrate full request/response connectivity

As a result, the HTTP module remains intentionally simple, serving primarily as a proof of integration rather than a fully-featured web framework.

---

## Planned Improvements

Several areas have been identified for future development:

- **Low-latency optimized design**
  (reduction of syscall overhead, improved event batching, tighter loop control)

- **Memory-pool / zero-allocation mindset**
  (reducing dynamic allocations in the connection and HTTP processing layers)

- **Production-grade separation of concerns**
  (stronger isolation between parsing, routing, and network layers)

---

## Future Extensions

Once the core networking and HTTP modules are further optimized, the architecture could be extended toward a more complete server model, including:

- **Session management layer**
  (connection persistence, state tracking, user/session abstraction)

- **Configuration system**
  (runtime or file-based server configuration similar to Nginx-style setups)

These extensions would progressively move the project closer to a fully configurable, production-like HTTP server architecture.

---

## Design Philosophy

The current state of the project reflects a deliberate learning-oriented approach:
build a minimal but complete system first, then progressively optimize and extend it toward production-level constraints.