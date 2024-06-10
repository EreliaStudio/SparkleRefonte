#pragma once

#include "application/spk_widget_based_application.hpp"
#include "gtest/gtest.h"

class WidgetBasedApplicationTest : public ::testing::Test
{
protected:
	spk::WidgetBasedApplication app;

	void SetUp() override
	{
		// Setup code if needed
	}

	void TearDown() override
	{
		// Teardown code if needed
	}
};