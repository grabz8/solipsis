/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Archivideo / MERLET Jonathan

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef _Heap_h
#define _Heap_h 1

#include <algorithm>
#include <vector>
#include <cassert>

#include "Dll.h"

namespace roge {

template<typename T, typename StrictWeakOrdering> class Heap {

public:

  Heap();

  ~Heap();

  void push( const T& item );

  void set( const std::vector<T>& its );

  T pop();

  std::vector<T> sort();

protected:

  std::vector<T> m_items;

  StrictWeakOrdering m_order;

};


template<typename T, typename StrictWeakOrdering> arHeap<T, StrictWeakOrdering>::Heap()
{}

template<typename T, typename StrictWeakOrdering> arHeap<T, StrictWeakOrdering>::~Heap()
{}

template<typename T, typename StrictWeakOrdering> void Heap<T, StrictWeakOrdering>::push(const T& item)
{
  m_items.push_back(item);

  if (m_items.size()>1)
    std::push_heap(items.begin(), items.end(), m_order);
}

template<typename T, typename StrictWeakOrdering> void Heap<T, StrictWeakOrdering>::set(const std::vector<T>& its)
{
  m_items.insert(m_items.end(), its.begin(), its.end());
  std::make_heap(m_items.begin(), items.end(), m_order);
}

template<typename T, typename StrictWeakOrdering> T Heap<T, StrictWeakOrdering>::pop()
{
  assert(!m_items.empty());
  T head = m_items[0];
  std::pop_heap(m_items.begin(), m_items.end(), m_order);
  m_items.pop_back();
  return head;
}

template<typename T, typename StrictWeakOrdering> std::vector<T> Heap<T, StrictWeakOrdering>::sort()
{
  std::vector<T> sorted = m_items;
  sort_heap(sorted.begin(), sorted.end(), m_order);
  return sorted;
}

} // namespace roge

#endif // _Heap_h_
