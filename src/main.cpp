#include <iostream>
#include <fstream>
#include <expected>
#include <string>
#include <string_view>
#include <optional>

inline auto check_arg_count(const int argc) -> bool
{
	return argc == 3;
}

auto try_fetch_content(const int argc, const char *argv[]) -> std::optional<std::string>
{
	if (check_arg_count(argc))
	{
		return std::nullopt;
	}

	std::string content;
	std::ifstream stream;
	stream.open(argv[1]);

	stream >> content;

	stream.close();
	return std::move(content);
};

auto format_to_str(const std::string_view input_str) -> std::optional<std::string>
{
	if (not input_str.empty())
	{

		std::string result{"#pragma once\n#include <string_view>\n\nconstexpr std::string_view str = \""};
		result.reserve(80);

		for (const auto &c : input_str)
		{
			switch (c)
			{
			case '\n':
				result.append("\\n");
				break;
			case '\"':
				result.append("\\\"");
				break;
			case '\t':
				result.append("\\t");
				break;
			case '\r':
				result.append("\\r");
				break;
			default:
				result.push_back(c);
				break;
			}
		}

		result.push_back('\";');

		return std::move(result);
	}

	return std::nullopt;
};

/// @return if there is a str, it means, something went wrong
auto save_data(
	const int argc,
	const char *argv[],
	const std::string &str) -> std::optional<std::string_view>
{
	if (check_arg_count(argc))
	{
		std::ofstream stream;

		try
		{
			stream.open(argv[2]);

			stream << str;

			stream.close();
		}
		catch (const std::exception &e)
		{
			return e.what();
		}

		return std::nullopt;
	}

	return "argument validation failed";
};

auto main(const int argc, const char *argv[]) -> int
{
	const auto &fetch_optional = try_fetch_content(argc, argv);

	if (fetch_optional.has_value())
	{
		const auto &format_optional =
			format_to_str(fetch_optional.value());

		if (format_optional.has_value())
		{
			const auto &save_optional_error =
				save_data(argc, argv, format_optional.value());

			if (save_optional_error.has_value())
			{
				std::cout
					<< "\n"
					<< save_optional_error.value()
					<< "\n";
			}
			else
			{
				std::cout
					<< "success!\n";
			}
		}
	}

	return 0;
};
