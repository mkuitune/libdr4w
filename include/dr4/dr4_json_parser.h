#pragma once

#include <dr4/dr4_result_types.h>
#include <dr4/dr4_scene2d.h>
#include<memory>

namespace dr4 {

	enum class JsonParseResult {
		Success,
		ParseError
	};
	std::string JsonParseResultToString(JsonParseResult j);

	class JsonImpl;
	class Json {
	public:
		std::shared_ptr<JsonImpl> json;
		bool isValid() {
			if (json)
				return true;
			else
				return false;
		}
		std::string compress() const;
		static Json Deflate(const std::string& compressed);
	};

	typedef TypedResult<JsonParseResult, Json> JsonResult;
	JsonResult CreateJsonFromString(const std::string&);
	Json CreateJson();

	class JsonParser {
	public:
		Json toJson(const Scene2D& scene) const;
		bool fromJson(const Json& json, Scene2D& out);
	};

}
