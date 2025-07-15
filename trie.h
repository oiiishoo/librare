#pragma once
#include<iostream>
#include<list>

namespace Utrie{
	void stnod_reset() {

	}
	void stnod_set(char elem_idx,char*ptr) {

	}
};

//template<typename T>
class tnod {
public:
	//alligned typesafe
	char hasval : 1, typ : 1;
	//opted if key
	char key;
	char unused[6];
	//external struct
	tnod(char _hasval,char _type,char _key,size_t _unus6b) {
		hasval = _hasval;
		typ = _type;
		key = _key;
		*(size_t*)unused = ((~(65535<<47))&_unus6b);
	}
};
template<typename T>
class Trie {
	char lay[256];


public:
	size_t size;
	short uniq_symbols;
	//делать через хранение указателей

	Trie(const char*allowed_symbols) {
		char uniquesymbols[256];
		memset(uniquesymbols,0,256);
		memset(lay, 0, 256);
		for (size_t i = 0; i < 256; i++)
		{
			if (allowed_symbols[i] == 0)break;
			uniquesymbols[allowed_symbols[i]]=1;
			
		}
		
		int temp=0;
		for (size_t i = 0; i < 256; i++)
		{
			if (uniquesymbols[i]) {
				lay[i] = temp;
				temp++;
			}
		}
	}
	Trie() {
		dict.reserve(256);
		d.cks;
	}
	//linear search
	bool has(T&buf) {

	}
	//поиск снизу
};
