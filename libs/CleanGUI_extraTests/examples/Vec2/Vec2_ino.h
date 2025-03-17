// by Marius Versteegen, 2023

#include <crt_CleanRTOS.h>        // This file includes crt_Config.h  You'll need to change defines there for a release build.
#include <crt_Vec2.h>

// All Tasks should be created in this main file.

void logVec2(const char* tag, crt::Vec2& vec2)
{
	ESP_LOGI(tag, "x=%d, y=%d", vec2.x, vec2.y);
}

void setup()
{
	ESP_LOGI("checkpoint", "start of main");

	crt::Vec2 a(1, 2);
	crt::Vec2 b(3, 4);

	logVec2("a", a);
	logVec2("b", b);

	crt::Vec2 c = a + b;
	logVec2("c=a+b", c);
	
	c = a - b;
	logVec2("c=a-b", c);

	c = a * b;
	logVec2("c=a*b", c);

	c = a * 2;
	logVec2("c=a*2", c);

	a *= 2;
	logVec2("a*=2", a);

	a += b;
	logVec2("a+=b", a);

	a -= b;
	logVec2("a-=b", a);

	crt::Vec2 d(a);
	logVec2("crt::Vec2 d(a)", d);

	if (b == a)
	{
		ESP_LOGI("?!","b==a");
	}
	else
	{
		ESP_LOGI("ok","!(b==a)");
	}

	if (b != a)
	{
		ESP_LOGI("ok", "b!=a");
	}
	else
	{
		ESP_LOGI("?!", "!(b!=a)");
	}

	b = a;
	logVec2("b=a, vec b", b);

	if (b == a)
	{
		ESP_LOGI("ok", "b==a");
	}
	else
	{
		ESP_LOGI("?!","!(b==a)");
	}

	if (b != a)
	{
		ESP_LOGI("?!", "b!=a");
	}
	else
	{
		ESP_LOGI("ok", "!(b!=a)");
	}
}

void loop()
{
	vTaskDelay(1);// Nothing to do in loop - all example code runs in the 4 threads above.
}
