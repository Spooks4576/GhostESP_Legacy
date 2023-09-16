#ifndef CommandLine_h
#define CommandLine_h

#include <Arduino.h>
#include <vector>

#define MAX_COMMANDS 10
#define MAX_TOKENS 20

template <typename... Args>
struct ArgumentHandlerN;  // Primary template

template <>
struct ArgumentHandlerN<> {
    using FuncType = void (*)();
    FuncType func_;

    ArgumentHandlerN(FuncType func) : func_(func) {}

    void call(String tokens[], int tokenCount) { 
        if(tokenCount == 0 && func_) {
            func_();
        }
    }
};

template <typename Arg1>
struct ArgumentHandlerN<Arg1> {
    using FuncType = void (*)(Arg1);
    FuncType func_;

    ArgumentHandlerN(FuncType func) : func_(func) {}

    void call(String tokens[], int tokenCount) { 
        if(tokenCount == 1 && func_) {
            func_(tokens[0].c_str());
        }
    }
};

// Specialization for 2 arguments
template <typename Arg1, typename Arg2>
struct ArgumentHandlerN<Arg1, Arg2> {
    using FuncType = void (*)(Arg1, Arg2);
    FuncType func_;

    ArgumentHandlerN(FuncType func) : func_(func) {}

    void call(String tokens[], int tokenCount) { 
        if(tokenCount == 2 && func_) {
            func_(tokens[0].c_str(), tokens[1].c_str());
        }
    }
};

// Specialization for 3 arguments
template <typename Arg1, typename Arg2, typename Arg3>
struct ArgumentHandlerN<Arg1, Arg2, Arg3> {
    using FuncType = void (*)(Arg1, Arg2, Arg3);
    FuncType func_;

    ArgumentHandlerN(FuncType func) : func_(func) {}

    void call(String tokens[], int tokenCount) { 
        if(tokenCount == 3 && func_) {
            func_(tokens[0].c_str(), tokens[1].c_str(), tokens[2].c_str());
        }
    }
};

class CommandBase {
public:
    CommandBase(const String& command, const String& description)
        : command_(command), description_(description) {}
    virtual ~CommandBase() = default;
    virtual void call(String tokens[], int tokenCount) = 0;

    virtual const String& getCommandStr() const {
        return command_;
    }

    String command_;
    String description_;
};

template<typename... Args>
class Command : public CommandBase {
public:
    Command(const String& command, const String& description, void (*func)(Args...))
        : CommandBase(command, description), handler_(func) {}

    void call(String tokens[], int tokenCount) override {
        handler_.call(tokens, tokenCount);
    }

private:
    ArgumentHandlerN<Args...> handler_;
};


class CommandLine {
public:
  CommandLine(CommandBase* cmds[], size_t cmdCount)
    : commands(cmds), cmdCount(cmdCount) {}

  void loop() {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();

      String tokens[MAX_TOKENS];
      int tokenCount = tokenize(input, ' ', tokens, MAX_TOKENS);

      if (tokenCount == 0) return;

      for (size_t i = 0; i < cmdCount; i++) {
        if (tokens[0] == commands[i]->getCommandStr()) {
          // Remove the command from tokens
          for (int j = 0; j < tokenCount - 1; j++) {
            tokens[j] = tokens[j + 1];
          }
          tokenCount--;

          commands[i]->call(tokens, tokenCount);
          return;
        }
      }
      Serial.println("Unknown command. Type 'help' for a list of commands.");
    }
  }

private:
  CommandBase** commands;
  size_t cmdCount;

  int tokenize(const String& str, char delimiter, String output[], int maxTokens) {
    int startIndex = 0, endIndex = 0;
    int tokenCount = 0;

    while ((endIndex = str.indexOf(delimiter, startIndex)) >= 0 && tokenCount < maxTokens) {
      output[tokenCount++] = str.substring(startIndex, endIndex);
      startIndex = endIndex + 1;
    }
    if (startIndex < str.length() && tokenCount < maxTokens) {
      output[tokenCount++] = str.substring(startIndex);
    }
    return tokenCount;
  }
};

#endif