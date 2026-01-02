#pragma once

#include <string>

class InvalidAlleles : public std::exception {
private:
	std::string message;
public:
	InvalidAlleles(const std::string& msg) : message(msg) {}

	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

class NonexistantAlleles : public std::exception {
private:
	std::string message;
public:
	NonexistantAlleles(const std::string& msg) : message(msg) {}

	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};