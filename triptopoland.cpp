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
Tree buildTree(Tokens);
StringList toPoland(Tokens);
Tokens insideParens(Tokens&);
void extend(StringList&, StringList&);
Token pop_front(Tokens&);


class Node
{
	public:
		string value;
		Tree left;
		Tree right;

		Node (Token tok) : value(tok.token) {};
		Node (Token tok, Tree& t_left, Tree& t_right) : value(tok.token), left(move(t_left)), right(move(t_right)) {};

		bool isLeaf() {return left.get() == nullptr && right.get() == nullptr; }

		virtual ~Node ();
};


int main() {
	string math("-ln(10 + 7) * 1  +2*3 - (-5-6)");
	/* math = "5*1^0+2*3^4 - 5"; */
	cout << math << endl;

	Tokens t = tokenize(math);

	for (auto tok : t) {
		cout << tok.token << " prec: " << tok.precedence << " const: " << tok.constant << endl;
	}

	StringList polo;
	try {
		polo = toPoland(t);

	} catch(Error e) {
		cout << e.msg << endl;
		return -1;
	}

	for (auto e : polo) {
		cout << e << " ";
	}
	cout << endl;

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

StringList toPoland(Tokens tokList) {
	StringList result;
	Tokens operatorHeap;

	while (not tokList.empty()) {
		Token tok = pop_front(tokList);
		if (tok.constant) {
			result.push_back(tok.token);
		} else if (tok.precedence == 0) {
			// parentheses
			Tokens inner = insideParens(tokList);
			inner.pop_back();  // Remove the last parenthesis
			StringList innerResult = toPoland(inner);
			extend(result, innerResult);
			if (tok.token == "(") {
				// ignore, we don't put it in the poloand notation
			} else {
				// The function name without the closing (
				result.push_back(tok.token.substr(0, tok.token.size() - 1));
			}
		} else {
			while (not operatorHeap.empty() && tok.precedence >= operatorHeap.back().precedence) {
				result.push_back(operatorHeap.back().token);
				operatorHeap.pop_back();
			}
			operatorHeap.push_back(tok);
		}
	}
	while (not operatorHeap.empty()) {
		result.push_back(operatorHeap.back().token);
		operatorHeap.pop_back();
	}
	return result;
}

Tokens insideParens(Tokens& tokList) {
	Tokens result;

	int depth(1);
	while (depth != 0 && !tokList.empty()) {
		if (tokList[0].token == ")") {
			--depth;
		} else if (tokList[0].token.back() == '(') {
			++depth;
		}
		result.push_back(tokList[0]);
		tokList.erase(tokList.begin());
	}

	if (tokList.empty() && depth != 0) {
		 throw Error{"Missing closing parenthesis"};
	}

	return result;
}
void extend(StringList& l1, StringList& l2) {
	for (auto s : l2) {
		l1.push_back(s);
	}
}
Token pop_front(Tokens& l) {
	Token t = l[0];
	l.erase(l.begin());
	return t;
}
