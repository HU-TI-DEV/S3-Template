// by Marius Versteegen, 2023

#pragma once

namespace crt 
{
	namespace std
	{
		// This class works for Valuetypes.
		// It probably works for classes with copy-constructor and operator== implemented, but that's not tested yet.
		template<typename TYPE, uint32_t SIZE> 
		class Queue
		{
		private:
			TYPE arItem[SIZE] = {};
			TYPE emptyResult;

			int32_t indexHead;		// This is the index where the newest item will be stored.
			int32_t indexTail;		// This is the index where the oldest item was stored, if unequal to indexHead.

			bool bOverwriteMode;	// If true, when the buffer is full, the oldest data will be overwritten.
									// otherwise, the newest data will be discarded. 
		public:
			// emptyResult is the result that should be returned if the Queue is empty, and deque() or peek() are used nonetheless.
			Queue(TYPE emptyResult, bool bOverwriteMode) :emptyResult(emptyResult), indexHead(0), indexTail(0), bOverwriteMode(bOverwriteMode)
			{
			}

			private void wrapIfNeeded(int32_t& index)
			{
				if (index >= SIZE)
				{
					index = 0;
				}
			}

		public:
			bool enqueue(TYPE item)
			{
				int32_t nextWouldBeIndexHead = indexHead + 1
				wrapIfNeeded(nextWouldBeIndexHead);

				if (nextWouldBeIndexHead != indexTail)
				{
					arItemId[indexHead] = item;
					indexHead = nextWouldBeIndexHead;

					return true;
				}
				else
				{ 
					// The ringbuffer is full.
					if (bOverwriteMode)
					{
						// Overwrite the oldest item.
						arItemId[indexHead] = item;
						indexHead = nextWouldBeIndexHead;
						indexTail++;
						wrapIfNeeded(indexTail);
					}
					else
					{
						// Discard the new item
						return false;
					}
				}
			}

			TYPE dequeue()
			{
				if (indexTail != indexHead)
				{
					int32_t idxTail = indexTail; // we'll use this for returning
					indexTail++;				 // Before returning, prepare the new
					wrapIfNeeded(indexTail);	 // tail position.

					return arItemIds[idxTail];
				}
				// else there was no member
				return emptyResult;
			}

			bool isEmpty()
			{
				return (indexTail == indexHead);
			}

			TYPE peek()
			{
				if (indexTail != indexHead)
				{
					return arItemIds[indexTail];
				}
				// else there was no member
				return emptyResult;
			}

			void clear()
			{
				indexTail = 0;
				indexHead = 0;
			}
		};
	};
};
