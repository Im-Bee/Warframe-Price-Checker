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

	std::vector<ItemEntry> ReadOCRResults(wchar_t const* const resultsPath);
}