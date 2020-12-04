
#include <dr4/dr4_io.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>

std::string dr4::preparePathForWriting(const char* pathStr) {
	std::filesystem::path path(pathStr);
	if (path.has_parent_path()) {
		if (!std::filesystem::exists(path.parent_path())) {
			std::error_code err;
			if (!std::filesystem::create_directories(path.parent_path(), err)) {
				std::ostringstream ostr;
				ostr << "preparePathForWriting:Could not create path '"
					<< path.parent_path().generic_string();
				ostr << "' with error code " << err;
				return ostr.str();
			}
		}
	}
	return "";
}

std::string dr4::writeBytesToPath(const std::vector<uint8_t>& bytes, const char* pathStr)
{
	using namespace std;
	auto pathres = preparePathForWriting(pathStr);
	if (!pathres.empty())
		return pathres;

	ofstream fout;
	fout.open(pathStr, ios::binary | ios::out);
	fout.write((char*)bytes.data(), bytes.size());
	fout.close();
	return "";
}

dr4::array_result_t dr4::readBytesFromPath(const char* pathStr)
{
	std::fstream file(pathStr, std::fstream::in | std::fstream::binary);

	if (!file.is_open())
		return std::make_pair(nullptr, std::string("Could not open file ") + pathStr);

	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	auto bytes = std::make_unique<std::vector<uint8_t>>(size);
	file.read((char*)bytes->data(), size);

	if (!file)
		return std::make_pair(nullptr, std::string("Could not read all bytes from ") + pathStr);

	file.close();

	return std::make_pair(std::move(bytes), std::string(""));
}

dr4::string_result_t dr4::readFileFromPathToString(const std::string& pathStr)
{
	std::fstream file(pathStr, std::fstream::in | std::fstream::binary);

	if (!file.is_open())
		return std::make_pair(std::string(""), std::string("Could not open file ") + pathStr);


	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> vec(size);
	file.read(vec.data(), size);

	if (!file)
		return std::make_pair(std::string(""), std::string("Could not read all bytes from ") + pathStr);

	std::string stringOut(vec.begin(), vec.end());
	file.close();

	return std::make_pair(stringOut, std::string(""));
}

std::string dr4::FileNameStem(const std::string& path){
	std::filesystem::path p(path);
	return p.stem().string();
}
