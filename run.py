import os
import platform
from subprocess import run, PIPE

# how your executable should be named
project_name: str = "file2str"

# arguments you wonna use for testing
testing_arguments = "t.txt a.h"


# build code (don't touch it, unless you know what your doing)
windows: bool = platform.system() == "Windows"

if windows:
	run("rmdir /S /Q build", shell = True)
else:
	run("rm -rf build", shell = True)

if not os.path.exists("build"):
	run("mkdir build", shell=True)

def execute_cmd(cmd: str, description: str):
	conan_result = run(
		cmd,
		shell=True,
		stdout=PIPE,
		stderr=PIPE
	)

	if conan_result.returncode == 0:
		print(description + " was successful!!!\n\n\n")
	else:
		print("---DETAIL ZONE---")
		if conan_result.stdout:
			print(conan_result.stdout)
		if conan_result.stderr:
			print(conan_result.stderr)
		print("---END DETAIL ZONE---")
		
		print("\n\n\n" + description + " has failed! (details are above)")
		exit(1)



cmds = [
			(
				"conan install . --build=missing -of build",
				"Conan installing packages required by your conanfile.txt"
			),
			(
				"cmake . -B build",
				"CMake pre generating the stuff necessary for building your project"
			),
			(
				"make -C build",
				"Make trying to build your project form the cmake generated stuff"
			)
		]

for cmd in cmds:
	execute_cmd(cmd[0], cmd[1])
# end building code

# testign code
run([f"mkdir testing"], shell=True)

if windows:
	if os.path.exists(f"build\\{project_name}_exec.exe"):
		run([f"cp build\\{project_name}_exec.exe testing"], shell=True, check=True)
		run([f".\\testing\\{project_name}_exec.exe {testing_arguments}"], shell=True, check = True)
else:
	if os.path.exists(f"build/{project_name}_exec"):
		run([f"cp build/{project_name}_exec testing"], shell=True, check=True)
		run([f"./build/{project_name}_exec {testing_arguments}"], shell=True, check = True)
# end testing code
