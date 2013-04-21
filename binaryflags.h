#pragma once
/*
inliners to do the oh-so-tedious binary ops
set sets the xth field to 1, clear sets it to 0, flip xors it,
assign sets or clears depending on bool being true or false respectively
templates because i can use/need ints, unsigneds, unsigned chars, chars, etc. for the flag
type and its no hassle to write it like that
*/
namespace edy{
	namespace core{
		template<typename T> T assignField(T in,int field,bool value)
		{
			in&=~(1<<field);//clear
			if(value) in|=1<<field;//set if its true
			return in;
		}
		template<typename T> T setField(T in,int field)
		{
			//or with 1 at right pos
			in|=1<<field;
			return in;
		}
		template<typename T> T clearField(T in,int field)
		{
			//and with full 1s and 0 at right pos
			in&=~(1<<field);
			return in;
		}
		template<typename T> T flipField(T in,int field)
		{
			//xoring with 0 is false, with 1 is toggle,so we move that 1 to right pos and xor
			in^=1<<field;
			return in;
		}
	}
}