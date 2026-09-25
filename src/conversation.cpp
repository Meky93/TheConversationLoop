#include "../include/core/conversation.h"
#include <stdexcept>


// Default constructor that initializes an empty Conversation object with no allocated storage.
Conversation::Conversation() = default;

// Destructor that releases all owned Message storage.
Conversation::~Conversation() {
    delete[] data_;
}

// Copy constructor that performs a deep copy of the "other" object (Conversation object).
Conversation::Conversation(const Conversation& other)
    : data_(nullptr),
      size_(other.size_),
      capacity_(other.capacity_) {
    if (capacity_ == 0) {
        return;
    }

    // Allocate new memory for the data array.
    data_ = new Message[capacity_];

    try {

        // Copy each Message from "other" to the new array.
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    
    // If an exception occurs during copying, the allocated memory is deleted to prevent memory leaks.
    } catch (...) {
        delete[] data_;
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
        throw;
    }
}

// Copy assignment operator that performs a deep copy of the "other" object (Conversation object).
// It checks for self-assignment, allocates new memory for the data array, and copies each Message from "other" 
// to the new array..
Conversation& Conversation::operator=(const Conversation& other) {
    if (this == &other) {
        return *this;
    }

    Message* new_data = nullptr;

    // Allocate new memory for the data array if the other object's array is not size 0.
    if (other.capacity_ > 0) {
        new_data = new Message[other.capacity_];

        try {

            // Copy each Message from the other object's data array to the new array. (Deep Copying)
            for (std::size_t i = 0; i < other.size_; ++i) {
                new_data[i] = other.data_[i];
            }

        // If an exception occurs during the copying process, the allocated memory is deleted to prevent memory leaks. 
        } catch (...) {
            delete[] new_data;
            throw;
        }
    }

    // Deletes the current object's data array to free up memory before assigning the new data array.
    delete[] data_;

    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.capacity_;

    return *this;
}

Conversation::Conversation(Conversation&& other) noexcept
    : data_(other.data_),
      size_(other.size_),
      capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}

// This is a move assignment operator that transfers ownership of data.
// It checks for self-assignment, deletes the current object's data, and 
// transfers the data pointer, size, and capacity from "other" to the current object. 
// Then "other" is reset to a valid but empty state.
Conversation& Conversation::operator=(Conversation&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    delete[] data_;

    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;

    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;

    return *this;
}

// Appends Message m to the conversation, growing the backing array if needed. If array size is 0 add 1, else double the capacity.
void Conversation::append(Message m) {

    // If the current size equals the capacity, we need to grow the array (can do this because array increases by 1 everytime).
    if (size_ == capacity_) {
        const std::size_t new_capacity =
            capacity_ == 0 ? 1 : capacity_ * 2;

        Message* new_data = new Message[new_capacity];

        // Move existing messages to the new array and add the new message. Here try and catch are used to handle exceptions and prevent memory leaks.
        try {
            for (std::size_t i = 0; i < size_; ++i) {
                new_data[i] = data_[i];
            }

            new_data[size_] = std::move(m);
        } catch (...) {
            delete[] new_data;
            throw;
        }

        // Delete the old array and update the data pointer and capacity.
        delete[] data_;

        data_ = new_data;
        capacity_ = new_capacity;
    } else {
        data_[size_] = std::move(m);
    }

    ++size_;
}


// Number of messages currently stored.
std::size_t Conversation::size() const noexcept {
    return size_;
}

// Bounds-checked access. Throws std::out_of_range if i >= size().
const Message& Conversation::at(std::size_t i) const {
    if (i >= size_) {
        throw std::out_of_range("Conversation index out of range");
    }

    return data_[i];
}

// Returns a pointer to the first message in the conversation and the end of the conversation, returning size also.
const Message* Conversation::begin() const noexcept {
    return data_;
}


const Message* Conversation::end() const noexcept {
    return data_ + size_;
}
