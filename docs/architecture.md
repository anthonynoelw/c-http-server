# Architecture

This document describes the high-level architecture of the C HTTP server and the responsibilities of each component.

## Overview

The server processes each request through a linear pipeline:

```
Sockets -> Request Parser -> Router -> Handler -> Response Builder -> Sockets
```

A connection is accepted by the socket layer, its raw bytes are parsed into a structured request, the router selects the appropriate handler, the handler produces a result, and the response builder serializes that result back onto the connection.

## Components

### Sockets

Owns all network I/O. Responsible for creating, binding, and listening on the server socket, accepting client connections, reading raw request bytes, writing response bytes, and closing connections. No other component interacts with the network directly.

### Request Parser

Converts raw bytes received from the socket into a structured request representation. Parses the request line (method, target, protocol version), headers, and, where present, the body. Malformed input is rejected at this stage with an appropriate error status rather than being passed downstream.

### Router

Maps a parsed request to a handler based on its method and target path. When no route matches, the router yields a `404 Not Found` result; when the path matches but the method is not supported, it yields `405 Method Not Allowed`.

### Handler

Implements the application logic for a given route. A handler receives the parsed request and produces a response description (status, headers, and body). Handlers do not perform socket I/O or format the wire representation themselves.

### Response Builder

Serializes a response description into a valid HTTP response: status line, headers (such as `Content-Length` and `Content-Type`), and body. The resulting bytes are handed to the socket layer for transmission.

## Design Principles

- **Separation of concerns:** each stage has a single responsibility and communicates with its neighbors through well-defined data structures.
- **Isolated I/O:** only the socket layer performs network operations, which keeps the remaining components straightforward to test.
- **Early failure:** invalid requests are detected and answered as soon as possible in the pipeline.

For protocol-level details, see [http-protocol.md](http-protocol.md).
