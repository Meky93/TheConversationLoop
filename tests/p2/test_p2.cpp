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

