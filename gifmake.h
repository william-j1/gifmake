#pragma once
#include <stdint.h>

class Gifmake
{
public:
	/*
	menu id for OnSetDura
	*/
	static const uint8_t MENU_ID_SET_DURAS = 0x00;

	/*
	resolution for frames

	decreasing this value will increase the file size
	and increase the fidelity of the animation
	*/
	static const uint8_t FRAMES_RESOLUTION = 100;
};