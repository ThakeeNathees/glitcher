/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef MS_API_H
#define MS_API_H

#include "common.h"
#include <Node.hpp>

// TODO: Remove extern "C".
extern "C" {
	#include "miniscript.h"
}

class MiniScript : public Node {
	GODOT_CLASS(MiniScript, Node);

	MSVM* vm = NULL;

public:
	static void _register_methods();

	void _init();
	void _process(float delta) {}

	String test_hello() {
		return "Hello!";
	}

};


#endif // MS_API_H
