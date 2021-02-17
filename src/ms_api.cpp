/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "ms_api.h"

void MiniScript::_register_methods() {

	register_method("_init", &MiniScript::_init);
	register_method("_process", &MiniScript::_process);

	register_method("get_stdout", &MiniScript::get_stdout);
	register_method("get_errors", &MiniScript::get_errors);
	register_method("interpret", &MiniScript::interpret);

	register_method("test_hello", &MiniScript::test_hello);
	register_method("set_source", &MiniScript::set_source);


	//register_signal<MiniScript>("done_compilation" /*, TODO: ARGS*/);
	//register_signal<MiniScript>("done_interpretation" /*, TODO: ARGS*/);
}

/*****************************************************************************/
/* MINISCRIPT CONFIGURATIONS                                                 */
/*****************************************************************************/

static void errorPrint(MSVM* vm, MSErrorType type, const char* file, int line,
  const char* message) {
	MiniScript* self = (MiniScript*)msGetUserData(vm);
	
	Ref<_ErrorInfo> info;
	info.instance();
	_ErrorInfo* err = info.ptr();

	err->message = message;
	err->file = file;
	err->line = line;

	switch (type) {
	  case MS_ERROR_COMPILE:    self->ms_stderr_compile.append(info); break;
	  case MS_ERROR_RUNTIME:    self->ms_stderr_runtime.append(info); break;
	  case MS_ERROR_STACKTRACE: self->ms_stderr_runtime.append(info); break;
	}
}

static void writeFunction(MSVM* vm, const char* text) {
	// TODO: Prevent crash due to `while true do print("something")`.
	MiniScript* self = (MiniScript*)msGetUserData(vm);
	self->ms_stdout << text;
}

static void loadScriptDone(MSVM* vm, const char* path, void* user_data) {
	MiniScript* self = (MiniScript*)msGetUserData(vm);
	//TODO;
}

static MSLoadScriptResult loadScript(MSVM* vm, const char* path) {
	MiniScript* self = (MiniScript*)msGetUserData(vm);
	MSLoadScriptResult result;
	result.is_failed = false;
	result.source = self->test_source.ascii().get_data(); /* <------- */ //TODO;
	result.user_data = NULL;
	return result;
}

void MiniScript::_init() {
	MSConfiguration config;
	config.user_data = (void*)this;
	config.error_fn = errorPrint;
	config.write_fn = writeFunction;
	config.load_script_fn = loadScript;
	config.load_script_done_fn = loadScriptDone;
	vm = msNewVM(&config);
}

/*****************************************************************************/
/* MINISCRIPT API METHODS                                                    */
/*****************************************************************************/

MiniScript::MiniScript() {
}

MiniScript::~MiniScript() {
}

void MiniScript::clear() {
	last_result = RESULT_SUCCESS;
	ms_stdout.str("");
	ms_stderr_compile.clear();
	ms_stderr_runtime.clear();
}


String MiniScript::get_stdout() {
	return ms_stdout.str().c_str();
}

Array MiniScript::get_errors() {
	switch (last_result) {
		case RESULT_SUCCESS:
			return Array();
		case RESULT_COMPILE_ERROR:
			return ms_stderr_compile;
		case RESULT_RUNTIME_ERROR:
			return ms_stderr_runtime;
	}
	UNREACHABLE();
	return Array();
}


