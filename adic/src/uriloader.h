/*
 * Copyright (C) 2002 Jens Thiele <karme@berlios.de>
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
#include "utils.h"

//! concept of object cache
/*!
  objects are referenced by a uri string
*/
class CacheConcept
{
public:
  //! get object from cache
  /*!
    \param uri the uri string
    \param dest the pointer to the object

    \return set dest to the object or NULL if the object is not in the cache
  */
  template <class X>
  void get(const std::string &uri, DOPE_SMARTPTR<X> &dest)
  {}
  //! add object to cache
  /*!
    \param uri the uri string
    \param dest the pointer to the object
  */  
  template <class X>
  void add(const std::string &uri, DOPE_SMARTPTR<X> &dest)
  {}
};

//! get object referenced by uri (probably from a cache)
template <class Cache=CacheConcept>
class URILoader {
public:
  URILoader() {}
  ~URILoader(){}

  //! get object referenced by uri
  template <class X>
  void get(const std::string &uri, DOPE_SMARTPTR<X> &dest)
  {
    if (cache) {
      cache->get(uri,dest);
      if (dest.get())
	return;
    }
    if (uri.length()>5) {
      std::string scheme(uri,0,5); 
      if (scheme=="file:") {
	dest=loadFromFile<X>(std::string(uri,5).c_str());
	if (cache) cache->add(uri,dest);
	return;
      }
      if (scheme=="data:") {
	std::string dataFile(uri,5);
	std::string fname(findDataFile(dataFile));
	if (fname.empty())
	  throw ResourceNotFound(uri,std::string("Data file \"")+dataFile+"\" does not exist in path");
	dest=loadFromFile<X>(fname.c_str());
	if (cache) cache->add(uri,dest);
	return;
      }
      throw ResourceNotFound(uri,std::string("Unsupported scheme: \"")+scheme+"\"");
    }
    throw ResourceNotFound(uri,std::string("Unsupported URI: \"")+uri+"\"");
  }
  //! pointer to the cache object or NULL
  static Cache* cache;
protected:
  template <typename X>
  DOPE_SMARTPTR<X> loadFromFile(const char *fname)
  {
    std::ifstream file(fname);
    if (!file.good())
      throw ResourceNotFound(fname,std::string("Could not open file"));
    DOPE_SMARTPTR<X> x(new X());
    // todo in the moment we assume the format is always XML
    typedef XMLSAXInStream<std::streambuf> InStream;
    InStream is(*file.rdbuf());
    is.simple(*x.get(),NULL);
    return x;
  }
};
template <class Cache> Cache* URILoader<Cache>::cache=NULL;

//! URICache implementing the CacheConcept
template <class X, class REF = std::string >
class URICache
{
protected:
  typedef std::map<std::string,DOPE_SMARTPTR<X> > Cache;
  //! the cache
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
