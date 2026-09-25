// tests/p2/test_p2.cpp
//
// YOUR test suite goes here. At least 12 assert-based test cases — see
// spec §5 for the required categories and the sample test for the
// expected level of rigor.
//
// This file is a stub so the project builds out of the box; replace the
// body of main() with your own tests.

#include "../../include/core/conversation.h"
#include "../../include/core/sentinel_scanner.h"
#include "../../include/core/message.h"


#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <string>
//#include <utility>

void test_empty_conversation();
void test_empty_conversation_iteration();
void test_system_message_ordering();
void test_copy_constructor_deep_copy();
void test_copy_assignment_deep_copy();

int main() {
    test_empty_conversation();
    test_empty_conversation_iteration();
    test_system_message_ordering();


    return 0;
}


// Test cases for Conversation class and SentinelScanner class
void test_empty_conversation() {
    Conversation conversation;

    assert(conversation.size() == 0);
    assert(conversation.begin() == conversation.end());

    bool threw = false;

    try {
        conversation.at(0);
    } catch (const std::out_of_range&) {
        threw = true;
    }

    assert(threw);
}

void test_empty_conversation_iteration() {
    Conversation conversation;

    std::size_t count = 0;

    for (const Message& message : conversation) {
        (void)message;
        ++count;
    }

    assert(count == 0);
}

void test_system_message_ordering() {
    Conversation conversation;

    conversation.append(Message(Role::System, "You are helpful."));
    conversation.append(Message(Role::User, "Hello"));
    conversation.append(Message(Role::Assistant, "Hi"));

    assert(conversation.size() == 3);
    assert(conversation.at(0).role() == Role::System);
    assert(conversation.at(0).content() == "You are helpful.");
    assert(conversation.at(1).role() == Role::User);
    assert(conversation.at(2).role() == Role::Assistant);
}

void test_copy_constructor_deep_copy() {
    Conversation original;
    original.append(Message(Role::User, "hello"));
    original.append(Message(Role::Assistant, "hi"));

    Conversation copy(original);

    assert(copy.size() == original.size());
    assert(copy.at(0).content() == "hello");
    assert(copy.at(1).content() == "hi");

    // The two Conversation objects must own different arrays.
    assert(copy.begin() != original.begin());
}

void test_copy_assignment_deep_copy() {
    Conversation original;
    original.append(Message(Role::User, "hello"));

    Conversation assigned;
    assigned.append(Message(Role::Assistant, "old value"));

    assigned = original;

    assert(assigned.size() == 1);
    assert(assigned.at(0).role() == Role::User);
    assert(assigned.at(0).content() == "hello");
    assert(assigned.begin() != original.begin());

    // Self-assignment must be safe.
    assigned = assigned;

    assert(assigned.size() == 1);
    assert(assigned.at(0).content() == "hello");
}

void test_move_constructor() {
    Conversation original;
    original.append(Message(Role::User, "hello"));
    original.append(Message(Role::Assistant, "hi"));

    const Message* original_data = original.begin();

    Conversation moved(std::move(original));

    assert(moved.begin() == original_data);
    assert(moved.size() == 2);
    assert(moved.at(0).content() == "hello");

    assert(original.size() == 0);
    assert(original.begin() == original.end());
}

void test_move_assignment() {
    Conversation source;
    source.append(Message(Role::User, "source"));

    const Message* source_data = source.begin();

    Conversation destination;
    destination.append(Message(Role::Assistant, "old"));

    destination = std::move(source);

    assert(destination.begin() == source_data);
    assert(destination.size() == 1);
    assert(destination.at(0).content() == "source");

    assert(source.size() == 0);
    assert(source.begin() == source.end());
}

void test_growth_behavior() {
    Conversation conversation;

    for (int i = 0; i < 100; ++i) {
        conversation.append(
            Message(Role::User, "message " + std::to_string(i))
        );
    }

    assert(conversation.size() == 100);

    for (std::size_t i = 0; i < conversation.size(); ++i) {
        assert(conversation.at(i).content() ==
               "message " + std::to_string(i));
    }
}

std::string scan_all(
    SentinelScanner& scanner,
    const std::string& first,
    const std::string& second = "") {
    std::string result;

    auto first_out = scanner.feed(first);
    result += first_out.safe_text;

    if (!second.empty()) {
        auto second_out = scanner.feed(second);
        result += second_out.safe_text;
        assert(second_out.sentinel_found);
    }

    auto final_out = scanner.flush();
    result += final_out.safe_text;

    return result;
}

void test_scanner_clean_text() {
    SentinelScanner scanner("<|end_conversation|>");

    auto out = scanner.feed("Hello, world!");
    auto flushed = scanner.flush();

    assert(!out.sentinel_found);
    assert(!flushed.sentinel_found);
    assert(out.safe_text + flushed.safe_text == "Hello, world!");
}

void test_scanner_split_at_every_boundary() {
    const std::string sentinel = "<|end_conversation|>";
    const std::string text = "Goodbye." + sentinel;

    for (std::size_t split = 0; split <= text.size(); ++split) {
        SentinelScanner scanner(sentinel);

        auto first = scanner.feed(
            std::string_view(text).substr(0, split)
        );

        auto second = scanner.feed(
            std::string_view(text).substr(split)
        );

        auto flushed = scanner.flush();

        assert(!first.sentinel_found ||
               first.safe_text == "Goodbye.");

        assert(first.sentinel_found ||
               second.sentinel_found);

        assert(first.safe_text + second.safe_text + flushed.safe_text
               == "Goodbye.");
    }
}

void test_scanner_false_alarm() {
    SentinelScanner scanner("<|end_conversation|>");

    auto out = scanner.feed("<|end_world|>");
    auto flushed = scanner.flush();

    assert(!out.sentinel_found);
    assert(!flushed.sentinel_found);
    assert(out.safe_text + flushed.safe_text == "<|end_world|>");
}

void test_scanner_one_character_at_a_time() {
    const std::string sentinel = "<|end_conversation|>";
    const std::string text = "Goodbye." + sentinel;

    SentinelScanner scanner(sentinel);
    std::string safe_text;
    bool found = false;

    for (char character : text) {
        auto out = scanner.feed(std::string(1, character));
        safe_text += out.safe_text;
        found = found || out.sentinel_found;
    }

    auto flushed = scanner.flush();
    safe_text += flushed.safe_text;

    assert(found);
    assert(safe_text == "Goodbye.");
}

void test_scanner_bounded_stream_behavior() {
    const std::string sentinel = "<|end_conversation|>";
    SentinelScanner scanner(sentinel);

    std::string safe_text;

    for (int i = 0; i < 100000; ++i) {
        auto out = scanner.feed("x");
        safe_text += out.safe_text;
    }

    auto flushed = scanner.flush();
    safe_text += flushed.safe_text;

    assert(safe_text.size() == 100000);
}
