#include <iostream>
#include <vector>
#include <string>

using namespace std;
constexpr char MINUS_SIGN = '_';  // True minus sign, for unary minus

void removeSpace(string& s);
void fixUnaryMinus(string& s);

int main() {
	string math("-ln(7) * 1  +2*3 - (-5-6)");
	cout << math << endl;

	removeSpace(math);
	fixUnaryMinus(math);

	cout << math << endl;
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
