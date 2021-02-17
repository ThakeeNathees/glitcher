/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef MS_TYPES
#define MS_TYPES

#include "common.h"

// Objects and types reltaed to the MiniScript VM.


class _ErrorInfo : public Reference {
	GODOT_CLASS(_ErrorInfo, Reference);

public:
	String message; //< The error message;
	String file;    //< The script file path.
	int line;       //< 1 based error line.
	static void _register_methods() {
		register_property<_ErrorInfo, String>("message", &_ErrorInfo::message, "");
		register_property<_ErrorInfo, String>("file", &_ErrorInfo::file, "");
		register_property<_ErrorInfo, int>("line", &_ErrorInfo::line, 0);
	}

	void _init() {
		line = 0;
	}

};

#endif // MS_TYPES
