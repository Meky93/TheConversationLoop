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

#include <cassert>
//#include <cstddef>
#include <stdexcept>
//#include <string>
//#include <utility>


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

