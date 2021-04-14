#pragma once

#include <dr4/dr4_geometryresult.h>
#include <string>
#include <dr4/dr4_tuples.h>
namespace dr4 {

	template<class METADATA> METADATA SuccessValue();
	template<> inline GeometryResult SuccessValue<GeometryResult>() { return GeometryResult::Success; };
	template<> inline std::string SuccessValue<std::string>() { return ""; };
	
	template<class METADATA> std::string MetaToString(const METADATA& m);
	template<> inline std::string MetaToString<GeometryResult>(const GeometryResult& m) {
		return GeometryErrorToString(m);
	}
	template<> inline std::string MetaToString<std::string>(const std::string& m) { return m; }



	template<class METADATA, class RESVALUE>
	class TypedResult{
	public:
		typedef typename RESVALUE result_type_t;
		bool m_isValid;
		METADATA m_meta;
		RESVALUE m_value;

		/* Success */
		TypedResult(const RESVALUE& value_):m_isValid(true), m_meta(SuccessValue<METADATA>()), m_value(value_) {}
		/* Error */
		TypedResult(const METADATA& meta_):m_isValid(false), m_meta(meta_) {}

		operator bool() const { return m_isValid; }
		const RESVALUE& value() const { return m_value; }
		const METADATA& meta() const { return m_meta; }
		std::string metaToString() const {
			return MetaToString(m_meta);
		}

		static TypedResult Error(METADATA meta) {return TypedResult(meta);}
		static TypedResult Success(const RESVALUE& result) { return TypedResult(result); }
	};

	typedef TypedResult<GeometryResult, float> GeometryResultFloat;
	typedef TypedResult<GeometryResult, Pairf> GeometryResultPairf;
	typedef TypedResult<GeometryResult, std::pair<Pairf, Pairf>> GeometryResultHandles2D;

}
