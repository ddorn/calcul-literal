#include <iostream>
#include <vector>
#include <string>
#include <memory>

using namespace std;

constexpr char MINUS_SIGN = '_';  // True minus sign, for unary minus
constexpr int CONSTANT_PRECEDENCE = 31415;

const vector<vector<string>> PRECEDENCE = {
	{"(", ")"},  // functions need also to be hardocded at 0
	{"^"},
	{"*", "/"},
	{string(1, MINUS_SIGN)},
	{"+", "-"}
};


class Node;

struct Token {
	string token;
	int precedence;
	bool constant = false;
};

struct Error {
	string msg;
};

using StringList = vector<string>;
using Tokens = vector<Token>;
using Tree = unique_ptr<Node>;

void removeSpace(string& s);
void fixUnaryMinus(string& s);
bool isAlpha(string s);
Token popToken(string& s);
Tokens tokenize(string s);
int getPrecedence(string s);


class Node
{
	public:
		string value;
		Tree left;
		Tree right;

		Node (Token tok) : value(tok.token) {};
		Node (Token tok, Tree& t_left, Tree& t_right) : value(tok.token), left(move(t_left)), right(move(t_right)) {};
		virtual ~Node ();
};


int main() {
	string math("-ln(10 + 7) * 1  +2*3 - (-5-6)");
	cout << math << endl;

	Tokens t = tokenize(math);

	for (auto tok : t) {
		cout << tok.token << " prec: " << tok.precedence << " const: " << tok.constant << endl;
	}

	return 0;
}

void removeSpace(string& s) {
	string result("");

	for (auto c : s) {
		if (c != ' ') result += c;
	}
	s = result;
}

void fixUnaryMinus(string& s) {
	if (s[0] == '-') s[0] = MINUS_SIGN;

	for (size_t i = 1; i < s.size(); ++i) {
		if (s[i] == '-' && s[i-1] == '(') {
			s[i] = MINUS_SIGN;
		}
	}
}

Token popToken(string& s) {
	// The purpose of this function is to group together tokens that ar more than one char long, like integers or functions names
	Token token;
	token.token = string(1, s[0]);

	auto firstParenthesis = s.find('(');

	// if it is a number
	if (isdigit(s[0])) {
		token.token = "";
		token.precedence = CONSTANT_PRECEDENCE;
		token.constant = true;

		int pos(0);
		while (isdigit(s[pos])) {
			token.token += s[pos];
			++pos;
		}
	} else if (firstParenthesis != 0
			&& firstParenthesis != string::npos
			&& isAlpha(s.substr(0, firstParenthesis))) {
		// if it is a function
		token.token = s.substr(0, firstParenthesis + 1);
		token.precedence = 0;
	} else {
		token.precedence = getPrecedence(token.token);
	}

	s.erase(0, token.token.size());
	return token;
}

Tokens tokenize(string s) {
	Tokens tokens;

	removeSpace(s);
	fixUnaryMinus(s);

	while (not s.empty()) {
		tokens.push_back(popToken(s));
	}

	return tokens;
}

bool isAlpha(string s) {
	for (auto c : s) {
		if (not isalpha(c)) return false;
	}
	return true;
}

int getPrecedence(string operatr) {
	int prec(0);
	for (auto precList : PRECEDENCE) {
		for (auto op : precList) {
			if (operatr == op) {
				return prec;
			}
		}
		++prec;
	}

	throw Error{"operator " + operatr + "not supported"};
}
