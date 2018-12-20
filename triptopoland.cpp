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


struct Error {
	string msg;
};

using StringList = vector<string>;

int getPrecedence(string s);
string popstring(string& s);
StringList tokenize(string s);
StringList toPoland(StringList);
StringList insideParens(StringList&);

// useful utilities
bool isOperator(string);
// utilities
void removeSpace(string& s);
void fixUnaryMinus(string& s);
bool isAlpha(string s);
string pop_front(StringList&);
void extend(StringList&, StringList&);

int main() {
	string math("-ln(10 + 7) * 1  +2*3 - (-5-6)");
	/* math = "5*1^0+2*3^4 - 5"; */
	cout << "enter some math:";
	getline(cin, math);
	cout << math << endl;

	StringList t = tokenize(math);
	StringList polo;

	try {
		polo = toPoland(t);
	} catch(Error e) {
		cout << e.msg << endl;
		return 1;
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

string popstring(string& s) {
	// The purpose of this function is to group together tokens that ar more than one char long, like integers or functions names
	string token = string(1, s[0]);

	auto firstParenthesis = s.find('(');

	// if it is a number
	if (isdigit(s[0])) {
		token = "";

		int pos(0);
		while (isdigit(s[pos])) {
			token += s[pos];
			++pos;
		}
	} else if (firstParenthesis != 0
			&& firstParenthesis != string::npos
			&& isAlpha(s.substr(0, firstParenthesis))) {
		// if it is a function
		token = s.substr(0, firstParenthesis + 1);
	} else {
	}

	s.erase(0, token.size());
	return token;
}

StringList tokenize(string s) {
	StringList tokens;

	removeSpace(s);
	fixUnaryMinus(s);

	while (not s.empty()) {
		tokens.push_back(popstring(s));
	}

	return tokens;
}

bool isAlpha(string s) {
	for (auto c : s) {
		if (not isalpha(c)) return false;
	}
	return true;
}

int getPrecedence(string token) {
	int prec(CONSTANT_PRECEDENCE);

	if (token.back() == '(') {
		return 0;
	}
	for (auto precList : PRECEDENCE) {
		for (auto op : precList) {
			if (token == op) {
				return prec;
			}
		}
		++prec;
	}

	return prec;
}

StringList toPoland(StringList tokList) {
	StringList result;
	StringList operatorHeap;


	while (not tokList.empty()) {
		string tok = pop_front(tokList);
		int precedence = getPrecedence(tok);

		if (precedence == 0) {
			// parentheses
			// The idea is to first totally process what' s inside the parens and just then put the operator.
			StringList inner = insideParens(tokList);
			inner.pop_back();  // Remove the last parenthesis
			StringList innerResult = toPoland(inner);
			extend(result, innerResult);
			if (tok == "(") {
				// ignore, we don't put it in the poloand notation
			} else {
				// The function name without the closing (
				result.push_back(tok.substr(0, tok.size() - 1));
			}
		} else if (isOperator(tok)) {
			while (not operatorHeap.empty() && precedence >= getPrecedence(operatorHeap.back())) {
				result.push_back(operatorHeap.back());
				operatorHeap.pop_back();
			}
			operatorHeap.push_back(tok);
		} else {  // it is a constant / symbol
			result.push_back(tok);
		}
	}

	// Move every operator left to the result
	while (not operatorHeap.empty()) {
		result.push_back(operatorHeap.back());
		operatorHeap.pop_back();
	}

	return result;
}

StringList insideParens(StringList& tokList) {
	// This is supposed to be called with a token list starting just after a parenthesis
	// And returns all the token that are enclose in this pair of ().

	StringList result;

	int depth(1);
	while (depth != 0 && !tokList.empty()) {
		if (tokList[0] == ")") {
			--depth;
		} else if (tokList[0].back() == '(') {
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
string pop_front(StringList& l) {
	string t = l[0];
	l.erase(l.begin());
	return t;
}

bool isOperator(string token) {
	for (auto l : PRECEDENCE) {
		for (auto op : l) {
			if (op == token) return true;
		}
	}
	return false;
}
