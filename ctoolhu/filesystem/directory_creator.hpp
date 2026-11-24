//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_filesystem_directory_creator_included_
#define _ctoolhu_filesystem_directory_creator_included_

#include <concepts>
#include <filesystem>

namespace Ctoolhu::FileSystem {

	//creates the directory, including any missing parents, if it doesn't exist yet
	//returns the path unchanged, so that it can be used to initialise a member (of any type constructible from it)
	//throws std::filesystem::filesystem_error if it can't be created (e.g. a file of the same name exists)
	template <class Path>
		requires std::constructible_from<std::filesystem::path, const Path &>
	Path EnsureDirectoryExists(Path path)
	{
		std::filesystem::create_directories(std::filesystem::path{path}); //does nothing if the directory exists
		return path;
	}

} //ns

#endif //file guard
