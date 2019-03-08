#pragma once

#include <string>
#include <map>
#include <System/Scene.h>
#include <System/Math.h>
#include <Graphics/Color.h>
#include <SDL.h>

/**
 * CommandLine
 * -----------
 */
class CommandLine {
public:
	void backspace();
	void close();
	const std::string& getCurrentCommand() const;
	void executeCurrentCommand();
	bool isOpen() const;
	void open();
	void queueCharacter(char character);
	void setActiveScene(Scene* scene);

private:
	std::string currentCommand = "";
	Scene* activeScene = nullptr;
	bool isQueueingCommand = false;

	void parseCurrentCommand(std::string& term, std::string& value);
	void parseColor(std::string& value, Color& color);
	void parseVec3(std::string& value, Vec3& vector);
	void parseTriple(std::string& value, std::string (&triple)[3]);
};
