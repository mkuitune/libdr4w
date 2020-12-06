#include <iostream>
#include <filesystem>
#include <optional>
#include <string>
#include <list>
#include <vector>

//------------------------------------------------------
// Test utilites. Figure out what to do with these
//------------------------------------------------------

namespace dr4 {

	typedef std::filesystem::path Path;

	// Waiting for std::filesystem and utf-8 conventions in C++ to stabilize - use this wrapper in the meanwhile
	class PathString {
	public:
		Path m_path;
		PathString(const std::string& str):m_path(str){
			m_path.make_preferred();
		}
		Path toPath() const{ 
			return m_path;
		}
	};

	class Resources {
	public:		
		static std::optional<Path> FindResourceDirectory(int depth = 3) {
			namespace fs = std::filesystem;
			using namespace std;
			string seekName =  "Resources" ;
			// find path called "Resources" in this or top directories
			auto currentPath = fs::current_path();
			std::vector<fs::path> pathsToScan;
			for (int i = 0; i < depth; i++) {
				pathsToScan.push_back(currentPath);
				auto par = currentPath.parent_path();
				if (par != currentPath)
					currentPath = par;
				else
					break;
			}
			for (const auto& dirToScan : pathsToScan){
				for (const auto& entry : fs::directory_iterator(dirToScan)) {
					fs::path entrypath = entry.path();
					const auto filenamestr = entrypath.filename().string();
					if (entry.is_directory() ){
						if(filenamestr == seekName)
							return entry;
					}
				}
			}
			return nullopt;
		}

		static std::optional<Path> GetResourcePath(const PathString& ps) {
			namespace fs = std::filesystem;
			auto rootRes = FindResourceDirectory();
			if (!rootRes)
				return std::nullopt;
			auto root = rootRes.value();

			auto outpath =  root / ps.toPath();
			if (!fs::exists(outpath))
				return std::nullopt;

			return outpath;
		}



	};
}
