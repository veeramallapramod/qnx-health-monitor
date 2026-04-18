# QNX Health Monitor

A health monitoring system built for QNX using Pulse-based IPC.

## Files
- `monitor_server.c` — Server that watches for heartbeats
- `heartbeat_client.c` — Client that sends periodic heartbeats

## How It Works
- The server creates a channel and starts a 2-second timer
- The client connects and sends a heartbeat pulse every 2 seconds
- If no heartbeat is received after 3 ticks, the server raises an alert
- After 10 heartbeats, the client simulates a failure/crash

## How to Compile (QNX)
```bash
qcc -o monitor_server monitor_server.c
qcc -o heartbeat_client heartbeat_client.c
```

## How to Run
1. Run the server first → it will print its PID and Channel ID
2. Run the client → enter the server's PID and Channel ID when prompted
