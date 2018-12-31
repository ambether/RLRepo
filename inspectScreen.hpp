#pragma once

class InspectScreen : public UiElement {
public:
	InspectScreen(int width, int height);
	void render() override;
};