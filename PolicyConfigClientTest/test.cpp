#include "pch.h"
#include "PolicyConfigClient.h"

TEST(TestSetDefaultAudioEndPoint, InvalidDeviceId) {
	ASSERT_FALSE(SetDefaultAudioEndPoint(TEXT("SomeInvalidId"), ERole::eCommunications));
	ASSERT_FALSE(SetDefaultAudioEndPoint(TEXT("SomeInvalidId"), ERole::eMultimedia));
	ASSERT_FALSE(SetDefaultAudioEndPoint(TEXT("SomeInvalidId"), ERole::eConsole));
}