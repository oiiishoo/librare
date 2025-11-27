#pragma once
#include<list>
#include"libs.hpp"
#include <type_traits>
// сделать общие методы для работы добавлений, удалений
enum ETrie {
	VALUE_SET = 1,
	VALUE_UNSET = 0,
	TRIE_KEY_TYPE_PTR = 1748744,
	TRIE_KEY_TYPE_FIELD = 138470,
	STATIC = 1,
	ITERATIVE = 0,

};

//integrated value was bad idea
// hasval
// typ - 0 iterative 1 static
class abnode {
public:
	char hasval : 1, typ : 1;
	abnode(char _hasval, char _type
	) {
		hasval = !!_hasval;
		typ = !!_type;
	}
	abnode() {

	}
};
template<typename k,typename v>
class Trie {
	char lay[256];
	short systemkeys; // range
	str darray; // root node storage - not a key
	ETrie keytype;// key operating type
	bool binstring; // is string binary
	unsigned int treedepth; // max key longevity
	//global functions
	bool hasval(str anynode, v* buffer) {
		if ((!anynode))return 0;
		abnode ab = *(abnode*)anynode;
		if (ab.hasval) {
			if (buffer) {
				*buffer = *(v*)(anynode + sizeof(abnode) + 2);
			}
			return 1;
		}
		return 0;
	}

	str setkey(str Node, unsigned char key, strs ptr) {
		return setkey(0, Node, key, ptr);
	}

