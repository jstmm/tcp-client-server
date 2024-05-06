# TCP Client Server

TCP connection between client and server programs using glibc's socket implementation

## Install

Build using:

```bash
$ make
```

### Single connection 
Start server with:

```bash
$ ./build/server
```

And client with:

```bash
$ ./build/client 127.0.0.1
```

### Multi-connection using select()
Start server with:

```bash
$ ./build/server_select
```

And client with:

```bash
$ nc 127.0.0.1 5555 # TODO: to replace with client executable
```

Then type the text to be sent.

Debugging can be done using VSCode launch configurations.
