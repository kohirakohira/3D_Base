#pragma once
#include <unordered_map>
#include <any>
class BlackBoard {
public:
	// 要素をセット
	template<typename T>
	void set_value(const std::string& key, const T& value) {
		mData[key] = value;
	}

	// 要素を取得
	template<typename T>
	T get_value(const std::string& key) const {
		auto it = mData.find(key);
		if (it != mData.end()) {
			return std::any_cast<T>(it->second);
		}

		throw std::runtime_error("キーが見つかりませんでした： " + key);
	}

	// キーがあるかチェック
	bool has_key(const std::string& key) const {
	//	return mData.find(key) != mData.end();
	}

private:
//	std::unordered_map<std::string, > mData;
};
