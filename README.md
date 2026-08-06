*This project has been created as part of the 42 curriculum by hlaaz.*

# Codexion

## Description

Codexion is a multithreaded simulation written in C, inspired by the classic
Dining Philosophers problem and reframed around a coding theme. A configurable
number of **coders** sit in a circular co-working hub, sharing a limited pool
of **USB dongles** placed on a central table (one dongle between each pair of
neighboring coders). Each coder repeatedly cycles through three phases:

- **compiling** — requires holding two dongles simultaneously (left and right neighbor)
- **debugging** — a fixed-duration phase requiring no resources
- **refactoring** — a fixed-duration phase requiring no resources, after which the coder immediately attempts to compile again

Coders do not communicate directly; they only compete for dongles through
mutex-protected shared state. If a coder does not start compiling within
`time_to_burnout` milliseconds of the start of its last compile (or the start
of the simulation), it **burns out**, and the simulation stops immediately.
Otherwise, the simulation stops once every coder has compiled at least
`number_of_compiles_required` times.

The goal of the project is to correctly implement resource sharing, fair
scheduling, and precise timing across concurrent threads using POSIX threads,
mutexes, and condition variables — while avoiding deadlocks, data races, and
starvation.

## Instructions

### Compilation

```bash
make
```

Compiles the project with `-Wall -Wextra -Werror -pthread` and produces a
binary named `codexion` at the root of the repository.

Other Makefile rules:

```bash
make clean   # remove object files
make fclean  # remove object files and the binary
make re      # fclean + rebuild
```

### Execution

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coder threads and number of dongles |
| `time_to_burnout` | Max time (ms) a coder can go without starting a compile before burning out |
| `time_to_compile` | Duration (ms) of the compiling phase |
| `time_to_debug` | Duration (ms) of the debugging phase |
| `time_to_refactor` | Duration (ms) of the refactoring phase |
| `number_of_compiles_required` | Simulation stops once every coder reaches this compile count |
| `dongle_cooldown` | Time (ms) a dongle stays unavailable after being released |
| `scheduler` | Dongle arbitration policy: `fifo` or `edf` |

Example:

```bash
./codexion 5 2000 200 200 200 10 0 fifo
```

All arguments are mandatory and validated: negative numbers, non-integer
strings, and an unrecognized scheduler are rejected with an explicit error
message and a non-zero exit code.

## Resources

- POSIX Threads Programming (LLNL) — reference for `pthread_create`,
  `pthread_mutex_t`, and `pthread_cond_t` usage and semantics
- `man pthread_cond_timedwait`, `man pthread_mutex_lock`, `man clock_gettime`
  — manual pages consulted for correct condition-variable and timed-wait usage
- The Dining Philosophers problem (E. W. Dijkstra) and Coffman's conditions
  for deadlock — the classical background this project's core resource-sharing
  challenge is built on
- Earliest Deadline First (EDF) scheduling — general real-time scheduling
  literature, adapted here for dongle arbitration

**How AI was used:** An AI was used throughout development
as a debugging and design-review partner, not as a code generator. Specific
uses:
- Diagnosing a circular-wait deadlock caused by acquiring both dongles in a
  fixed left-then-right order for every coder, and confirming the fix (a
  total ordering over dongles, with the last coder acquiring in reverse)
  against the classical proof that a total resource order prevents circular
  wait
- Identifying an unprotected read of a shared timestamp (`last_compile_start`)
  used by the EDF priority calculation, and confirming the correct mutex to
  protect it with
- Reviewing `helgrind`/`drd` output to distinguish a real synchronization bug
  from a known `pthread_cond_timedwait` false-positive pattern in Helgrind
- Reviewing argument-parsing logic for integer-overflow and truncation edge
  cases (e.g., values exceeding `INT_MAX` silently wrapping into a valid
  range before validation)
- General code review of thread-safety, log-ordering guarantees, and
  discussion of the project's official evaluation/grading criteria
