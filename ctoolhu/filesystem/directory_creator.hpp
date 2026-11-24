//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_filesystem_directory_creator_included_
#define _ctoolhu_filesystem_directory_creator_included_

#include <boost/filesystem.hpp>

namespace Ctoolhu::FileSystem {

	//creates the directory, including any missing parents, if it doesn't exist yet
	//returns the path, so that it can be used to initialise a member
	//throws boost::filesystem::filesystem_error if it can't be created (e.g. a file of the same name exists)
	inline boost::filesystem::path EnsureDirectoryExists(const boost::filesystem::path &path)
	{
		boost::filesystem::create_directories(path); //does nothing if the directory exists
		return path;
	}

} //ns

#endif //file guard
