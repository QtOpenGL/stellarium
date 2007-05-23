/*
 * Stellarium
 * Copyright (C) 2006 Fabien Chereau
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
#ifndef STELMODULEMGR_H
#define STELMODULEMGR_H

#include <boost/iterator/iterator_facade.hpp>
#include "StelModule.hpp"

//! @brief Manage a collection of StelModule
//! @author Fabien Chereau <stellarium@free.fr>
class StelModuleMgr{
public:
	
	StelModuleMgr();
    ~StelModuleMgr();
	
	//! Register a new StelModule to the list
	void registerModule(StelModule* m);
	
	//! Load dynamically a module
	//! @param moduleID the name of the module = name of the dynamic library file without extension 
	//! (e.g "mymodule" for mymodule.so or mymodule.dll)
	//! @return the loaded module or NULL in case of error. The returned Stelmodule still needs to be initialized 
	StelModule* loadExternalPlugin(const std::string& moduleID);

	//! Get the corresponding module or NULL if can't find it.
	StelModule* getModule(const std::string& moduleID);
	
	//! Generate properly sorted calling lists for each action (e,g, draw, update)
	//! according to modules orders dependencies
	void generateCallingLists();

	//! Get the list of modules in the correct order for calling the given action
	const std::vector<StelModule*>& getCallOrders(const std::string& action)
	{
		return callOrders[action];
	}

	//! Add iterator so that we can easily iterate through registered modules
	class Iterator : public boost::iterator_facade<Iterator, StelModule*, boost::forward_traversal_tag>
	{
	 public:
	    Iterator() : mapIter() {}
	 private:
	    friend class boost::iterator_core_access;
	    friend class StelModuleMgr;
		Iterator(std::map<std::string, StelModule*>::iterator p) : mapIter(p) {}
	    void increment() { ++mapIter; }
	
	    bool equal(Iterator const& other) const
	    {
	        return this->mapIter == other.mapIter;
	    }
	
	    StelModule*& dereference() const { return mapIter->second; }
	
	    std::map<std::string, StelModule*>::iterator mapIter;
	};

   Iterator begin()
   {
      return Iterator(modules.begin());
   }

   const Iterator& end()
   {
      return endIter;
   }
private:
	
	//! The main module list associating name:pointer
	std::map<std::string, StelModule*> modules;
	
	//! The list of all module in the correct order for each action
	std::map<std::string, std::vector<StelModule*> > callOrders;
	
	const Iterator endIter;
};

#endif
