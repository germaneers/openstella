/*
 * FragmentedList.h
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

#ifndef FRAGMENTEDLIST_H_
#define FRAGMENTEDLIST_H_

#include "OS/RecursiveMutex.h"
#include <functional>

template <class TItem, int fragmentSize> class FragmentedList {
	private:
		struct fragment_t {
			TItem* items[fragmentSize];
			fragment_t *next;
		};

	private:
		RecursiveMutex _lock;

		fragment_t *createFragment() {
			fragment_t *result = new fragment_t;
			memset(result, 0, sizeof(fragment_t));
			return result;
		}


	public:
		static const int _fragmentSize = fragmentSize;
		fragment_t *_firstFragment;

		FragmentedList() : _firstFragment(0) {
		}

		void lock() {
			_lock.take();
		}

		void unlock() {
			_lock.give();
		}

		void addItem(TItem *itemToAdd) {
			RecursiveMutexGuard guard(&_lock);

			if (_firstFragment==0) { _firstFragment = createFragment(); }

			for (fragment_t *list = _firstFragment; list!=0; list = list->next)
			{
				for (int i=0; i<fragmentSize; i++) {
					TItem *item = list->items[i];
					if (item==0) { // empty slot
						list->items[i] = itemToAdd;
						return;
					}
				}

				if (list->next==0) {
					list->next = createFragment();
				}
			}
		}

		unsigned int removeItem(TItem *itemToRemove) {
			unsigned int removedItems = 0;

			RecursiveMutexGuard guard(&_lock);
			for (fragment_t *list = _firstFragment; list!=0; list = list->next)
			{
				for (int i=0; i<fragmentSize; i++) {
					if (list->items[i]==itemToRemove) {
						list->items[i] = 0;
						++removedItems;
					}
				}

			}
			return removedItems;
		}



		/* DO NOT USE! fancy, but pulls in exception-handling code. +80KB in .text! */

		/*
		void each(std::function<bool(TItem*)> f) {
			RecursiveMutexGuard guard(&_lock);
			for (fragment_t *list = _firstFragment; list!=0; list = list->next)
			{
				for (int i=0; i<fragmentSize; i++) {
					if (list->items[i]!=0) {
						bool shouldContinue = f(list->items[i]);
						if (!shouldContinue) return;
					}
				}

			}
		}
		*/
};

#endif /* FRAGMENTEDLIST_H_ */
