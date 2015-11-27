#include "stdafx.h"
#include "randomStringGenerator.h"
#include <iterator>

using namespace std;
void RandomStringGenerator::generate(wstring &str) {
	static const wchar_t alphabet[] =
		L"abcdefghijklmnopqrstuvwxyz"
		L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		L"0123456789";

	random_device rd;
	default_random_engine rng(rd());
	uniform_int_distribution<> dist(0,sizeof(alphabet)/sizeof(*alphabet)-2);

	str.clear();
	str.reserve(NAME_LENGTH);
	generate_n(back_inserter(str), NAME_LENGTH,
		[&] { return alphabet[dist(rng)]; });
}