#pragma once
#include<list>
#include"libs.hpp"
	

//template<typename T>
//hasval
//typ
class tnod {
public:
	//alligned typesafe
	char hasval : 1, typ : 1;
	char unused[7];
	//external struct
	tnod(char _hasval, char _type
		//, szt _unus7b
	) {
		hasval = _hasval;
		typ = _type;
		//*(szt*)unused = ((~(255 << 55)) & _unus7b);
	}
	tnod() {

	}
};
template<typename T>
class Trie {
	char lay[256];
	char* darray;
	std::list<char*> heaps;
	/*iterative
	struct tnod
	int items
	val
	keys first
	pointers * current keys
	*/
	str ImakeAbstruct(int items) {
		if ((!items) || (items > 256))return 0;
		int elementsMem = (sizeof(str) + 1) * items;
		str res = strc malloc(elementsMem + sizeof(tnod) + 4+sizeof(T));
		if (!res)return 0;
		tnod t(0, 0);
		*(tnod*)(res) = t;
		*((int*)res + sizeof(tnod)) = 0;
		return res;
	}
	void Iset(char key, str iterativeNode, str value) {
		if (!iterativeNode)return;

		*((szt*)iterativeNode + sizeof(tnod)) += 1;
	}
	void Ifree(char key, str iterativeNode) {
		if (!iterativeNode)return;

	}
	/*static no keys / uses lay/offset layout
	struct tnod
	int items
	val
	raw pointers * items count
	*/
	str SmakeAbstruct(int items) {
		if ((!items) || (items > 256))return 0;
		int elementsMem = sizeof(str) * items;
		str res = strc malloc(elementsMem + sizeof(tnod) + 4);
		if (!res)return 0;
		tnod t(0, 1);
		t.
		*(tnod*)(res) = t;
		*((int*)staticnode) = items;
		str core = strc(res + 4 + sizeof(tnod)+sizeof(T));
		for (size_t i = 0; i < items; i++)
		{
			core[i] = 0;
		}
		heaps.emplace_back(res);
		return res;
	}
	void SsetVal(str staticnode,T val) {
		if (!staticnode)return;
		*(tnod*)(staticnode+4+ sizeof(tnod)) = val;
	}
	void SsetPTR(int offset, str staticnode, str ptr) {
		if ((offset < 0) || (offset > 255) || (!staticnode))return;
		if (offset > (*((int*)(staticnode+sizeof(tnod)))-1))return;
		( (str*)(staticnode + 4 + sizeof(tnod)+sizeof(T)) )[offset] = ptr;
	}
	bool Shasval(str staticnode,T*buffer=0) {
		if (!staticnode)return;
		tnod t= *(tnod*)staticnode;
		if (t.hasval) {
			if (buffer) {
				*buffer = t;
			}
			return 1;
		}
		return 0;
	}
	void Sfree(str staticnode) {
		if (!staticnode)return;
		int items=*(int*)(staticnode+sizeof(tnod));
		if (!items) {
			free(staticnode);
			return;
		}
		str core=strc(staticnode+4+sizeof(tnod)+sizeof(T));
		//todo recursive 
		for (size_t i = 0; i < items; i++)
		{
			if (core[i]) {
				*(int*)(core + sizeof(tnod))
				if()
				free(core[i]);
			}
		}
	}

public:
	size_t size;
	int uniq_symbols;
	unsigned short maxLength;
	//делать через хранение указателей

	Trie(str allowed_symbols,short treelength) {
		maxLength = treelength;
		const int evil=256;
		for( char* _:heaps)
		{
			free(_);
		}
		if (!allowed_symbols) {
			//binary fill
			for (szt i = 0; i < evil; i++)
			{
				lay[i] = i;

			}
			return;
		}
		char uniquesymbols[evil];
		memset(uniquesymbols,0, evil);
		memset(lay, 0, evil);

		for (szt i = 0; i < evil; i++)
		{
			if (!allowed_symbols[i])break;
			uniquesymbols[allowed_symbols[i]]=1;
		}
		
		int temp=0;
		for (szt i = 0; i < evil; i++)
		{
			if (uniquesymbols[i]) {
				lay[i] = temp;
				temp++;
			}
		}
	}
	Trie(cstr allowed_symbols) {
		Trie(strc allowed_symbols);
	}
	//Empty initializer
	Trie() {
		
	}
	//linear search
	bool has(T*buf) {

	}
	T& operator[](cstr _cstr) {
		findBykey(_cstr);
		//return
	}
	T& operator[](str _str) {
		findBykey(_str);
		//return 
	}

	bool findBykey(str _str) {
		if (_str) {
			
		}
		return 0;
	}

	bool findBykey(cstr _cstr) {

		return findBykey(strc _cstr);
	}

};
