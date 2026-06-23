# DISTRIBUTED ORCHESTRA SIMULATOR

## PROBLEM STATEMENT
The project aims to design and implement a client-server-based system that simulates a co-ordinated musical performance.
- In the system, the instrumentalists (clients) follow a pre-defined sequence of notes, stored as time-indexed lines of text.
- The server synchronises their execution by broadcasting periodic ‘ticks,’ like counting time with a metronome. This ensures co-ordination among performers, similar to a real-world orchestra.
- The system uses role-based control, allowing an administrator to start and end a performance, which controls the server’s broadcasting.
- Additionally, multiple clients may concurrently interact with shared files (the sheet music), requiring mechanisms to maintain data consistency.
- Finally, the system uses socket programming to enable communication and co-ordination between the distributed processes.

## CONCEPT IMPLEMENTATION EXPLANATIONS
### 1. ROLE-BASED AUTHORISATION
- The system defines 2 roles: Conductor (administrator) and Instrumentalist (user).
- Administrators are responsible for managing pieces and rehearsals, while users can view and edit their parts and participate in rehearsals.

### 2. SOCKET PROGRAMMING
- The system uses a TCP-based client-server architecture to enable communication between the server and instrumentalists and conductor (admin).
- Admins send START and STOP requests to the server to start/stop a rehearsal.
- Clients send JOIN requests to the server to participate in a rehearsal.
- Synchronisation is achieved via a centralised timing mechanism controlled by the server.
- The server maintains multiple client connections and broadcasts synchronised ticks to all connected instrumentalists.
- The instrumentalists print the note(s) at each tick.

### 3. CONCURRENCY CONTROL & DATA CONSISTENCY
- Concurrency arises from multiple client processes accessing shared sheet music files.
- To prevent race conditions, mandatory file-level locking is implemented via the fcntl system call.
- A write lock is acquired before appending notes and released afterwards, ensuring mutual exclusion and maintaining data consistency.
- The server ensures that all clients operate on a consistent timeline and reset system state after each rehearsal.

## OUTPUT SCREENSHOTS
<figure>
<img width="712" height="452" alt="image" src="https://github.com/user-attachments/assets/1b7b0d7b-2ae2-4b33-9ebb-a970022194ad" />
<figcaption> Figure 1: Creation of Piece Directory by Administrator </figcaption>
</figure>

<figure>
<img width="756" height="938" alt="image" src="https://github.com/user-attachments/assets/92ffb641-158e-4019-9e55-7c3fea0f1fec" />
<figcaption> Figure 2: Adding a Note to a Part File; file locking ensures concurrency control </figcaption>
</figure>

<figure>
<img width="1591" height="840" alt="image" src="https://github.com/user-attachments/assets/91ac070c-830d-4790-95a9-703ed44ab99f" />
<figcaption> Figure 3: Ready State: Instrumentalists join rehearsal and enter ready state. </figcaption>
</figure>

<figure>
<img width="1792" height="704" alt="image" src="https://github.com/user-attachments/assets/a643d86c-7782-4b7a-835f-c783fbbe818c" />
<figcaption> Figure 4: Synchronised Tick Broadcast from Server to Instrumentalists </figcaption>
</figure>

<figure>
<img width="1792" height="704" alt="image" src="https://github.com/user-attachments/assets/dce3a37b-bb67-476f-832c-00332666bc4e" />
<figcaption> Figure 5: A client joining mid-rehearsal synchronises with the current tick and executes accordingly </figcaption>
</figure>

## CHALLENGES FACED and SOLUTIONS
### 1. BLOCKING SOCKET BEHAVIOUR
- Initially, the server used a blocking accept() call, which prevented the system from performing other operations like broadcasting ticks.
- A transition to non-blocking sockets, at first, introduced further issues where the server would skip execution when no connections were present.
- These were resolved by restructuring the main loop to continuously process both incoming connections and system logic.

### 2. HANDLING CLIENT DISCONNECTS
- When clients disconnected at the end of a rehearsal, the server was terminated due to a broken pipe exception.
- This was resolved by ignoring the SIGPIPE signal and implementing logic to remove disconnected instrumentalists from the array of active users, allowing for execution to continue without interruption.

### 3. FILE ACCESS DESIGN
- The initial design had local file access on each client and the server too, breaking the distributed model during a performance.
- This required a redesign, where the server became the source of truth, sending part files to clients during the join operation.
- The change ensured that the system function aligned more towards a true client-server architecture.

