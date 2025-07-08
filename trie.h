#pragma once
//adaptive school level bigdata searching algorithm
enum TrieMode {
	STATIC= 1 << 13,
	DYNAMIC= 1 << 14,
	ITERATIVE=1<<15
};
struct targpack {
	bool st, dm, it;
	short bits;
	targpack(short args):st(0),dm(0),it(0) {
		short bits = 0;
		if (STATIC & args)st=1;
		if (DYNAMIC & args)dm = 1;
		if (ITERATIVE & args)it=1;
		if (st) bits |= STATIC;
		if (dm) bits |= DYNAMIC;
		if (it) bits |= ITERATIVE;
	}
};
template<typename T>
class Trie {
	targpack args;
	Trie(short args_):args(args_) {
		
	}
	Trie() {
		
	}

	bool has(T&buf) {

	}
};
