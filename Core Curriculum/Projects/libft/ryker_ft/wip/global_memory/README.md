# Global pointer container — WIP backup

Study backup of the proposed function-local static container. Not integrated
into libft headers or its build: this directory is below the current wildcard depth.

Get the shared container with ryker_ft_global_container(), then pass it and a
payload pointer to ryker_ft_global_append(). Append returns 1 on success, 0 on
failure; NULL payloads are rejected. Allocation failure leaves existing entries
intact. The accessor returns a pointer rather than a shallow struct copy.

The array stores pointers, not copies of their targets. There are no type tags.
Callers must keep targets alive, retrieve them using the correct original type,
and free any owned payload allocations themselves. Clear frees only the array
and resets its pointer and count; previously saved array pointers become invalid.

This preserves the shared mutable state idea; it is not thread-safe or a complete
memory manager. Append/clear also accept a caller-owned zero-initialized container.
