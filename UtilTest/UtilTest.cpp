#include "pch.h"
#include "framework.h"
#include "CppUnitTest.h"
#include "Utils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UtilTest
{
	TEST_CLASS(UtilTest)
	{
	public:
		
		TEST_METHOD(TestUnicodeASCIIInWideChars)
		{
			#ifndef UNICODE
				Logger::WriteMessage("Warning: UNICODE MACRO IS NOT DEFINED!!");
			#endif // UNICODE

			const char* source = "Some unicode string\n";
			Logger::WriteMessage("Converting unicode in single bytes\n");
			const auto wide = UnicodeInWideChars(source);
			Logger::WriteMessage("Printing converted str in wide chars\n");
			Logger::WriteMessage(wide.c_str());
		}

		TEST_METHOD(TestUnicodeJapaneseInWideChars)
		{
			#ifndef UNICODE
				Logger::WriteMessage("Warning: UNICODE MACRO IS NOT DEFINED!!");
			#endif // UNICODE

			const char* source = "ユニコードだよ\n";
			Logger::WriteMessage("Converting unicode in single bytes\n");
			const auto wide = UnicodeInWideChars(source);
			Logger::WriteMessage("Printing converted str in wide chars\n");
			Logger::WriteMessage(wide.c_str());
		}
	};
}
