#include "CppUnitTest.h"
#include "Utils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UtilTest
{
	TEST_CLASS(UtilTest)
	{
	public:
		
		TEST_METHOD(TestWideCharFromLegacyByteArrayWithASCII)
		{
			#ifndef UNICODE
				Logger::WriteMessage("Warning: UNICODE MACRO IS NOT DEFINED!!");
			#endif // UNICODE

			const char* source = "Some unicode string";
			Logger::WriteMessage("Converting unicode in single bytes\n");
			Logger::WriteMessage("Before: ");
			Logger::WriteMessage(source);
			Logger::WriteMessage("\n");

			Logger::WriteMessage("After: ");
			const auto wide = HandyAudioControl::WideCharFromLegacyByteArray(source);
			Logger::WriteMessage(wide.c_str());
			Logger::WriteMessage("\n");
		}

		TEST_METHOD(TestWideCharFromLegacyByteArrayWithJapanese)
		{
			#ifndef UNICODE
				Logger::WriteMessage("Warning: UNICODE MACRO IS NOT DEFINED!!");
			#endif // UNICODE

			const char* source = "ユニコードだよ";
			Logger::WriteMessage("Converting unicode in single bytes\n");
			Logger::WriteMessage("Before: ");
			Logger::WriteMessage(source);
			Logger::WriteMessage("\n");

			Logger::WriteMessage("After: ");
			const auto wide = HandyAudioControl::WideCharFromLegacyByteArray(source);
			Logger::WriteMessage(wide.c_str());
			Logger::WriteMessage("\n");
		}

		TEST_METHOD(TestWideCharFromU8WithASCII)
		{
#ifndef UNICODE
			Logger::WriteMessage("Warning: UNICODE MACRO IS NOT DEFINED!!");
#endif // UNICODE

			const auto source = u8"Some unicode string";
			Logger::WriteMessage("Converting unicode in single bytes\n");
			Logger::WriteMessage("Before: ");
			Logger::WriteMessage("CANNOT PRINT UNICODE WITH WriteMessage()");
			Logger::WriteMessage("\n");

			Logger::WriteMessage("After: ");
			const auto wide = HandyAudioControl::WideCharFromU8(source);
			Logger::WriteMessage(wide.c_str());
			Logger::WriteMessage("\n");
		}

		TEST_METHOD(TestWideCharFromU8WithJapanse)
		{
#ifndef UNICODE
			Logger::WriteMessage("Warning: UNICODE MACRO IS NOT DEFINED!!");
#endif // UNICODE

			const auto source = u8"ユニコードだよ";
			Logger::WriteMessage("Converting unicode in single bytes\n");
			Logger::WriteMessage("Before: ");
			Logger::WriteMessage("CANNOT PRINT UNICODE WITH WriteMessage()");
			Logger::WriteMessage("\n");

			Logger::WriteMessage("After: ");
			const auto wide = HandyAudioControl::WideCharFromU8(source);
			Logger::WriteMessage(wide.c_str());
			Logger::WriteMessage("\n");
		}
	};
}
