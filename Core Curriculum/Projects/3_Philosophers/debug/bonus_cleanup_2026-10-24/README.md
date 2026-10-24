# Bonus partial-initialization cleanup

`main_bonus.c` now zero-initializes t_sim_bonus, parses input, calls init_bonus,
and always calls cleanup_bonus before reporting its final status. Successful
setup still returns ERR_NOT_IMPLEMENTED: no philosopher processes run yet.

cleanup_bonus first stops/reaps tracked children, then releases printing,
take_pair and forks in reverse initialization order. NULL and SEM_FAILED handles
are skipped. Since opening uses O_CREAT | O_EXCL, a valid handle indicates this
run successfully created the corresponding name. A failed open never authorizes
unlinking that name.

For each owned semaphore, cleanup calls sem_close(handle), clears the pointer on
success, then sem_unlink(name). On close failure it retains the pointer and name;
other semaphores are still attempted. An unlink failure is reported and can leave
a stale name; cleanup does not pretend it was removed. ENOENT means the name is
already absent. The original initialization error takes priority over subsequent
semaphore cleanup errors; otherwise the first semaphore cleanup error is returned.
If stopping/reaping children fails, cleanup returns that failure before touching
semaphores, retaining resources for unresolved child cleanup.

Added ERR_SEM_CLOSE / ERR_SEM_UNLINK and their messages, and shared name macros so
opening and unlinking use identical names. Semaphore messages have a small separate
lookup to keep the functions within Norm's line limit. Removed duplicate blank
lines flagged by Norm in the bonus sources. Mandatory sources are unchanged.

## Verification

- Bonus build and Norm passed.
- Ten isolated harness cases passed: normal setup/cleanup, allocation failure,
  failure at each of three sem_open calls, a pre-existing name at each position,
  sem_unlink failure, and sem_close failure.
- Pre-existing semaphores retained their names and original token count.
- Normal/open-failure cleanup was safe to call again after resources were cleared.
- Five Memcheck cases passed: normal, partial open failure, name collision,
  unlink failure and close failure. The harness releases deliberately retained
  resources after verifying error behavior; this does not imply failed cleanup
  itself removed every resource.

Run `python3 debug/bonus_cleanup_2026-10-24/run_checks.py` from the project root.
The harness uses unique test names and linker wrappers to inject failures; it
does not remove or overwrite the application's normal semaphore names.
The harness-only calls such as sem_getvalue are outside submission sources.
