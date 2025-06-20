#include <iostream>
#include <fstream>
#include <expected>
#include <string>
#include <string_view>
#include <optional>

#define DEBUG 0

inline auto check_args(const int argc, const char *argv[]) -> std::expected<std::pair<std::string_view, std::string_view>, std::string_view>
{
	switch (argc)
	{
	case 3:
		return std::move(std::pair{argv[1], argv[2]});
		break;
	default:
		return std::move(std::unexpected("wrong argument number"));
	}
}

auto fetch_content(const std::string_view &filename) -> std::expected<std::string, std::string>
{
	try
	{
		std::string content;
		std::ifstream stream;

#if DEBUG
		std::cout << "filename for fetching: " << filename << "\n";
#endif

		stream.open(filename);

		char c;
		while (stream.get(c))
		{
			content.push_back(c);
		}

		stream.close();

#if DEBUG
		std::cout << "content: " << content << "\n";
#endif

		return content;
	}
	catch (const std::exception &e)
	{
		return std::unexpected(e.what());
	}
};

auto format_to_str(
	const std::string_view input_str,
	const std::string_view &outputfile) -> std::expected<std::string, std::string_view>
{
	if (not input_str.empty())
	{
		std::string output_str_name;

		for (const auto &e : outputfile)
		{
			if (e == '.')
			{
				break;
			}

			output_str_name.push_back(e);
		}

		try
		{
			std::string format_result_str =
				"#pragma once\n\n#include <string_view>\n\n\nconstexpr std::string_view ";
			format_result_str.append(output_str_name);
			format_result_str.append(" = \"");

			format_result_str.reserve(80);

			for (const auto &c : input_str)
			{
				switch (c)
				{
				case '\n':
					format_result_str.append("\\n");
					break;
				case '\"':
					format_result_str.append("\\\"");
					break;
				case '\t':
					format_result_str.append("\\t");
					break;
				case '\r':
					format_result_str.append("\\r");
					break;
				case '\\':
					format_result_str.append("\\\\");
					break;
				default:
					format_result_str.push_back(c);
					break;
				}
			}

			format_result_str.append("\";");

			return std::move(format_result_str);
		}
		catch (const std::exception &e)
		{
			return std::unexpected(e.what());
		}
	}
	else
	{
		return std::unexpected("string is empty");
	}
};

/// @return if there is a str, it means, something went wrong
auto save_data(
	const std::string_view &outfile,
	const std::string &str) -> std::optional<std::string_view>
{
	try
	{
		std::ofstream stream;

		stream.open(outfile);

		stream << str;

		stream.close();

		return std::nullopt;
	}
	catch (const std::exception &e)
	{
		return e.what();
	}
};

auto main(const int argc, const char *argv[]) -> int
{
#if DEBUG
	std::cout << "arguments taken it:\n";

	for (size_t i = 0; i < argc; i++)
	{
		std::cout << "" << argv[i];

		if (i != argc - 1)
		{
			std::cout << ", ";
		}
	}

	std::cout << "\n\n";
#endif

	const auto &check_args_result = check_args(argc, argv);

	if (check_args_result.has_value())
	{
		const auto &[inputfile, ouputfile] = check_args_result.value();
#if DEBUG
		std::cout << "inputfile: " << inputfile << "\n";
		std::cout << "outputfile: " << ouputfile << "\n";
#endif
		const auto &fetch_result = fetch_content(inputfile);

		if (fetch_result.has_value())
		{
#if DEBUG
			std::cout << "fetch result value:\n"
					  << fetch_result.value() << "\n";
#endif

			const auto &format_result =
				format_to_str(fetch_result.value(), ouputfile);

			if (format_result.has_value())
			{
				const auto &data = format_result.value();

				const auto &save_error =
					save_data(ouputfile, data);

				if (save_error.has_value())
				{
					std::cout << "error when saving: " << save_error.value() << "\n";
				}
#if DEBUG
				else
				{
					std::cout << "success!\n";
				}
#endif
			}
			else
			{
				std::cerr << "error when formating: " << format_result.error() << "\n";
			}
		}
		else
		{
			std::cerr << "error when fetching: " << fetch_result.error() << "\n";
		}
	}
	else
	{
		std::cerr << "error when checking args: " << check_args_result.error() << "\n";
	}
};
