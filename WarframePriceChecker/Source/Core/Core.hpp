#pragma once

#include <string>
#include <vector>

namespace WarframeSnail
{
	struct ItemEntry
	{
		int Price = 0;
		std::wstring ItemName = std::wstring();

		static ItemEntry Empty() { return ItemEntry(); }

		bool operator==(ItemEntry& right)
		{
			if (this->ItemName == right.ItemName &&
				this->Price == right.Price)
				return true;
			else
				return false;
		}
	};

	const wchar_t* GetCurrentPath();

	void ExeOCROnPNG(wchar_t const* const pngPath);

	void ExePy(wchar_t const* const pyPath, char const* const args);

	const std::vector<std::pair<std::string, std::string>>& CreateDict();

	/*
	* Throws std::invalid_argument() if can't open the path */
	std::vector<ItemEntry> ReadPythonResults(wchar_t const* const resultsPath);

	/*
	* Throws std::invalid_argument() if can't open the path */
	std::string ReadOCRResultsForPython(wchar_t const* const resultsPath);
}