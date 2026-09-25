#pragma once
#include <string>

// The Role class represents the role of a message sender in a conversation.
enum class Role { System, User, Assistant };

// The Message class contains objects that represent single messages within a conversation. Each with a specific role. (System, User, or Assistant).
class Message {
public:
    // Default-constructs an empty System message with empty content.
    // Needed so Conversation can allocate raw array slots before
    // append() fills them in.
    Message();

    Message(Role role, std::string content);

    Role               role()    const noexcept;  // Who sent this message.
    const std::string& content() const noexcept;  // The message text.

private:
    Role        role_;
    std::string content_;
};