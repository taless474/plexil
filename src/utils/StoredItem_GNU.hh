/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef STORED_ITEM__GNU_HH
#define STORED_ITEM__GNU_HH

/**
 * @file StoredItem_GNU.hh
 * @brief Implements the StoredItem class for platforms with GNU STL extensions.
 * @author Robert Harris based on code by Conor McGann
 * @date 4 October 2007
 * @ingroup Utils
 */

#include "KeySource.hh"


// Which files are included is dependent on the version of the 
// GNU libstdc++ installation, not the compiler.
// The macro __GLIBCXX__ (or __GLIBCPP__ for older versions) contains that information.

#ifdef PLEXIL_ANDROID
// Oddball Android 2.2 (maybe earlier as well)
# define _GLIBCXX_PERMIT_BACKWARD_HASH 1
# include <ext/hash_map>
# include <backward/hash_fun.h>
#else
// Location of the required include files depends on the libstdc++ version.
# if defined (__GLIBCXX__)
// +++ ADD LATEST REVISIONS HERE +++
#  if __GLIBCXX__ >= 20080306 && __GLIBCXX__ != 20080519 && __GLIBCXX__ != 20080704
// GCC 4.3.0 (20080306) through at least 4.5.2
#   include <backward/hash_map>
#   include <backward/hash_fun.h>
#  else
// GCC 3.4.0 through 4.2.4 (20080519)
// Red Hat uses a bastard version of 4.1.2 (20080704)
#   include <ext/hash_map>
#   include <ext/hash_fun.h>
#  endif
# elif defined (__GLIBCPP__)
// GCC 3.00 through 3.3.6 (20050503)
#  include <ext/hash_map>
#  include <ext/stl_hash_fun.h>
# else
#  error "Unable to determine GNU libstdc++ version."
# endif // defined (__GLIBCXX__)
#endif

namespace PLEXIL
{

   /** 
    * @class StoredItemKeyHasFunction
    *
    * @brief Provides a specialized hash functions for key_t in StoredItem.
    */

   template <class key_t>
   class StoredItemKeyHashFunction
   {
      public:
         size_t operator()(const key_t key) const
         {
            return (size_t) (key / KeySource<key_t>::increment());
         }
   };

   /** 
    * @class StoredItemKeyHasFunction
    *
    * @brief Provides a specialized hash functions for item_t in StoredItem.
    */

   template <class item_t>
   class StoredItemItemHashFunction
   {
      public:
         size_t operator()(const item_t * item) const
         {
            return (size_t)item;
         }
   };

   /**
    * @class StoredItem
    * @brief Provides for a symbolic value to be handled in a domain.
    *
    * The reader should note that items are stored in a static data
    * structure so that they can be shared. Access to the store is
    * provided by a key value. This reduces operations on StoredItem to
    * operations on key_t valued keys which is considerablly more
    * efficient. This encoding is largely transparent to users.
    */

   template <class key_t, 
             class item_t, 
             class itemHashFunction_t = StoredItemItemHashFunction<item_t>,
             class itemEqualFunction_t = std::equal_to<const item_t*>,
             class keyHashFunction_t  = StoredItemKeyHashFunction <key_t >,
             class keyEqualFunction_t = std::equal_to<const key_t> >
   class StoredItem
   {
      public:
   
         // define types for item and key store
         
         typedef typename __gnu_cxx::hash_map<
            const key_t, 
            item_t*, 
            keyHashFunction_t,
            keyEqualFunction_t>                             itemStore_t;
         typedef typename itemStore_t::const_iterator      itemConstIterator_t;
         typedef typename itemStore_t::iterator            itemIterator_t;
         typedef typename std::pair<const key_t, item_t*>  itemPair_t;

         typedef typename __gnu_cxx::hash_map<
            const item_t*,
            key_t,
            itemHashFunction_t,
            itemEqualFunction_t>                            keyStore_t;
         typedef typename keyStore_t::const_iterator       keyConstIterator_t;
         typedef typename keyStore_t::iterator             keyIterator_t;
         typedef typename std::pair<const item_t*, key_t>  keyPair_t;

         /**
          * @brief Zero argument constructor.
          * @note Should only be used indirectly, e.g., via std::list.
          */

         StoredItem()
         {
            m_key = ensureKey(&empty(), false);
         }

         /**
          * @brief This constructor copies the passed item into a
          * permanent location while the StoredItem is created.  The
          * passed item should be discarded.
          *
          * @param item The value of the item from which the created
          * item will be copied.
          */

         StoredItem(item_t& item)
         {
            m_key = ensureKey(&item, true);
         }

         /**
          * @brief This constructor which optinally copies the passed
          * item into a permanent location while the StoredItem is
          * created.  The passed item should be discarded is copyItem is
          * true.
          *
          * @param item A pointer to value of the item, which will be
          * stored, or optionally copied before it's store.
          *
          * @param copyItem If true, indicates that the item is not
          * currently stored in a permanent location and should be
          * copied into such a location.
          */

         StoredItem(item_t* item, bool copyItem)
         {
            m_key = ensureKey(item, copyItem);
         }

         /**
          * @brief Constructor from encoded key.
          *
          * Each StoredItem gets encoded as a key such that any 2
          * instances of a StoredItem constructed from the same item
          * will have the same key and that the key preserves
          * lexicographic ordering.
          *
          * @param key the key value for a previously created StoredItem instance.
          * @see m_key, getItem()
          */
         StoredItem(key_t key) : m_key(key)
         {
            assertTrueMsg(isKey(m_key), "Invalid key " << key << " provided.");
         }

