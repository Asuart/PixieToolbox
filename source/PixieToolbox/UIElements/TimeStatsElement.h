#pragma once
#include <Window/UIElement.h>

class PixieEngineEditor;
class Interface;

class TimeStatsElement : public UIElement {
public:
	TimeStatsElement();

	void Draw() override;
};
