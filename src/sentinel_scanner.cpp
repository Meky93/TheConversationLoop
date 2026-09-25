#include "../include/core/sentinel_scanner.h"

// Constructor that initializes a SentinelScanner object, given a specific sentinel string to watch for.
SentinelScanner::SentinelScanner(std::string sentinel)
    : sentinel_(std::move(sentinel)) {
}

// Feeds the scanner with text in "chunks." It returns the safe text that does not contain the sentinel (or part of it)
// and returns true/false (boolean) for .sentinel_found (this becomes truew only when the whole sentinel has been found)..
SentinelScanner::Out SentinelScanner::feed(std::string_view chunk) {
    if (sentinel_found_) {
        return {"", true};
    }

    //
    std::string combined = pending_;
    combined.append(chunk.data(), chunk.size());

    const std::size_t position = combined.find(sentinel_);

    // If the sentinel is found in the combined string, we set sentinel_found_ to true, clear the pending_ string, and return the safe text.
    if (position != std::string::npos) {
        sentinel_found_ = true;
        pending_.clear();

        return {
            combined.substr(0, position),
            true
        };
    }

    if (sentinel_.empty()) {
        return {combined, false};
    }

    // If the sentinel is not found, we keep the last (sentinel_.size() - 1) characters in pending_ and return the rest as safe text.
    const std::size_t keep =
        std::min(combined.size(), sentinel_.size() - 1);

    const std::size_t safe_length = combined.size() - keep;

    Out result{
        combined.substr(0, safe_length),
        false
    };

    pending_ = combined.substr(safe_length);
    return result;
}

// Called after the stream ends to release any text still being held back.
SentinelScanner::Out SentinelScanner::flush() {
    if (sentinel_found_) {
        pending_.clear();
        return {"", true};
    }

    Out result{
        std::move(pending_),
        false
    };

    pending_.clear();
    return result;
}


