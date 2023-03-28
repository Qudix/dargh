#pragma once

template <class T>
class TSingleton
{
public:
	TSingleton() = default;
	TSingleton(TSingleton&&) = delete;
	TSingleton(const TSingleton&) = delete;
	TSingleton& operator=(TSingleton&&) = delete;
	TSingleton& operator=(const TSingleton&) = delete;
	~TSingleton() = default;

public:
	static T* GetSingleton()
	{
		static T singleton;
		return std::addressof(singleton);
	}
};
