#pragma once

#include <dr4/dr4_geometryresult.h>
#include <string>
#include <dr4/dr4_tuples.h>
namespace dr4 {

	template<class METADATA> METADATA SuccessValue();
	template<> inline GeometryResult SuccessValue<GeometryResult>() { return GeometryResult::Success; };
	template<> inline std::string SuccessValue<std::string>() { return ""; };

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
		TypedResult(bool isValid,const METADATA& meta_):m_isValid(isValid), m_meta(meta_) {}

		operator bool() const { return m_isValid; }
		const RESVALUE& value() { return m_value; }
		const METADATA& meta() { return m_meta; }

		static TypedResult Error(METADATA meta) {return TypedResult(false, meta);}
		static TypedResult Success(const RESVALUE& result) { return TypedResult(result); }
	};

	typedef TypedResult<GeometryResult, float> GeometryResultFloat;
	typedef TypedResult<GeometryResult, Pairf> GeometryResultPairf;
	typedef TypedResult<GeometryResult, std::pair<Pairf, Pairf>> GeometryResultHandles2D;

}
