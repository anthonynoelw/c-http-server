# HTTP Protocol

This document describes the parts of HTTP/1.1 that this server implements. It is a practical summary, not a replacement for the specification.

**References**

- [RFC 9110 – HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110)
- [RFC 9112 – HTTP/1.1](https://www.rfc-editor.org/rfc/rfc9112)

## Contents

- [Basic Principle](#basic-principle)
- [Message Structure](#message-structure)
- [Request](#request)
- [Response](#response)
- [Methods](#methods)
- [Status Codes](#status-codes)
- [Important Headers](#important-headers)
- [Body and Message Length](#body-and-message-length)
- [Connection Behavior](#connection-behavior)
- [Error Handling](#error-handling)
- [Scope of This Server](#scope-of-this-server)

## Basic Principle

HTTP is a **text-based request/response protocol** at the application layer. It usually runs over TCP (default port 80, 443 with TLS).

- The **client** sends a request.
- The **server** replies with exactly one response.
- HTTP is **stateless**: every request must contain all the information needed to process it.

```
Client                         Server
  |  ---- Request ---------->    |
  |  <---- Response ---------    |
```

## Message Structure

Requests and responses follow the same basic structure:

```
Start line CRLF
Header-Field: Value CRLF
Header-Field: Value CRLF
CRLF
[Body]
```

- Lines end with **CRLF** (`\r\n`).
- An **empty line** separates the headers from the body.
- The body is optional.

## Request

### Request Line

```
METHOD SP Request-Target SP HTTP-Version CRLF
```

Example:

```
GET /index.html HTTP/1.1
```

| Part             | Meaning                                           |
| ---------------- | ------------------------------------------------- |
| Method           | Requested action, e.g. `GET`                      |
| Request target   | Resource, usually a path plus an optional query   |
| HTTP version     | `HTTP/1.1`                                        |

In practice the request target usually has the form `/path?key=value`. The query string begins after the `?`.

### Complete Example

```http
POST /api/users HTTP/1.1
Host: localhost:8080
Content-Type: application/json
Content-Length: 24

{"name":"Anna","age":30}
```

> `Content-Length` must always match the number of bytes in the body exactly (here 24).

## Response

### Status Line

```
HTTP-Version SP Status-Code SP Reason-Phrase CRLF
```

Example:

```
HTTP/1.1 200 OK
```

The reason phrase is purely informational. Clients may only rely on the numeric status code.

### Complete Example

```http
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
Content-Length: 13
Connection: close

Hello, World!
```

## Methods

| Method    | Purpose                                  | Safe | Idempotent | Body in request |
| --------- | ---------------------------------------- | :--: | :--------: | :-------------: |
| `GET`     | Read a resource                          | yes  |    yes     |       no        |
| `HEAD`    | Like `GET`, but without a response body  | yes  |    yes     |       no        |
| `POST`    | Send data for processing                 |  no  |     no     |       yes       |
| `PUT`     | Replace/create a resource in full        |  no  |    yes     |       yes       |
| `DELETE`  | Delete a resource                        |  no  |    yes     |     unusual     |
| `OPTIONS` | Query the supported methods              | yes  |    yes     |     unusual     |

- **Safe**: does not change server state.
- **Idempotent**: executing it multiple times has the same result as executing it once.

Method names are **case-sensitive** (`GET`, not `get`).

## Status Codes

Status codes are divided into five classes:

| Class  | Meaning               |
| ------ | --------------------- |
| `1xx`  | Informational         |
| `2xx`  | Success               |
| `3xx`  | Redirection           |
| `4xx`  | Client error          |
| `5xx`  | Server error          |

### Relevant for This Project

| Code  | Name                            | Usage                                                |
| ----- | ------------------------------- | ---------------------------------------------------- |
| `200` | OK                              | Request succeeded, response contains content         |
| `201` | Created                         | A resource was created                               |
| `204` | No Content                      | Success, intentionally without a body                |
| `301` | Moved Permanently               | Resource permanently moved (`Location` header)       |
| `400` | Bad Request                     | Request is syntactically malformed                   |
| `404` | Not Found                       | Resource does not exist                              |
| `405` | Method Not Allowed              | Method not allowed for this resource (`Allow`)       |
| `408` | Request Timeout                 | Client did not send the request in time              |
| `411` | Length Required                 | `Content-Length` is missing                          |
| `413` | Content Too Large               | Body exceeds the limit                               |
| `414` | URI Too Long                    | Request target exceeds the limit                     |
| `431` | Request Header Fields Too Large | Headers exceed the limit                             |
| `500` | Internal Server Error           | Unexpected error in the server                       |
| `501` | Not Implemented                 | Method is not supported                              |
| `505` | HTTP Version Not Supported      | HTTP version other than `HTTP/1.1` (or `1.0`)        |

## Important Headers

Headers have the form `Name: Value`. Names are **case-insensitive**; leading and trailing whitespace around the value is ignored.

### Request Headers

| Header           | Meaning                                                        |
| ---------------- | -------------------------------------------------------------- |
| `Host`           | Target host and port. **Mandatory in HTTP/1.1**.               |
| `User-Agent`     | Identifier of the client                                       |
| `Accept`         | Media types the client accepts                                 |
| `Content-Type`   | Media type of the request body                                 |
| `Content-Length` | Length of the request body in bytes                            |
| `Connection`     | Connection control (`close`, `keep-alive`)                     |

### Response Headers

| Header           | Meaning                                                        |
| ---------------- | -------------------------------------------------------------- |
| `Content-Type`   | Media type of the response body                                |
| `Content-Length` | Length of the response body in bytes                           |
| `Connection`     | Connection control                                             |
| `Date`           | Time of the response (HTTP date, GMT)                          |
| `Server`         | Identifier of the server software                              |
| `Location`       | Target for redirects                                           |
| `Allow`          | Allowed methods (for `405` and `OPTIONS`)                      |

### Common Media Types

| Extension      | `Content-Type`                     |
| -------------- | ---------------------------------- |
| `.html`        | `text/html; charset=utf-8`         |
| `.css`         | `text/css`                         |
| `.js`          | `text/javascript`                  |
| `.json`        | `application/json`                 |
| `.txt`         | `text/plain; charset=utf-8`        |
| `.png`         | `image/png`                        |
| `.jpg`, `.jpeg`| `image/jpeg`                       |
| unknown        | `application/octet-stream`         |

## Body and Message Length

The receiver must know where a message ends. There are two common mechanisms for this:

1. **`Content-Length`**: the body has exactly that many bytes.
2. **`Transfer-Encoding: chunked`**: the body is sent in chunks.

```
HTTP/1.1 200 OK
Transfer-Encoding: chunked

5
Hello
6
 World
0

```

Each chunk begins with its length in hexadecimal, followed by CRLF and the data. A chunk of length `0` ends the body.

Without either of these, a request has **no body**. For a response, the end could only be detected by closing the connection.

Security note: if `Content-Length` and `Transfer-Encoding` are both set, or `Content-Length` appears multiple times with different values, the request must be **rejected** (`400`) to prevent request smuggling.

## Connection Behavior

- **HTTP/1.0**: by default the connection is closed after each response.
- **HTTP/1.1**: connections are **persistent** (keep-alive) by default. The client or server can request closing with `Connection: close`.

With persistent connections, multiple requests can run one after another over the same TCP connection. Responses must be sent in the same order as the requests.

A simple server can end every response with `Connection: close` and close the connection afterwards. This simplifies the implementation considerably.

## Error Handling

The server must handle malformed or malicious requests safely.

| Situation                                        | Response                              |
| ------------------------------------------------ | ------------------------------------- |
| Request line syntactically invalid               | `400 Bad Request`                     |
| Header without colon or with invalid name        | `400 Bad Request`                     |
| `Host` header missing (HTTP/1.1)                 | `400 Bad Request`                     |
| Unknown method                                   | `501 Not Implemented`                 |
| Known method, not allowed for the path           | `405 Method Not Allowed` + `Allow`    |
| Path not found                                   | `404 Not Found`                       |
| Request line too long                            | `414 URI Too Long`                    |
| Headers too large                                | `431 Request Header Fields Too Large` |
| Body too large                                   | `413 Content Too Large`               |
| Unsupported HTTP version                         | `505 HTTP Version Not Supported`      |
| Internal error                                   | `500 Internal Server Error`           |

Basic rules:

- All input is **untrusted**. Always check lengths before memory is allocated or copied.
- After a protocol error, close the connection with `Connection: close`.
- Error responses must not contain internal details (paths, memory addresses, stack traces).

For details, see [security/threat-model.md](security/threat-model.md) and [security/hardening.md](security/hardening.md).

## Scope of This Server

The server deliberately implements only a subset of HTTP/1.1.

| Feature                          | Status        |
| -------------------------------- | ------------- |
| `GET`, `HEAD`                    | planned       |
| `POST`                           | planned       |
| `PUT`, `DELETE`, `OPTIONS`       | not planned   |
| `Content-Length` bodies          | planned       |
| `Transfer-Encoding: chunked`     | not planned   |
| Keep-alive                       | not planned   |
| Pipelining                       | not planned   |
| TLS (HTTPS)                      | not planned   |
| HTTP/2, HTTP/3                   | not planned   |


## Relation to the Architecture

The protocol steps correspond to the components from [architecture.md](architecture.md):

```
Socket Listener -> Request Parser -> Router -> Handler -> Response Builder -> Socket Response
```

| Component         | Protocol responsibility                                              |
| ----------------- | -------------------------------------------------------------------- |
| Socket Listener   | Accept TCP connection, read bytes                                    |
| Request Parser    | Parse and validate request line, headers, and body                   |
| Router            | Map method and path to a handler (`404`, `405`)                      |
| Handler           | Execute application logic                                            |
| Response Builder  | Serialize status line, headers, and body                             |
| Socket Response   | Send bytes, close connection                                         |
