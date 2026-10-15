#pragma once
#include <charconv>
#include <concepts>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace csv_detail
{
	template<typename T, typename Tuple>
	struct ConstructibleFromTuple : std::false_type {};

	template<typename T, typename... Args>
	struct ConstructibleFromTuple<T, std::tuple<Args...>>
		: std::is_constructible<T, Args...> {};

	template<typename>
	inline constexpr bool always_false = false;
	
	inline std::string_view trim(std::string_view s)
	{
		constexpr std::string_view ws = " \t\r\n";
		const auto first = s.find_first_not_of(ws);

		if (first == std::string_view::npos) return {};
		const auto last = s.find_last_not_of(ws);
		return s.substr(first, last - first + 1);
	}

	inline std::vector<std::string_view> split(std::string_view line)
	{
		std::vector<std::string_view> cells;
		std::size_t start = 0;
		while (true)
		{
			const auto comma = line.find(',', start);
			if (comma == std::string_view::npos)
			{
				cells.push_back(trim(line.substr(start)));
				return cells;
			}
			cells.push_back(trim(line.substr(start, comma - start)));
			start = comma + 1;
		}
	}

	template<typename F>
	F parseField(std::string_view s)
	{
		if constexpr (std::is_same_v<F, std::string>)
		{
			return std::string(s);
		}
		else if constexpr (std::is_arithmetic_v<F>)
		{
			F value{};
			const char* end = s.data() + s.size();
			auto [ptr, ec] = std::from_chars(s.data(), end, value);

			if (s.empty() || ec != std::errc{} || ptr != end)
				throw std::invalid_argument("cannot convert '" + std::string(s) + "'");

			return value;
		}
		else
		{
			static_assert(always_false<F>, "Unsupported CSV field type");
		}
	}

	template<typename Tuple, std::size_t... I>
	Tuple parseRow(const std::vector<std::string_view>& cells, std::index_sequence<I...>)
	{
		return Tuple{ parseField<std::tuple_element_t<I, Tuple>>(cells[I])... };
	}
}

// T must declare `using TypeTuple = std::tuple<...>` matching a constructor.
template<typename T>
concept CsvRecord = requires { typename T::TypeTuple; }
&& csv_detail::ConstructibleFromTuple<T, typename T::TypeTuple>::value;

class CsvUtils
{
public:
	template<CsvRecord T>
	static std::vector<T> parse(const std::string& path, bool hasHeader = true);
};

template<CsvRecord T>
std::vector<T> CsvUtils::parse(const std::string& path, bool hasHeader)
{
	using Fields = typename T::TypeTuple;
	constexpr std::size_t N = std::tuple_size_v<Fields>;

	std::ifstream file(path);
	if (!file)
		throw std::runtime_error(path + ": cannot open file");

	std::vector<T> records;
	std::string line;
	std::size_t lineNo = 0;

	while (std::getline(file, line))
	{
		++lineNo;
		if (hasHeader && lineNo == 1) continue;
		if (csv_detail::trim(line).empty()) continue;

		try
		{
			const auto cells = csv_detail::split(line);
			if (cells.size() != N)
				throw std::invalid_argument("expected " + std::to_string(N) +
					" fields, found " + std::to_string(cells.size()));

			Fields fields = csv_detail::parseRow<Fields>(cells, std::make_index_sequence<N>{});
			records.push_back(std::make_from_tuple<T>(std::move(fields)));
		}
		catch (const std::exception& e)
		{
			// Includes validation errors thrown by T's constructor.
			throw std::runtime_error(path + ":" + std::to_string(lineNo) + ": " + e.what());
		}
	}
	return records;
}