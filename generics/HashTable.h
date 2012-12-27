/*
 * HashTable.h
 *
 * Copyright 2012 Germaneers GmbH
 * Copyright 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
 *
 * This file is part of libopenstella.
 *
 * libopenstella is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * libopenstella is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libopenstella.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "LinkedList.h"

template <class Tkey, class Tvalue> class HashTable {
	private:
		class LLEntry {
			public:
				LLEntry(Tkey theKey, Tvalue theValue) { key = theKey; value=theValue; }
				Tkey key;
				Tvalue value;
				bool operator == (const LLEntry& other) const { return (other.key==key); }
		};
		LinkedList<LLEntry> _list;

	public:
		void set(Tkey key, Tvalue value) {
			LLEntry e(key,value);
			_list.remove(e);
			_list.add(e);
		}

		bool get(Tkey key, Tvalue *value) {
			for (typename LinkedList<LLEntry>::Item *item = _list.getFirstItem(); item!=0; item=item->getNext()) {
				LLEntry e = item->getData();
				if (e.key==key) {
					*value = e.value;
					return true;
				}
			}
			return false;
		}
};


#endif /* HASHTABLE_H_ */
