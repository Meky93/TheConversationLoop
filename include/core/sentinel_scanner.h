#include <string>

// Objects of the SentinelScanner class are used to scan a stream of text looking for a specific string. 
// It holds back characters that could potentially be part of the sentinel until it can determine whether 
// the sentinel has been fully seen or not.
class SentinelScanner {
public:
    explicit SentinelScanner(std::string sentinel);

    struct Out {
        std::string safe_text;
        bool sentinel_found;
    };

    // Feed the next chunk. Returns text guaranteed NOT to be part of
    // the sentinel (safe to print immediately) and whether the
    // sentinel has now been fully seen.
    Out feed(std::string_view chunk);

    // Call once, after the stream ends, to release any text still
    // being held back.
    Out flush();

private:
    std::string sentinel_;
    std::string pending_;   // holds back at most sentinel_.size() - 1
    bool sentinel_found_ = false;
                             // trailing characters that could still
                             // become the start of the sentinel
};