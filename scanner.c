#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "common.h"
#include "scanner.h"

typedef struct {
    const char *start;
    const char *current;
    int line;
} Scanner;

Scanner scanner;

void init_scanner(const char *source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static Token make_token(TokenType type) {
    return (Token){
        .type = type,
        .start = scanner.start,
        .length = (int)(scanner.current - scanner.start),
        .line = scanner.line
    };
}

static Token error_token(const char *message) {
    return (Token){
        .type = TOKEN_ERROR,
        .start = message,
        .length = strlen(message),
        .line = scanner.line
    };
}

static bool match(char expected) {
    if (*scanner.current == '\0' || *scanner.current != expected) {
        return false;
    }
    scanner.current++;
    return true;
}

static void skip_whitespace_and_comments() {
    for (;;) {
        switch (*scanner.current) {
            case ' ':
            case '\r':
            case '\t':
                scanner.current++;
                break;
            case '\n':
                scanner.line++;
                scanner.current++;
                break;
            case '/':
                if (scanner.current[1] == '/') {
                    scanner.current++;
                    while ((*scanner.current != '\n') && (*scanner.current != '\0')) {
                        scanner.current++;
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

static TokenType check_keyword(
    int start, int length, const char *rest, TokenType type
) {
    if (scanner.current - scanner.start == start + length) {
        if (memcmp(scanner.start + start, rest, length) == 0) {
            return type;
        }
    }
    return TOKEN_IDENTIFIER;
}

static TokenType identifier_type() {
    switch (scanner.start[0]) {
        case 'a':
            return check_keyword(1, 2, "nd", TOKEN_AND);
        case 'c':
            return check_keyword(1, 4, "lass", TOKEN_CLASS);
        case 'e':
            return check_keyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'a': 
                        return check_keyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o': 
                        return check_keyword(2, 1, "r", TOKEN_FOR);
                    case 'u': 
                        return check_keyword(2, 1, "n", TOKEN_FUN);
                }
            }
            break;
        case 'i':
            return check_keyword(1, 1, "f", TOKEN_IF);
        case 'n':
            return check_keyword(1, 2, "il", TOKEN_NIL);
        case 'o':
            return check_keyword(1, 1, "r", TOKEN_OR);
        case 'p':
            return check_keyword(1, 4, "rint", TOKEN_PRINT);
        case 'r':
            return check_keyword(1, 5, "eturn", TOKEN_RETURN);
        case 's':
            return check_keyword(1, 4, "uper", TOKEN_SUPER);
        case 't':
            if (scanner.current - scanner.start > 1) {
                switch (scanner.start[1]) {
                    case 'h': 
                        return check_keyword(2, 2, "is", TOKEN_THIS);
                    case 'r': 
                        return check_keyword(2, 2, "ue", TOKEN_TRUE);
                }
            }
            break;
        case 'v':
            return check_keyword(1, 2, "ar", TOKEN_VAR);
        case 'w':
            return check_keyword(1, 4, "hile", TOKEN_WHILE);
    }
    return TOKEN_IDENTIFIER;
}

static Token identifier() {
    while (isalnum(*scanner.current) || *scanner.current == '_') {
        scanner.current++;
    }
    return make_token(identifier_type());
}

static Token number() {
    while (isdigit(*scanner.current)) scanner.current++;

    if (*scanner.current == '.' && isdigit(scanner.current[1])) {
        scanner.current++;
    }

    while (isdigit(*scanner.current)) scanner.current++;
    return make_token(TOKEN_NUMBER);
}

static Token string() {
    while (*scanner.current != '"' && *scanner.current != '\0') {
        if (*scanner.current == '\n') {
            scanner.line++;
        }
        scanner.current++;
    }

    if (*scanner.current == '\0') {
        return error_token("Unterminated string");
    }
    scanner.current++;
    return make_token(TOKEN_STRING);
}

Token scan_token() {
    skip_whitespace_and_comments();
    scanner.start = scanner.current;

    if (*scanner.current == '\0') {
        return make_token(TOKEN_EOF);
    }

    if (*scanner.current == '_' || isalpha(*scanner.current)) {
        return identifier();
    }

    if (isdigit(*scanner.current)) {
       return number();
    }

    switch (*scanner.current++) {
        case '(':
            return make_token(TOKEN_LEFT_PAREN);
        case ')':
            return make_token(TOKEN_RIGHT_PAREN);
        case '{':
            return make_token(TOKEN_LEFT_BRACE);
        case '}':
            return make_token(TOKEN_RIGHT_BRACE);
        case ';':
            return make_token(TOKEN_SEMICOLON);
        case ',':
            return make_token(TOKEN_COMMA);
        case '.':
            return make_token(TOKEN_DOT);
        case '-':
            return make_token(TOKEN_MINUS);
        case '+':
            return make_token(TOKEN_PLUS);
        case '/':
            return make_token(TOKEN_SLASH);
        case '*':
            return make_token(TOKEN_STAR);
        case '!':
            return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"':
            return string();
    }

    return error_token("Unexpected character");
}
