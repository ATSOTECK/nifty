#include <iostream>

#include "lexer.hpp"

int main(int argc, char **argv) {
    String path = argv[1];
    Lexer lexer(path);
    Token t = lexer.nextToken();

    while (t.type != TK_EOF) {
        std::cout << tokenToString(t) << std::endl;
        t = lexer.nextToken();
    }

    return 0;
}
