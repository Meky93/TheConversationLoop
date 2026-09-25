// src/message.cpp
#include "../include/core/message.h"

// Default constructor that initializes an empty System message with empty content.
Message::Message()
    : role_(Role::System), content_() {
}

// Constructor that initializes a Message object when the role and content is specified.
Message::Message(Role role, std::string content)
    : role_(role), content_(content) {
}

//  Returns the role of the message sender.
Role Message::role() const noexcept {
    return role_;
}

// Returns the actual string (content) in the Message object.content of the message.
const std::string& Message::content() const noexcept {
    return content_;
}