#include "stdafx.h"
#include "CppUnitTest.h"
#include "randomStringGenerator.h"
#include "consts.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace COMServerUnitTest
{		
	TEST_CLASS(RandomStringGeneratorTest)
	{
	public:
		
		TEST_METHOD(TestGenerate)
		{
			wstring str1,str2;
			RandomStringGenerator::generate(str1);
			RandomStringGenerator::generate(str2);
			Assert::AreEqual((size_t)NAME_LENGTH, str1.size());
			Assert::AreEqual((size_t)NAME_LENGTH, str2.size());
			Assert::AreNotEqual(str1, str2);
		}

	};

}