import sys, subprocess
from os import listdir
from os.path import isfile, join

def load_files(mypath):
    return [f for f in listdir(mypath) if isfile(join(mypath, f))]


## LOAD FILE
def load_file(filename):
    f = open(filename, "r")
    data = f.read()
    f.close()

    lines = data.strip().split('\n')
    return '\n'.join(lines[1:])


def make_single_file(files):
    result = []
    for file in files:
        result.append(load_file(file))

    return '\n'.join(result)


def write_file(filename, data):
    f = open(filename, "w")
    f.write(data)
    f.close()


## MAIN CODE
if len(sys.argv) < 2:
    print("Missing parameter filepath!")
    exit(1)

filepath = sys.argv[1]
print("Loading " + filepath)

files = load_files(filepath)
files.sort()
print("Found files:")
[print(file) for file in files]

absolute_files = [filepath + '/' + file for file in files]
file = make_single_file(absolute_files)
write_file('result.csv', file)
