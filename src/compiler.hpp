//
// Created by Skyler on 2/22/23.
//

#ifndef NIFTY_COMPILER_HPP
#define NIFTY_COMPILER_HPP


class Compiler {
public:
    void compile();

private:
    void lex();
    void parse();
    void codegen();
    void link();
};


#endif //NIFTY_COMPILER_HPP
