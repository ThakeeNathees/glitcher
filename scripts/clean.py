from pathlib import Path ## python 3.4
import shutil
import os, sys

SCRIPT_PATH = Path(os.path.realpath(__file__))
ROOT = str(SCRIPT_PATH.parent.parent)

def main():
	clean()

def log(*msg):
	print('[clean.py] ', end='')
	for _msg in msg:
		print(' ' + _msg, end='')
	print()

def remove_all(list):
	for path in list:
		if os.path.isdir(path):
			remove_dir(path)
		else:
			remove_file(path)

def remove_dir(path):
	if os.path.exists(path):
		try:
			shutil.rmtree(path)
			log('Removed:', path)
		except PermissionError:
			log('PermissionError:', path)

def remove_file(path):
	if os.path.exists(path):
		try:
			os.remove(path)
			log('Removed:', path)
		except PermissionError:
			log('PermissionError:', path)

## the default clean method
def clean():
	os.chdir(ROOT)
	remove_list = [
		'__pycache__/', 'config.json', '.sconsign.dblite'
	]
	if sys.platform == 'win32':
		remove_list += [
			## dirs
			'build/', 'x64/', 'debug/', '.vs/', 
			## files
			'MiniScript.sln',
			'MiniScript.vcxproj',
			'MiniScript.vcxproj.filters',
			'MiniScript.vcxproj.user',
		]
		
		for file in os.listdir('.'):
			if os.path.isfile(file):
				if file.endswith('.idb') or file.endswith('.pdb'):
					remove_list.append(file)
	remove_all(remove_list)
	
	log('done cleaning')

if __name__ == '__main__':
	main()