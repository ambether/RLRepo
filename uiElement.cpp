#include "main.hpp"

UiElement::UiElement(int width, int height) : width(width), height(height) { con = new TCODConsole(width, height); }

UiElement::~UiElement() { delete con; }

int UiElement::getWidth() const { return width; }

int UiElement::getHeight() const { return height; }
