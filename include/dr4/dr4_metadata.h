#pragma once

#include <string>
#include <map>
#include <memory>

namespace dr4 {

	class Metadata {
	public:
		enum class DataType { CSV, JSON };

		class IData {
		public:
			virtual ~IData() {}
			virtual DataType getType() const = 0;
		};

		typedef std::unique_ptr<IData> idataptr_t;

		class CSVData : public IData {
		public:
			std::string data;
			virtual ~CSVData() {}
			virtual DataType getType() const override { return DataType::CSV; }

		};
		class JSONData : public IData {
		public:
			std::string data;
			virtual ~JSONData() {}
			virtual DataType getType() const override { return DataType::JSON; }
		};

		std::map<std::string, idataptr_t> m_fields;
	private:
		template<class CLASTO>
		static const CLASTO* Cast(const IData* i) {
			auto dp = dynamic_cast<const CLASTO*>(i);
			return dp ? dp : nullptr;
		}

		static bool Is(const IData* ptr, DataType t) { return ptr->getType() == t; }

		bool hasKey(const std::string& str) const { return m_fields.find(str) != m_fields.end(); }
		std::string convertToUnique(const std::string& str) {
			using namespace std;
			int idx = 1;
			std::string candidate = str;
			while (hasKey(candidate)) {
				candidate = str + to_string(idx);
				idx++;
			}
			return candidate;
		}
	public:
		void addCSV(const std::string& key, const std::string& value) {
			using namespace std;
			auto csv = make_unique<CSVData>();
			csv->data = value;
			m_fields[convertToUnique(key)] = std::move(csv);
		}

		const IData* tryGet(const std::string& key) const {
			auto iter = m_fields.find(key);
			if (iter != m_fields.end()) {
				return iter->second.get();
			}
			return 0;
		}

		std::string getCSV(const std::string& key) {
			auto elem = tryGet(key);
			if (elem) {
				auto csv = Cast<CSVData>(elem);
				return csv ? csv->data : "";
			}
			return "";
		}

		std::string getJSON(const std::string& key) {
			auto elem = tryGet(key);
			if (elem) {
				auto json = Cast<JSONData>(elem);
				return json ? json->data : "";
			}
			return "";
		}
	};
}