	//capabilities
	// if Node to change is null return 0
	// if Node is need to be reallocated then previousNode must be set or return 0
	// 
	// if ptr is null but already present in Node, then Node will reallocate, but if previousNode not set, return 0
	// if ptr is defined and not defined previously in this Node, Node must be reallocated, if previousNode not set, return 0
	// if Node has the present key then no change to previousNode and reallocation of Node is not needed
	str setkey(str previousNode, str Node, unsigned char key, str ptr) {
		if (!Node)return 0;
		if (key > systemkeys)return 0;
		abnode ab = *(abnode*)Node;
		int haveval = ab.hasval ? sizeof(v) : 0;
		short items = ab.typ ? systemkeys : *(short*)(Node + sizeof(abnode));
		str res;

		

		

		if (ptr) {
			//check if ptr is in and or needed to be set instead of realloc
			if (ab.typ) {
				str* SpStartseg = (str*)(Node + sizeof(abnode) + haveval);

				if (SpStartseg[key] == ptr) {
					SpStartseg[key] = ptr;

					res= Node;
				}

			}
			else {
				str IkStartseg = strc(Node + sizeof(abnode) + 2 + haveval);
				str* IpStartseg = (str*)(IkStartseg + items);
				for (size_t i = 0; i < items; i++)
				{
					if (IkStartseg[i] == key) {
						if (IpStartseg[i] == ptr) {


							IpStartseg[i] = ptr;

							res= Node;
						}
					}
				}

			}

			//ptr non exist in Node
			if (ab.typ) {
				//invalid case
				res= 0;
			}
			else {

				str IkStartseg = strc(Node + sizeof(abnode) + 2 + haveval);
				str* IpStartseg = (str*)(IkStartseg + items);

				short resitems = items + 1;
				if (resitems == systemkeys) {

					res = strc malloc(sizeof(abnode) + haveval + sizeof(str) * items);
					if (!res)return 0;
					str* ESpStartseg = (str*)(res + sizeof(abnode) + haveval);

					for (size_t i = 0; i < systemkeys; i++)
					{
						ESpStartseg[IkStartseg[i]] = IpStartseg[i];
					}
					ESpStartseg[key] = ptr;

					*(abnode*)(res) = abnode(ab.hasval, 1);
					if (ab.hasval) {
						*(v*)(res + sizeof(abnode)) = *(v*)(Node + sizeof(abnode));
					}
				}
				else {
					
					res = strc malloc(sizeof(abnode) + 2 + haveval + resitems + sizeof(str) * resitems);

					if (!res)return 0;
					str EIkStartseg = strc(res + sizeof(abnode) + 2 + haveval);
					str* EIpStartseg = (str*)(EIkStartseg + items);

					memcpy(EIkStartseg+1, IkStartseg, items);
					memcpy(EIpStartseg+sizeof(str), IpStartseg, sizeof(str) * items);
					EIkStartseg[0] = key;
					EIpStartseg[0] = ptr;

					*(abnode*)(res) = abnode(ab.hasval, 0);
					*(short*)(res + sizeof(abnode)) = resitems;
					if (ab.hasval) {
						*(v*)(res + sizeof(abnode)) = *(v*)(Node + sizeof(abnode));
					}
				}
			}
		}
		else {
			//if ptr null
			short resitems = items - 1;

			if ((resitems > 0) && (resitems < systemkeys)) {

				//build if val or ptr present
				//remove if no val and ptr present
				if (ab.hasval) {
					//build with val
					if (ab.typ) {
						//invalid case
					}
					else {
						res = strc malloc(sizeof(abnode) + 2 + sizeof(v));

						if (!res)return 0;

						*(abnode*)(res) = abnode(ab.hasval, 0);
						*(short*)(res + sizeof(abnode)) = resitems;
						if (ab.hasval) {
							*(v*)(res + sizeof(abnode)) = *(v*)(Node + sizeof(abnode));
						}
					}
				}
				else {
					//remove
					res = 0;
				}

			}
			else if (resitems == 0) {
				

			}
			else {
				//negative resitems
				// do nothing
				res = 0;
			}
			if (ab.typ) {
				str* SpStartseg = (str*)(Node + sizeof(abnode) + haveval);

				if (SpStartseg[key] == ptr) {
					SpStartseg[key] = ptr;

					return Node;
				}

			}
			else {
				str IkStartseg = strc(Node + sizeof(abnode) + 2 + haveval);
				str* IpStartseg = (str*)(IkStartseg + items);
				for (size_t i = 0; i < items; i++)
				{
					if (IkStartseg[i] == key) {
						if (IpStartseg[i] == ptr) {


							IpStartseg[i] = ptr;

							return Node;
						}
					}
				}

			}

			if (ab.typ) {

			}
			else {

			}
		}



		//todo
		if (previousNode) {
			abnode pab = *(abnode*)previousNode;
			int phaveval = pab.hasval ? sizeof(v) : 0;
			if (pab.typ) {
				str* pabSpStartseg = (str*)(previousNode + sizeof(abnode) + phaveval);

				for (size_t i = 0; i < items; i++)
				{
					if (pabSpStartseg[i] == Node) {
						pabSpStartseg[i] = res;
					}
				}
			}
			else {

				str pabIkStartseg = (str)(previousNode + sizeof(abnode) + 2 + phaveval);
				str* pabIpStartseg = (str*)(pabIkStartseg + items);
				for (size_t i = 0; i < items; i++)
				{
					if (pabIpStartseg[i] == Node) {
						pabIpStartseg[i] = res;
					}
				}
			}
		}
		if (Node == res) {
			return res;
		}
		free(Node);
		return res;

	}