All AI-assisted suggestions were tested, verified with `drd`/`helgrind`, and
understood before being integrated; no code was accepted without being able
to explain what it does and why.

## Blocking cases handled

- **Deadlock prevention (Coffman's conditions):** the classic Dining
  Philosophers circular-wait scenario is prevented by imposing a strict total
  order on dongle acquisition. Every coder except the last acquires its
  lower-indexed dongle before its higher-indexed one; the last coder acquires
  in the reverse order. Because every coder in the ring acquires dongles in
  ascending index order, a circular wait (and therefore deadlock) is
  structurally impossible, independent of thread scheduling.
- **Starvation prevention:** dongle requests are served through a strict
  ordering policy — arrival order under `fifo`, or earliest burnout deadline
  under `edf` — so no coder can be indefinitely passed over while another
  repeatedly cuts in line. Waiting coders re-check their position and the
  simulation's running state on every wakeup rather than busy-waiting or
  reacquiring blindly.
- **Cooldown handling:** each dongle tracks an `available_at` timestamp set on
  release (`current_time_ms() + dongle_cooldown`). A coder attempting to
  acquire a dongle waits — via `pthread_cond_timedwait` with a deadline set to
  `available_at` — until both the cooldown has elapsed and it is at the front
  of that dongle's waiting queue.
- **Precise burnout detection:** a dedicated monitor thread polls every
  coder's `last_compile_start` at a short, fixed interval and compares it
  against `time_to_burnout`. Once a coder exceeds its deadline, the monitor
  sets a shared `running` flag to `0` under mutex protection *before* logging
  the burnout message and waking all waiting coders, guaranteeing the
  "burned out" line is the last line printed — no coder thread logs any
  further state transition after the flag is cleared, since every state
  change is preceded by a check of that flag.
- **Log serialization:** every log line is printed under a single dedicated
  mutex (`log_mutex`), guaranteeing that two threads can never interleave
  partial lines and that timestamps appear in a globally consistent order
  relative to the lock's acquisition order.

## Thread synchronization mechanisms

- **`pthread_mutex_t state_mutex`** protects all simulation-wide mutable
  state shared across coder threads and the monitor: the `running` flag,
  each coder's `last_compile_start`, and each coder's `compiles_done`
  counter. Every read and every write to these fields — from
  `compile()`, `check_burnout()`, `check_compiles()`, `get_priority()`, and
  `coder_done()` — is wrapped in a lock/unlock pair on this mutex, which is
  what allows `drd`/`helgrind` to confirm no data race exists on this state
  even under heavy contention.
- **`pthread_mutex_t mutex` (per dongle)** protects each dongle's own
  `available` flag, `available_at` cooldown timestamp, and its small
  fixed-size waiting queue. A coder only ever queues on a dongle immediately
  before waiting on it, and never holds a dongle's mutex while attempting to
  acquire another dongle's mutex or `state_mutex`, which avoids any
  lock-ordering cycle between the two.
- **`pthread_cond_t cond` (per dongle)** is used with `pthread_cond_timedwait`
  so a waiting coder is woken either when the dongle is released (via
  `pthread_cond_broadcast` from `release_dongle`, called while holding the
  dongle's mutex) or when its cooldown naturally expires — preventing the
  otherwise-possible lost-wakeup scenario where a coder blocks indefinitely
  waiting for a cooldown to end that no other thread will ever explicitly
  signal for.
- **`pthread_mutex_t log_mutex`** serializes all `printf` calls across coder
  threads and the monitor thread, guaranteeing atomic, non-interleaved log
  lines regardless of how many threads log concurrently.
- **Thread-safe coder/monitor communication:** the monitor never touches a
  coder's dongles or queue directly; it only reads coder state under
  `state_mutex` and, on stopping the simulation, clears `running` and
  broadcasts on every dongle's condition variable so that any coder currently
  blocked in `pthread_cond_timedwait` wakes up, re-checks `running`, and exits
  its acquisition loop promptly instead of waiting out its full timeout.
