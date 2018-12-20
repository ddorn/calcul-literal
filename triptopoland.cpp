#include <iostream>
#include <vector>
#include <string>

using namespace std;

constexpr char MINUS_SIGN = '_';  // True minus sign, for unary minus

using Tokens = vector<string>;

void removeSpace(string& s);
void fixUnaryMinus(string& s);
bool isAlpha(string s);
Tokens tokenize(string s);
string popToken(string& s);


int main() {
	string math("-ln(10 + 7) * 1  +2*3 - (-5-6)");
	cout << math << endl;

	Tokens t = tokenize(math);

	for (auto tok : t) {
		cout << tok << endl;
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

string popToken(string& s) {
	// The purpose of this function is to group together tokens that ar more than one char long, like integers or functions names
	string token(1, s[0]);

	// if it is a function
	auto firstParenthesis = s.find('(');
	if (firstParenthesis != 0 && firstParenthesis != string::npos) {
		if (isAlpha(s.substr(0, firstParenthesis))) {
			token = s.substr(0, firstParenthesis + 1);
		}
	}

	// if it is anumber
	if (isdigit(s[0])) {
		token = "";
		int pos(0);
		while (isdigit(s[pos])) {
			token += s[pos];
			++pos;
		}
	}

	s.erase(0, token.size());
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
