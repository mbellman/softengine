#include <System/CommandLine.h>
#include <string>
#include <Helpers.h>

static std::string AMBIENT_LIGHT_COLOR = "ambientLightColor";
static std::string AMBIENT_LIGHT_VECTOR = "ambientLightVector";
static std::string AMBIENT_LIGHT_FACTOR = "ambientLightFactor";
static std::string BACKGROUND_COLOR = "backgroundColor";
static std::string BRIGHTNESS = "brightness";
static std::string VISIBILITY = "visibility";

/**
 * CommandLine
 * -----------
 */
void CommandLine::backspace() {
	if (currentCommand.size() > 0) {
		currentCommand.erase(currentCommand.end() - 1);
	}
}

void CommandLine::close() {
	currentCommand.assign("");
	isQueueingCommand = false;
}

const std::string& CommandLine::getCurrentCommand() const {
	return currentCommand;
}

void CommandLine::executeCurrentCommand() {
	std::string term;
	std::string value;

	parseCurrentCommand(term, value);

	if (term == AMBIENT_LIGHT_COLOR) {
		Color color;

		parseColor(value, color);

		activeScene->settings.ambientLightColor = color;
	} else if (term == AMBIENT_LIGHT_FACTOR) {
		activeScene->settings.ambientLightFactor = stof(value);
	} else if (term == AMBIENT_LIGHT_VECTOR) {
		Vec3 vector;

		parseVec3(value, vector);

		activeScene->settings.ambientLightVector = vector;
	} else if (term == BACKGROUND_COLOR) {
		Color backgroundColor;

		parseColor(value, backgroundColor);

		activeScene->settings.backgroundColor = backgroundColor;
	} else if (term == BRIGHTNESS) {
		activeScene->settings.brightness = stof(value);
	} else if (term == VISIBILITY) {
		activeScene->settings.visibility = stoi(value);
	}

	close();
}

bool CommandLine::isOpen() const {
	return isQueueingCommand;
}

void CommandLine::open() {
	isQueueingCommand = true;
}

void CommandLine::parseColor(std::string& value, Color& color) {
	std::string triple[3];

	parseTriple(value, triple);

	color.R = FAST_CLAMP(stoi(triple[0]), 0, 255);
	color.G = FAST_CLAMP(stoi(triple[1]), 0, 255);
	color.B = FAST_CLAMP(stoi(triple[2]), 0, 255);
}

void CommandLine::parseCurrentCommand(std::string& term, std::string& value) {
	int termValueDelimiterPos = currentCommand.find(" ");

	if (termValueDelimiterPos != std::string::npos) {
		term = currentCommand.substr(0, termValueDelimiterPos);
		value = currentCommand.substr(termValueDelimiterPos + 1);
	}
}

void CommandLine::parseTriple(std::string& value, std::string (&triple)[3]) {
	int offset = 0;

	for (int i = 0; i < 3; i++) {
		int nextDelimiterPos = value.find(",", offset);

		int substrLength = nextDelimiterPos != std::string::npos
			? nextDelimiterPos - offset
			: std::string::npos;

		triple[i] = value.substr(offset, substrLength);
		offset = nextDelimiterPos + 1;
	}
}

void CommandLine::parseVec3(std::string& value, Vec3& vector) {
	std::string triple[3];

	parseTriple(value, triple);

	vector.x = stof(triple[0]);
	vector.y = stof(triple[1]);
	vector.z = stof(triple[2]);
}

void CommandLine::queueCharacter(char character) {
	bool isLetter = (character >= 65 && character <= 90) || (character >= 97 && character <= 122);
	bool isNumber = character >= 48 && character <= 57;

	if (isLetter || isNumber || character == ' ' || character == '.' || character == '-' || character == ',') {
		currentCommand += character;
	}
}

void CommandLine::setActiveScene(Scene* scene) {
	activeScene = scene;
}
