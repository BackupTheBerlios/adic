/*
 * Copyright (C) 2002 Jens Thiele <jens.thiele@student.uni-tuebingen.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*!
   \file uriloader.h
   \brief load objects by URI
   \author Jens Thiele
*/

#ifndef URILOADER_H
#define URILOADER_H

// layer 2 xml sax input
#include "typedefs.h"
#include <dope/xmlsaxinstream.h>
#include <string>
#include <fstream>
#include <dope/dopeexcept.h>

class CacheConcept
{
public:
  template <class X>
  void get(const std::string &uri, DOPE_SMARTPTR<X> &dest)
  {}
  template <class X>
  void add(const std::string &uri, DOPE_SMARTPTR<X> &dest)
  {}
  int foo;
};
  
template <class Cache=CacheConcept>
class URILoader {
public:
  URILoader() {}
  ~URILoader(){}

  template <class X>
  void get(const std::string &uri, DOPE_SMARTPTR<X> &dest)
  {
    if (cache) {
      cache->get(uri,dest);
      if (dest.get())
	return;
    }
    std::string scheme(uri,0,5); // todo i think it is a bug if the std::string isn't > 5
    if (scheme=="file:") {
      std::ifstream file(std::string(uri,5).c_str()); // todo i think it is a bug if the std::string isn't > 5
      X* x=new X();
      // todo in the moment we assume the format is always XML
      typedef XMLSAXInStream<std::streambuf> InStream;
      InStream is(*file.rdbuf());
      is.simple(*x,NULL);
      dest=DOPE_SMARTPTR<X>(x);
      if (cache)
	cache->add(uri,dest);
      return;
    }
    throw ResourceNotFound(uri,std::string("Unsupported scheme: \"")+scheme+"\"");
  }
  static Cache* cache;
protected:
};
template <class Cache> Cache* URILoader<Cache>::cache=NULL;

template <class X, class REF = std::string >
class URICache
{
protected:
  typedef std::map<std::string,DOPE_SMARTPTR<X> > Cache;
  Cache cache;

public:
  void get(const REF &ref, DOPE_SMARTPTR<X> &xp)
  {
    typename Cache::iterator it(cache.find(ref));
    if (it!=cache.end())
      {
	xp=it->second;
      }
  }
  void add(const REF &ref, DOPE_SMARTPTR<X> &xp)
  {
    if (cache.find(ref)!=cache.end())
      throw std::string("There is already an entry for \"")+anyToString(ref)+"\"";
    cache[ref]=xp;
  }
};

#endif
