#include <dr4/dr4_json_parser.h>
#include <memory>
#include <nlohmann/json.hpp>

#include <dr4/dr4_compress.h>

namespace dr4 {

    using json = nlohmann::json;

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Pairf, x, y)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RGBAFloat32, r, g, b, a)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Line2D, fst, snd)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Line2DCollection, lines, material)

    NLOHMANN_JSON_SERIALIZE_ENUM(BlendType, {
        {BlendType::Default,"Default" }
    })

    // Normalized<float>
    void to_json(json& j, const Normalized<float>& n) {
        float val = n.value();
        j = json{ val };
    }
    void from_json(const json& j, Normalized<float>& n) {
        float f = j;
        n = Normalized(f);
    }
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Blend, type, opacity)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Material2D, linewidth, colorFill, colorLine)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ColorFill, colorFill)

    NLOHMANN_JSON_SERIALIZE_ENUM(Content2D, {
        {Content2D::Fill,"Fill" },
        {Content2D::Lines, "Lines"}
     })
    
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Graphics2DElement, content, idx)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Layer, graphics, blend)
    
    NLOHMANN_JSON_SERIALIZE_ENUM(ScalarPresentation::Unit, {
         {ScalarPresentation::Unit::Cm, "Cm"},
         {ScalarPresentation::Unit::Mm, "Mm"},
         {ScalarPresentation::Unit::Meter, "Meter"}
    })

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ScalarPresentation, scale, dimension)
        
    void to_json(json& j, const Scene2D& s) {
        j = json{
            {"lines", s.lines},
            {"colorFills", s.colorFills},
            {"layers", s.layers},
            {"materials", s.materials},
            {"scalarPresentation", s.scalarPresentation}
        };
    }

    #define DR4_TRY_SET_FROM_JSON(jsonparam_, fieldName_, targetElem_)\
        try {jsonparam_.at(#fieldName_).get_to(targetElem_.##fieldName_);}catch(...){}

    void from_json(const json& j, Scene2D& p) {
        p = Scene2D();
      
        // accept all plausible jsons leniently as scene
        DR4_TRY_SET_FROM_JSON(j, lines, p)
        DR4_TRY_SET_FROM_JSON(j, colorFills, p)
        DR4_TRY_SET_FROM_JSON(j, layers, p)
        DR4_TRY_SET_FROM_JSON(j, materials, p)
        DR4_TRY_SET_FROM_JSON(j, scalarPresentation, p)

    }

}

std::string dr4::JsonParseResultToString(dr4::JsonParseResult j) {
    switch (j) {
        case JsonParseResult::Success: return "Success";
        case JsonParseResult::ParseError: return "ParseError";
    }
    return "<Unknown>";
}

class dr4::JsonImpl {
public:
    nlohmann::json json;

    JsonParseResult tryParse(const std::string& str) {
        JsonParseResult res = JsonParseResult::Success;
        try {
            json = nlohmann::json::parse(str);
        }
        catch (...) {
            res = JsonParseResult::ParseError;
        }
        return res;
    }

    bool readFromCompressed(const std::string& comprs) {
		size_t len;
		std::vector<std::uint8_t> v_cbor;
		if (!UncompressLen(comprs, len))
			return false;

		v_cbor.resize(len);
		if (!UncompressBytes(comprs, (char*)&v_cbor[0]))
			return false;

        json = json::from_cbor(v_cbor);
        return true;
	}

	// write to cbor and compress
	std::string compress() const {
		std::vector<std::uint8_t> v_cbor = json::to_cbor(json);
		std::string buf;
		const char* fst = (const char*)&v_cbor[0];
		Compress(fst, v_cbor.size(), buf);
		return buf;
	}

};

std::string dr4::Json::compress() const {
    return json->compress();
}

dr4::Json dr4::Json::Deflate(const std::string& comprs) {
    std::shared_ptr<JsonImpl> jsonimpl = std::make_shared<JsonImpl>();
    if (jsonimpl->readFromCompressed(comprs))
        return { jsonimpl };

    return Json();
}

dr4::JsonResult dr4::CreateJsonFromString(const std::string& str)
{
    Json j;
    j.json = std::make_shared<JsonImpl>();
    JsonParseResult res = j.json->tryParse(str);

    if (res != JsonParseResult::Success)
        return res;

    return j;
}

dr4::Json dr4::CreateJson()
{
    Json j;
    j.json = std::make_shared<JsonImpl>();
	return Json();
}

dr4::Json dr4::JsonParser::toJson(const Scene2D& scene) const
{
    Json res = CreateJson();
	to_json(res.json->json, scene);
    return res;
}

bool dr4::JsonParser::fromJson(const Json& json, dr4::Scene2D& out)
{
    from_json(json.json->json, out);
    return true;
}