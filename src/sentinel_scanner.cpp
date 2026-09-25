#include "../include/core/sentinel_scanner.h"

SentinelScanner::SentinelScanner(std::string sentinel)
    : sentinel_(std::move(sentinel)) {
}

SentinelScanner::Out SentinelScanner::feed(std::string_view chunk) {
    if (sentinel_found_) {
        return {"", true};
    }

    std::string combined = pending_;
    combined.append(chunk.data(), chunk.size());

    const std::size_t position = combined.find(sentinel_);

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


