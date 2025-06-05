//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_filesystem_directory_creator_included_
#define _ctoolhu_filesystem_directory_creator_included_

#include <boost/filesystem.hpp>
#include <string>

namespace Ctoolhu::FileSystem {

	class DirectoryCreator {

	  public:

		//creates the path for specified directory if doesn't exist
		explicit DirectoryCreator(const std::string &pathName)
		{
			const boost::filesystem::path outputPath{pathName};
			if (!boost::filesystem::exists(outputPath))
				boost::filesystem::create_directories(outputPath);
		}
	};

} //ns

#endif //file guard