         /**
          * @brief Copy constructor.
          *
          * Only needs to copy the key since the item value can be
          * recovered from the shared repository.
          *
          * @param other The source StoredItem.
          */
         StoredItem(const StoredItem& other)
            : m_key(other.m_key)
         {
         }

         /**
          * @brief Return the canonical empty item.
          *
          * @return A static empty item.
          */

         static item_t& empty()
         {
            static item_t sl_empty;
            
            return sl_empty;
         }

         /**
          * @brief Cast operator to get the key for this object.
          *
          * @return The key to this item.
          */

         operator key_t () const
         {
            return(m_key);
         }

         /**
          * @brief Lexical ordering test - less than
          */
         bool operator <(const StoredItem& item) const
         {
            return toItem() < item.toItem();
         }

         /**
          * @brief Lexical ordering test - greater than
          */

         bool operator >(const StoredItem& item) const
         {
            return toItem() > item.toItem();
         }

         /**
          * @brief Return the represented item.
          */
         const item_t& toItem() const
         {
            return(getItem(m_key));
         }

         /**
          * @brief Obtain the encoded key value for the item.
          * @return The key for accessing the store of items.
          */
         inline key_t getKey() const
         {
            return(m_key);
         }

         /**
          * @brief Return the number of items stored.
          * 
          * @return the number of items in this store.
          */
         static unsigned int getSize()
         {
            checkError(itemStore().size() == keyStore().size(), 
                       "itemStore size " << itemStore().size() << 
                       " does not match key store size " << keyStore().size());
            return(itemStore().size());
         }

         /**
          * @brief Test if the given key_t valued key maps to an item.
          */
         static bool isKey(key_t key)
         {
            return(itemStore().find(key) != itemStore().end());
         }
         
         /**
          * @brief Test if the given key_t valued key maps to an item.
          */
         static bool isItem(item_t* item)
         {
            return(keyStore().find(item) != keyStore().end());
         }
         
         /**
          * @brief Returns the cached item.
          */

         item_t& getItem() const
         {
            return getItem(m_key);
         }

         /**
          * @brief Free memory for for this stored item.
          *
          * Once freed the key value is set to an unassignedKey value.
          * If STORED_ITEM_REUSE_KEYS is defined at compile time the key
          * will be stored and potentially reissued in the future.  If
          * STORED_ITEM_REUSE_KEYS is NOT defined the key is retired,
          * any attempt to reuse the key will result in an error being
          * thrown.
          */
         
         void unregister()
         {
            unregister(m_key);
         }

         /**
          * @brief Obtain the key for the given item conducting an
          * insertion into sytem of it does not exits.
          *
          * @param item a pointer to the item to be added the store.
          * This item should be perminently allocated as it's address is
          * used to compute a hash.
	  * @param copyItem Whether or not to create a copy of the item.
          *
          * @return The key value created.
          */

         static key_t ensureKey(item_t* item, bool copyItem)
         {
            // if item is already in the system, return it's key

            keyIterator_t ki = keyStore().find(item);
            if (ki != keyStore().end())
	      {
		return ki->second;
	      }
            
            // allocate a key for this item

            key_t key = KeySource<key_t>::next();

            // insert the item

            handleInsertion(key, copyItem ? new item_t(*item) : item);
            return(key);
         }

         /**
          * @brief Free memory for item associated with provided key.
          *
          * Once freed the key value is set to an unassignedKey value.
          * If STORED_ITEM_REUSE_KEYS is defined at compile time the key
          * will be stored and potentially reissued in the future.  If
          * STORED_ITEM_REUSE_KEYS is NOT defined the key is retired,
          * any attempt to reuse the key will result in an error being
          * thrown.
          *
          * @param key Key which will be unregistered.
          */

         static void unregister(key_t& key)
         {
            assertTrueMsg(isKey(key), "Invalid key " << key << " provided.");
            delete handleRemoval(key);
            KeySource<key_t>::unregister(key);
            key = KeySource<key_t>::unassigned();
         }

         /**
          * @brief The key value used as a proxy for the original item.
          * @note The only instance data.
          * @see handleInsertion.
          */

         key_t m_key;

         /**
          * @brief Constructs an entry in itemStore to allow lookup
          * of items from a key.
          *
          * @param key The key_t value encoding for the given item
          * @param item the item for which the key has been encoded.
          * @see s_itemStore, getItem()
          */
         static void handleInsertion(key_t key, item_t* item)
         {
            itemStore().insert(itemPair_t(key, item));
            keyStore().insert(keyPair_t(item, key));
         }

         /**
          * @brief Removes the entry in itemStore associated with the
	  * given key.
          *
          * @param key The key_t value encoding for the given item.
	  * @return The item associated with the key.
          * @see s_itemStore, getItem()
          */

         static item_t* handleRemoval(key_t key)
         {
            item_t* item = &getItem(key);
            itemStore().erase(key);
            keyStore().erase(item);
            return item;
         }

         /**
          * @brief Obtain the item from the key.
          * @param key The key_t valued encoding of the item
          * @return a reference to the original item held in the item store.
          * @see s_itemStore
          */
         static item_t& getItem(key_t key)
         {
            const itemIterator_t& it = itemStore().find(key);
            check_error(it != itemStore().end());
            return *it->second;
         }

         /**
          * @brief Map keys to keys for key retrieval - i.e. toKey().
          */
          static keyStore_t& keyStore()
          {
             static keyStore_t sl_keyStore;
             return sl_keyStore;
          }
         /**
          * @brief Map keys to items for item retrieval - i.e. toItem(). 
          */
         static itemStore_t& itemStore()
         {
            static itemStore_t sl_itemStore;
            return sl_itemStore;
         }
   };
}
#endif
