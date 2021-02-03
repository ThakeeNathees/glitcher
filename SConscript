Import('env')
import os

## only for ide
env.ALL_SOURCES = [] ## source files
env.ALL_HEADERS = [] ## include dirs
env.PROJECT_NAME = "MiniScript"

## compile the miniscript vm first
SConscript('src/miniscript/SConscript')

def target_path(name):
	return os.path.join('bin/', name)
	
## ide debug run target
env.RUN_TARGET = 'Godot 3.2.2.exe --path MiniScript'
if env['platform'] == 'windows':
	env.RUN_TARGET = 'run.bat'

## include directories
CPPPATH = [
	Dir('#src/godot-cpp/godot_headers/'),
	Dir('#src/godot-cpp/include/'),
	Dir('#src/godot-cpp/include/core/'),
	Dir('#src/godot-cpp/include/gen/'),
]

LIBPATH = [ 
	Dir("#src/godot-cpp/bin/"),
	Dir("src/miniscript/bin/"),
]
LIBS = [
	'libgodot-cpp.{}.{}.{}{}'.format(
	env['platform'],
	env['target'],
	env['bits'], ## TODO: -> 32, 64, default,... "arch_suffix",
	env['LIBSUFFIX']),

	'miniscript',
]

## lib sources
SOURCES = [
	Glob("src/test/*.cpp"),
]

library = env.SharedLibrary(
	target  = "bin/testlib",
	source  = SOURCES,
	CPPPATH = CPPPATH,
	LIBPATH = LIBPATH,
	LIBS    = LIBS,
)

## FIXME:
## collect ide sources
env.ALL_SOURCES += SOURCES + [
	Glob("#src/godot-cpp/src/core/*.cpp"),
	Glob("#src/godot-cpp/src/gen/*.cpp"),
	Glob("#src/miniscript/src/*.c"),
	Glob("#src/miniscript/test/*.c"),
	Glob("#src/test/*.cpp"),
	
]
env.ALL_HEADERS += CPPPATH + [
	'src/miniscript/src/',
	'src/miniscript/test/',
	'src/test/',
]

## copy compiled binar to gdnative
#from shutil import copyfile
#if env['platform'] == 'windows':
#	copyfile("bin/testlib.dll",
#		str(File("#MiniScript/test/bin/testlib.dll")))
	
	