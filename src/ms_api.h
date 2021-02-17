/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef MS_API_H
#define MS_API_H

#include <sstream>
#include <vector>

#include "common.h"
#include "miniscript.h"
#include "types.h"

#include <Node.hpp>


// MiniScript is an autoloaded script in godot. And it should be treated like
// a global state machine. One need to clean errors or stdout before running
// a new script. Once the script done running they have to check for errors
// get the output etc. It's inheriting Node instead of Object is because it'll
// be used as an auto loaded script and autoloaded script must inherit from 
// Node to add that to the scene tree.
class MiniScript : public Node {
	GODOT_CLASS(MiniScript, Node);

	MSVM* vm = NULL;
	MSInterpretResult last_result = RESULT_SUCCESS;

	// Std out of the last interpreted script.
	std::stringstream ms_stdout;

	// Error/stdout Arrays are array of Ref<_ErrorInfo> for runtime error it'll
	// contain runtime error entry and stack trace entries.
	Array ms_stderr_compile;
	Array ms_stderr_runtime;

	// TODO: Current source. temproary for testing.
	String test_source;

	// Declare configuration function as friend, So that it can access it's io
	// streams, interpret result, etc.
	friend void errorPrint(MSVM* vm, MSErrorType type, const char* file,
		                     int line, const char* message);
	friend void writeFunction(MSVM* vm, const char* text);
	friend void loadScriptDone(MSVM* vm, const char* path, void* user_data);
	friend MSLoadScriptResult loadScript(MSVM* vm, const char* path);

public:
	static void _register_methods();

	MiniScript();
	~MiniScript();

	void _init();
	void _process(float delta) {}

	// Clear errors stdout last result.
	void clear();

	String get_stdout();
	Array get_errors(); //< Returns an array of _ErrorInfo.

	// TODO: temp for testing.
	String test_hello() {
		return "Hello!";
	}

	void set_source(const String& source) {
		test_source = source;
	}

	int interpret() {
		clear();
		last_result = msInterpret(vm, "TODO:");
		return last_result;
	}

};


#endif // MS_API_H