	str setval(str Node, v* val) {
		return setval(0, Node, val);
	}
	//val pointer is for nullifying, replacing additional removal logic
	str setval(str previousNode, str Node, v* val) {
		if (!Node)return 0;
		abnode ab = *(abnode*)Node;
		int haveval = ab.hasval ? sizeof(v) : 0;
		str res;
		short items=ab.typ?systemkeys: *(short*)(Node + sizeof(abnode));

		if (val) {
			if (ab.hasval) {

				if (ab.typ) {
					*(v*)(Node + sizeof(abnode)) = *val;
				}
				else {
					*(v*)(Node + sizeof(abnode) + 2) = *val;
				}
				return Node;
			}
			else {
				if (ab.typ) {
					//if static
					str* SpStartseg = (str*)(Node + sizeof(abnode));

					res = strc malloc(sizeof(abnode) + sizeof(v) + sizeof(str) * items);
					if (!res)return 0;
					str* ESpStartseg = (str*)(res + sizeof(abnode) + sizeof(v));

					memcpy(ESpStartseg, SpStartseg, sizeof(str) * items);
					*(abnode*)(res) = abnode(1, 1);
					*(v*)(res + sizeof(abnode)) = *val;

				}
				else {
					//if iterative
					str IkStartseg = strc(Node + sizeof(abnode) + 2);
					str* IpStartseg = (str*)(IkStartseg + items);

					res = strc malloc(sizeof(abnode) + 2 + sizeof(v) + items + sizeof(str) * items);

					if (!res)return 0;
					str EIkStartseg = strc(res + sizeof(abnode) + 2 + sizeof(v));
					str* EIpStartseg = (str*)(EIkStartseg + items);
					memcpy(EIkStartseg, IkStartseg, items);
					memcpy(EIpStartseg, IpStartseg, sizeof(str) * items);

					*((abnode*)(res)) = abnode(1, 0);
					*(short*)(res + sizeof(abnode)) = items;
					*(v*)(res + sizeof(abnode) + 2) = *val;



				}
			}
		}
		// !val
		else {

			if (ab.typ) {
				abnode resab(0, 1);
				str* abSpStartseg = (str*)(Node + sizeof(abnode) + haveval);

				res = strc malloc(sizeof(abnode) + items * sizeof(str));

				if (!res)return 0;
				*(abnode*)(res) = resab;
				str* resabSpStartseg = (str*)(res + sizeof(abnode));
				memcpy(resabSpStartseg, abSpStartseg, items * sizeof(str));
			}
			else {
				abnode resab(0, 0);
				str abIkStartseg = (str)(Node + sizeof(abnode) + 2 + haveval);
				str* abIpStartseg = (str*)(abIkStartseg + items);

				res = strc malloc(sizeof(abnode) + 2 + items + items * sizeof(str));
				if (!res)return 0;
				*(abnode*)(res) = resab;
				*(short*)(res + sizeof(abnode)) = items;

				str resabIkStartseg = (str)(res + sizeof(abnode) + 2);
				str* resabIpStartseg = (str*)(res + items);
				memcpy(resabIkStartseg, abIkStartseg, items);
				memcpy(resabIpStartseg, abIpStartseg, items * sizeof(str));
			}
		}




		//previousNode
		if (previousNode) {
			abnode pab = *(abnode*)previousNode;
			int phaveval = pab.hasval ? sizeof(v) : 0;

			if (pab.typ) {
				str* pabSpStartseg = (str*)(previousNode + sizeof(abnode) + phaveval);

				for (size_t i = 0; i < items; i++)
				{
					if (pabSpStartseg[i] == Node) {
						pabSpStartseg[i] = res;
					}
				}
			}
			else {

				str pabIkStartseg = (str)(previousNode + sizeof(abnode) + 2 + phaveval);
				str* pabIpStartseg = (str*)(pabIkStartseg + items);
				for (size_t i = 0; i < items; i++)
				{
					if (pabIpStartseg[i] == Node) {
						pabIpStartseg[i] = res;
					}
				}
			}
		}


		free(Node);

		return res;
	}
	unsigned int keylen(k key, unsigned int binlength=0) {
		unsigned int aux;
		if (keytype == TRIE_KEY_TYPE_PTR) {
			aux = binstring ? (binlength ? binlength : strlen(key)) : strlen(key);
			if (aux > treedepth || aux < 1)return 0;

			return aux;
		}
		if (keytype == TRIE_KEY_TYPE_FIELD) {
			aux = binlength ? binlength : sizeof(k);


			return aux;
		}
		return 0;
	}
	// third
	bool findBykey(k key, v*val,int binlength=0) {
		if (keytype == TRIE_KEY_TYPE_PTR) {
			if (!key)return 0;
			unsigned int len = keylen(key, binlength);
			str nextnode=haskey(*(unsigned char*)(key));;
			//ostanovka tut
			for (size_t i = 1; i < len; i++)
			{	
				
				if (!nextnode)return 0;
				
				nextnode = haskey(nextnode, *(unsigned char*)(key + i)));

			}
			abnode ab = *(abnode*)(nextnode);
			if (ab.hasval) {
				*val = *(v*)(nextnode+sizeof(abnode)+2);
				return 1;
			}
			
			return 0;
		}
		if (keytype == TRIE_KEY_TYPE_FIELD) {
			unsigned int len=keylen(key, binlength);
			str kp = strc &key;
			str nextnode = haskey(*(unsigned char*)(kp));;
			//ostanovka tut
			for (size_t i = 1; i < len; i++)
			{

				if (!nextnode)return 0;

				nextnode = haskey(nextnode, *(unsigned char*)(key + i) );

			}
			abnode ab = *(abnode*)(nextnode);
			if (ab.hasval) {
				*val = *(v*)(nextnode + sizeof(abnode) + 2);
				return 1;
			}
			//todo
			return 1;
		}
		return 0;
	}
	// make path with inserted key and return the last node
	// this first
	str makePath(k key, int binlength=0) {
		if (keytype == TRIE_KEY_TYPE_PTR) {
			if (!key)return 0;
			int len = binstring ? (binlength ? binlength : strlen(key)) : strlen(key);
			if (len > treedepth || len < 1)return 0;
			
			
			//tut delat
			for (size_t i = 0; i < len; i++)
			{
				if (haskey(*(unsigned char*) (key+i) )) {

				}
			}


			return res;
		}
		if (keytype == TRIE_KEY_TYPE_FIELD) {
			int len = binlength ? binlength : (sizeof(k) ? sizeof(k) : 0);
			if (!len)return 0;


			return res;
		}
		return 0;
	}
	//has keyed node in a node
	//done
	str haskey(str node,unsigned char key) {
		if ((key+1) > systemkeys)return 0;
		if (!node) {
			if (!darray)return 0;
			abnode ab = *(abnode*)darray;

			//if root static
			if (ab.typ && ((str*)(darray + sizeof(abnode) + sizeof(v)))[key])return ((str*)(darray+sizeof(abnode)+sizeof(v)))[key];

			return 0;
		}
		abnode ab = *(abnode*)node;
		//if static
		int wehaveval = ab.hasval ? sizeof(v) : 0;
		if (ab.typ) {
			str* Spsg = (str*)(node + sizeof(abnode) + wehaveval);

			return Spsg[key];
		}
		else {
			short items=*(short*)(node+sizeof(abnode));
			str Iksg = (str)(node + sizeof(abnode) + 2+wehaveval);
			str* Ipsg = (str*)(Iksg+items);
			for (size_t i = 0; i < items; i++)
			{
				if (((unsigned char)(Iksg[i]))==key) {
					return Ipsg[i];//tut ostanovka
				}
			}
			return 0;
		}
	}
	// root node
	str haskey(unsigned char key) {
		return haskey(0,key);
	}
	// travel from root to key stringed last node
	str stepIn(k key, int binlength) {
		if (keytype == TRIE_KEY_TYPE_PTR) {
			if (!key)return 0;
			int len = binstring ?(binlength? binlength : strlen(key)) : strlen(key);
			if (len>treedepth || len<1)return 0;




			return res;
		}
		if (keytype == TRIE_KEY_TYPE_FIELD) {
			int len = binlength ? binlength : (sizeof(k);
			if (len > treedepth || len < 1)return 0;

			return res;
		}
		return 0;
	}
	//todo
	void clearNodeFromPreviousNode(str& previousNode,str node) {
		if (!previousNode || !node)return;
		abnode ab = *(abnode*)previousNode;
		int wehaveval = ab.hasval ? sizeof(v) : 0;
		if (ab.typ) {
			str* Spseg = (str*)(previousNode + sizeof(abnode) + wehaveval);
			for (size_t i = 0; i < systemkeys; i++)
			{
				if (Spseg[i]==node) {
					unsigned char key = i;
					free(node);
					//todo realloc
					str prevnode = ImakeAbstruct(systemkeys - 1);
					if (wehaveval) { 
						str temp=Isetval(); 
					}
				}
			}
		}
		else {
			short items = *(short*)(previousNode + sizeof(abnode));
			str Ikseg = (previousNode + sizeof(abnode) + 2 + wehaveval);
			str* Ipseg = (str*)(Ikseg + items);
			for (size_t i = 0; i < items; i++)
			{
				if (Ipseg[i]==node) {


					//todo realloc
				}
			}
		}
	}
	//continue here
	void freenode(str previousnode,str node) {
		if (!node)return;
		abnode ab = *(abnode*)node;
		int wehaveval = ab.hasval? sizeof(v) :0;
		if (ab.typ) {
			str* Spseg = (str*)(node+sizeof(abnode)+wehaveval);
			for (size_t i = 0; i < systemkeys; i++)
			{
				if (Spseg[i]) {
					freenode(0, Spseg[i]);
					if (previousnode)clearNodeFromPreviousNode(previousnode, Spseg[i]);
					free(Spseg[i]);
				}
			}
		}
		else {
			short items=*(short*)(node+sizeof(abnode));
			str Ikseg = (node+sizeof(abnode)+2 + wehaveval);
			str* Ipseg = (str*)(Ikseg+items);
			
			for (size_t i = 0; i < items; i++)
			{
				if (Ipseg[i]) {
					freenode(0,Ipseg[i]);
					if (previousnode)clearNodeFromPreviousNode(previousnode, Ipseg[i]);
					free(Ipseg[i]);
				}
			}
		}
	}
	/*iterative
	struct abnode
	short items
	val
	keys first
	pointers * current keys
	*/
	str ImakeAbstruct(int items) {
		if ((items<0) || (items > systemkeys))return 0;
		int elementsMem = sizeof(str)*items+items;
		str res = strc malloc(elementsMem + sizeof(abnode) + 2);
		if (!res)return 0;
		abnode ab(0, 0);
		*(abnode*)(res) = ab;
		*(short*)(res + sizeof(abnode)) = items;


		return res;
	}
	//returns
	str Isetval(str previousNode, str iterativeNode, v value) {
		if((!iterativeNode))return 0;
		abnode &ab = *(abnode*)(iterativeNode);
		if (ab.typ)return 0;
		if (ab.hasval) {
			ab = abnode(1, 0);
			*(v*)(iterativeNode + sizeof(abnode) + 2) = value;
			return iterativeNode;
		}
		short items = *(short*)(iterativeNode + sizeof(abnode));
		if ((items <= 0) || (items>256))return 0;
		str IkStartseg = strc(iterativeNode + sizeof(abnode) + 2);
		str* IpStartseg = (str*)(IkStartseg + items);
		str res = strc malloc(sizeof(abnode)+2 +sizeof(v)+ items + sizeof(str) * items);
		if (!res)return 0;
		str EIkStartseg = strc(res + sizeof(abnode) + 2+sizeof(v));
		str* EIpStartseg = (str*)(EIkStartseg + items);
		memcpy(EIkStartseg, IkStartseg,items);
		memcpy(EIpStartseg, IpStartseg,sizeof(str)*items);

		*(abnode*)(res) = abnode(1, 0);
		*(short*)(res + sizeof(abnode)) = items;
		*(v*)(res + sizeof(abnode) + 2) = value;
		//previousNode
		abnode pab = *(abnode*)();

		return res;
	}
	bool Ipopval(str previousNode,str iterativeNode, v& value) {
		if ((!iterativeNode))return 0;
		if ((*(abnode*)(iterativeNode)).typ)return 0;
		//todo realloc + unset value with return true, if inode is not iterative then, return false
	}
	
	void IsetPTR(str ptr,unsigned char key,str iterativeNode) {
		if (!iterativeNode)return;
		if ((*(abnode*)(iterativeNode)).typ)return;
		//todo realloc+set ptr
	}
	/*static, no obvious keys / uses lay/offset layout
	struct abnode
	val
	raw pointers * items count
	*/
	str SmakeAbstruct(str iterativeNode) {
		if (!iterativeNode)return 0;
		
		//copy from inode
		abnode ab = *(abnode*)(iterativeNode);

		if (ab.typ)return 0;

		short items = *(short*)(iterativeNode + sizeof(abnode));
		if (!(items==systemkeys)) return 0;
		v val;
		int wehaveVAL = ab.hasval ? sizeof(v) : 0;
		if(ab.hasval)val = *(v*)(iterativeNode + sizeof(abnode) + 2);
		str IkStartseg = strc(iterativeNode + sizeof(abnode) + 2 + wehaveVAL);
		str* IpStartseg = (str*)(IkStartseg +items);
		//alloc and set snode
		str res = strc malloc((sizeof(str) * items) + sizeof(abnode)+ wehaveVAL);
		if (!res)return 0;
		*(abnode*)(res) = abnode(ab.hasval,1);
		if (ab.hasval) {
			*(v*)(res + sizeof(abnode)) = val;
		}
		str* SpStartseg = (str*)(res + sizeof(abnode) + wehaveVAL);
		for (size_t i = 0; i < items; i++)
		{
			unsigned char idx = IkStartseg[i];
			SpStartseg[idx] = IpStartseg[idx];
		}
		return res;
	}
	//todo
	str Ssetval(str previousNode,str staticnode, v val) {
		if ((!staticnode))return;
		abnode ab = *(abnode*)(staticnode);
		if (!ab.typ)return 0;
		if ()return;
		*(abnode*)(staticnode) = abnode(1,1);
		*((v*)(staticnode + sizeof(abnode))) = val;
		//todo
	}
	//value ptr will be set if value present
	//returns new node
	//if previousNode present, changes ptr in previousNode of staticnode
	//make sure to free it yourself
	str Spopval(str previousNode,str staticnode, v* value) {
		if (!staticnode)return 0;
		abnode ab = *(abnode*)(staticnode);
		if (!(ab.hasval && ab.typ)) return 0;

		str* Spseg = (str*)(staticnode + sizeof(abnode)+sizeof(v));
		if (value) *value = *(v*)(staticnode + sizeof(abnode));
		str res = strc malloc(sizeof(abnode) + systemkeys * sizeof(str));
		if (!res)return 0;
		*(abnode*)(res) = abnode(0, 1);
		str* ESpseg = (str*)(res + sizeof(abnode));
		for (size_t i = 0; i < systemkeys; i++)
		{
			ESpseg[i] = Spseg[i];
		}
		if (!previousNode) return res;
		//ostanovka tut
		abnode pab = *(abnode*)(previousNode);
		int wehaveVAL = pab.hasval ? sizeof(v) : 0;
		if (pab.typ) {
			str* XSpseg = (str*)(previousNode + sizeof(abnode)+wehaveVAL);
			for (size_t i = 0; i < systemkeys; i++)
			{
				if (XSpseg[i]==staticnode) {
					XSpseg[i] = res;
				}
			}
		}
		else {
			short items = *(short*)(previousNode + sizeof(abnode));
			str Ikseg = strc(previousNode + sizeof(abnode) + 2 + wehaveVAL);
			str* Ipseg = (str*)(Ikseg + items);
			for (size_t i = 0; i < items; i++)
			{
				if (Ipseg[i]==staticnode) {
					Ipseg[i] = res;
				}
			}
		}


		return res;
	}
	void SsetPTR(str ptr,unsigned char key, str staticnode) {
		if ((key < 0) || (key > 255) || (!staticnode))return;
		if ((!(*(abnode*)(staticnode)).typ))return 0;
		if (key > ((*(short*)(staticnode+sizeof(abnode)))-1))return;
		
	}
	szt nodecount;
	szt valuecount;
	szt usedHeap;
public:
	szt getSize() {
		return size;
	}
	szt getNodecount() {
		return nodecount;
	}
	szt getValuecount() {
		return valuecount;
	}
	szt getUsedHeap() {
		return usedHeap;
	}
	v& rootVal=*(v*)valptr;
	Trie& put(k key, v val) {
		if (!darray) {
			darray = (str)malloc(sizeof(abnode)+ sizeof(v)+systemkeys*sizeof(str));
			*(abnode*)darray = abnode(1, 1);

		}

		return *this;
	}
	void clear() {
		//todo remake
		if (!darray)return*this;
		free(darray);
		lay = {};
		return *this;
	}
	~Trie() {
		clear();
	}
	Trie() {
		const int evil = 256;
		treedepth = evil;
		clear();
		if constexpr (std::is_pointer_v<k>) {
			printf("the key will be pointer array string for this map\n");
			keytype = ETrie::TRIE_KEY_TYPE_PTR;
		}
		else if constexpr (std::is_integral_v<k>) {
			printf("the key will be integer for this map\n");
			keytype = ETrie::TRIE_KEY_TYPE_FIELD;
		}
		else if constexpr (std::is_floating_point_v<k>) {
			printf("map errored! access key type is floating point\n");
			return;
		}
		else if constexpr (std::is_class_v<k>) {
			printf("the key will be structure for this map\n");
			keytype = ETrie::TRIE_KEY_TYPE_FIELD;
		}
		for (szt i = 0; i < evil; i++)
		{
			//binary fill
			lay[i] = i;

		}
		binstring = 1;
		systemkeys = evil;
	}
	void Konstructor_define_treelength_allowedsymbol_group(unsigned short treedepth, str allowed_symbols,unsigned int allowedsymbols_length=256) {
		const int evil = 256;
		this.treedepth = treedepth;
		clear();
		if constexpr (std::is_pointer_v<k>) {
			printf("the key will be pointer array string for this map\n");
			keytype = ETrie::TRIE_KEY_TYPE_PTR;
		}
		else if constexpr (std::is_integral_v<k>) {
			printf("the key will be integer for this map\n");
			keytype = ETrie::TRIE_KEY_TYPE_FIELD;
		}
		else if constexpr (std::is_floating_point_v<k>) {
			printf("map errored! access key type is floating point\n");
			return;
		}
		else if constexpr (std::is_class_v<k>) {
			printf("the key will be structure for this map\n");
			keytype = ETrie::TRIE_KEY_TYPE_FIELD;
		}
		if (!allowed_symbols) {
			//binary fill
			for (szt i = 0; i < evil; i++)
			{
				lay[i] = i;

			}
			binstring = 1;
			systemkeys = evil;
			return;
		}
		//бля всё нахуй тут перевернуть, задача научить работать с нулевым байтом, можно не надо если указан массив уникалов
		char uniquesymbols[evil]={};

		for (szt i = 0; i < evil; i++) {
			lay[i] = 255; // изначально все "запрещены"
		}
		if (allowedsymbols_length>0) {
			int asf = (allowedsymbols_length <= 256)?allowedsymbols_length :256 ;
			for (szt i = 0; i < allowedsymbols_length; i++)
			{
				uniquesymbols[allowed_symbols[i]] = 1;
			}
		}
		else {
			for (szt i = 0; i < evil; i++)
			{
				if (!allowed_symbols[i])break;
				uniquesymbols[allowed_symbols[i]] = 1;
			}
		}

		int temp = 0;
		for (szt i = 0; i < evil; i++)
		{
			if (uniquesymbols[i]) {
				lay[i] = temp;
				temp++;
			}
		}
		systemkeys = temp;
		if (uniquesymbols[0])binstring = 1;
	}
	Trie(cstr allowed_symbols) {
		Trie(strc allowed_symbols);
	}
	//Empty initializer
	//linear search
	bool has(v&buf) {

	}
	v operator[](k key) {
		findBykey(_cstr);
		//return

	}
	v operator[](k key) {
		findBykey(_str);
		//return 
	}

};
void practice() {
	//Trie<int,int> t;
	
}
