#pragma once

#define ALERT_ERROR "Error"
#define ALERT_ASSET_ERROR "Asset error"
#define ALERT_AUDIO_ERROR "Audio error"

namespace Alert {
	void error(const char* title, const char* message);
	void warning(const char* title, const char* message);
}
