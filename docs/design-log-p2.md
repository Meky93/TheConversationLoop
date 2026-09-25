# Design Log — Project 2

## Growth factor and amortized cost

"Conversations" are implemented as a manually managed array of "Message" objects. It stores a pointer to the allocated array, the number of valid messages, and the total capacity. The first conversation has a null pointer, size zero, and capacity zero.

When the array is full, the implementation allocates a new array with twice the previous capacity. The first allocation has capacity one, and later capacities are 2, 4, 8, 16, and so on. Existing messages are copied into the new array, the new message is added, and the old array is deleted. When there is unused capacity, "append" adds the message directly without allocating.

Doubling gives amortized constant time appending. Although one append that triggers reallocation takes O(n) time because it copies the existing messages, reallocations do not happen on every append. For n insertions, the number of copied messages is bounded by the geometric sum:

1 + 2 + 4 + 8 + ... + 2^k

where 2^k is less than or approximately equal to n. This sum is less than 2n, so all copying caused by reallocations takes O(n) total time. The n ordinary insertions also take O(n) total time. Therefore, n appends take O(n) total time, which means each append costs O(1) amortized. The tradeoff is that the array may temporarily have unused capacity, but this avoids the much worse O(n^2) behavior of increasing the capacity by one each time.

## Rule of Five evidence

"Conversation" owns its dynamically allocated "Message" array. Its destructor releases the array with "delete[]". The copy constructor and copy-assignment operator perform deep copies. They allocate a separate array and copy every stored "Message", so two conversations never share ownership of the same buffer. This prevents double frees and guarantees that modifying or destroying one conversation does not invalidate the other.

The copy assignment operator checks for self assignment. It allocates and fills the replacement array before deleting the destination's old array. This means that if allocation or copying throws an exception, the original destination remains unchanged and the temporary allocation is cleaned up.

The move constructor and move assignment operator transfer ownership of the existing array pointer instead of copying every element. After a move, the destination contains the original data pointer, size, and capacity. The moved-from conversation is reset to a valid empty state with a null pointer, zero size, and zero capacity. This makes it safe to destroy or assign to later. Move assignment first releases any storage already owned by the destination, preventing a memory leak.

Tests verify deep copying, pointer acquiring move behavior, self assignment, growth across multiple reallocations, and safe use of moved objects. AddressSanitizer is also used to detect leaks, double frees, and invalid memory access.

## Sentinel scanner: bounded pending proof

"SentinelScanner" processes model output in chunks. Since a chunk boundary may occur in the middle of "<|end_conversation|>", the scanner cannot immediately print every received character. It combines the previous pending suffix with the new chunk and searches for the complete sentinel.

If the sentinel is found, the scanner returns only the text before it and reports that the sentinel was found. The sentinel itself is never returned as safe text. If the sentinel is not found, the scanner emits all characters except the final "sentinel.size() - 1" characters. Those trailing characters are stored in "pending_", because they could be the beginning of a sentinel completed by the next chunk.

Thus, "pending_" never contains more than "sentinel.size() - 1" characters. If it contained a longer suffix, the earlier characters could no longer be part of a sentinel beginning at the end and would be safe to emit. When the stream ends, "flush()" releases the remaining pending characters because no later chunk can complete the sentinel.

This design uses bounded extra memory regardless of the total response length and handles whole chunk, split chunk, and single character inputs.

## What I would change differently

If raw arrays were not required for this assignment I'd use a standard container from the standard library. This would remove some of the headache around memory management and might help remove the risk of overlooking a memory leak.
