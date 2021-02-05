from pathlib import Path ## python 3.4
import os, sys

## usage buffergen.py [--clean]

SCRIPT_PATH = Path(os.path.realpath(__file__))
ROOT = str(SCRIPT_PATH.parent)

GEN_LIST = [
	## name     type
	('Int',     'int'),
	('Byte',    'uint8_t'),
]

def log(msg):
	print('[buffergen.py]', msg)

def gen():
	cwd = os.getcwd()
	os.chdir(ROOT)
	_gen()
	os.chdir(cwd)
	return 0

def clean():
	cwd = os.getcwd()
	os.chdir(ROOT)
	_clean()
	os.chdir(cwd)
	return 0

def _gen():

	header = ''
	source = ''
	with open('buffer.template.h', 'r') as f:
		header = f.read()
	with open('buffer.template.c', 'r') as f:
		source = f.read()

	for _type in GEN_LIST:
		_header = header.replace('''\
/** A place holder typedef to prevent IDE syntax errors. Remove this line 
 * when generating the source. */
typedef uint8_t $type$;
''', '')
		_header = _header.replace('_$name$', '_' + _type[0].lower())
		_header = _header.replace('$name$', _type[0])
		_header = _header.replace('$type$', _type[1])

		_source = source.replace('''\
/** Replace the following line with "$name$_buffer.h" */
#include "buffer.template.h"''', '#include "%s_buffer.gen.h"' % _type[0].lower())
		_source = _source.replace('_$name$', '_' + _type[0].lower())
		_source = _source.replace('$name$', _type[0])
		_source = _source.replace('$type$', _type[1])

		with open(_type[0].lower() + '_buffer.gen.h', 'w') as f:
			f.write(_header)
			log(_type[0].lower() + '_buffer.gen.h' + ' generated' )
		with open(_type[0].lower() + '_buffer.gen.c', 'w') as f:
			f.write(_source)
			log(_type[0].lower() + '_buffer.gen.c' + ' generated' )


def _clean():
	for _type in GEN_LIST:
		if os.path.exists(_type[0].lower() + '_buffer.gen.h'):
			os.remove(_type[0].lower() + '_buffer.gen.h')
			log(_type[0].lower() + '_buffer.gen.h' + ' removed')
		if os.path.exists(_type[0].lower() + '_buffer.gen.c'):
			os.remove(_type[0].lower() + '_buffer.gen.c')
			log(_type[0].lower() + '_buffer.gen.c' + ' removed')

def error_exit(msg):
	print("Error: %s\n\tusage buffergen.py [--clean]" % msg)
	exit(1)

if __name__ == '__main__':
	if len(sys.argv) > 2:
		error_exit("invalid arg count")
	if len(sys.argv) == 2:
		if sys.argv[1] == '--clean':
			_clean()
		else:
			error_exit("unknown argument")
	else:
		_gen()
	exit(0)