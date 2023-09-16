#ifndef CommandLine_h
#define CommandLine_h

#include <Arduino.h>
#include <vector>
#include <utility>

class CommandBase {
public:
    virtual ~CommandBase() = default;
    virtual void call(const std::vector<String>& tokens) = 0;
    virtual const String& getCommandStr() const = 0;
};

template <typename... Args>
struct Command : public CommandBase {
    std::function<void(Args...)> func;

    Command(const String& cmdStr, const String& helpStr, std::function<void(Args...)> func)
        : commandStr(cmdStr), helpStr(helpStr), func(func) {}
    String commandStr;
    String helpStr;

    const String& getCommandStr() const override {
        return commandStr;
    }

    void call(const std::vector<String>& tokens) {
        if (tokens.size() < sizeof...(Args)) {
            Serial.println("Insufficient arguments.");
            return;
        }
        callImpl(tokens);
    }

private:
    
    void callImpl(const std::vector<String>& tokens) {
        func(getArgs<Args>(tokens)...);
    }

    
    template <typename T>
    T getArgs(const std::vector<String>& tokens) {
        return static_cast<T>(tokens.front().c_str());
    }
};

class CommandLine {
public:
    CommandLine(std::vector<CommandBase*>& commands)
        : commands(commands) {}

    void loop() {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();

            std::vector<String> tokens = tokenize(input, ' ');

            if (tokens.empty()) return;

            for (const auto& cmd : commands) {
                if (tokens[0] == cmd->getCommandStr()) {
                    tokens.erase(tokens.begin());  // Remove the command from tokens
                    cmd->call(tokens);
                    return;
                }
            }
            Serial.println("Unknown command. Type 'help' for a list of commands.");
        }
    }

private:
    std::vector<CommandBase*> commands;

    std::vector<String> tokenize(const String& str, char delimiter) {
        std::vector<String> tokens;
        int startIndex = 0, endIndex = 0;
        while ((endIndex = str.indexOf(delimiter, startIndex)) >= 0) {
            tokens.push_back(str.substring(startIndex, endIndex));
            startIndex = endIndex + 1;
        }
        if (startIndex < str.length()) {
            tokens.push_back(str.substring(startIndex));
        }
        return tokens;
    }
};


#endif