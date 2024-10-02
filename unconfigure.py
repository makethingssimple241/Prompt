#!python3.12

from os import *
from os.path import *
from shutil import *

monaco_pkg = "Prompt/monaco-editor/package"

if exists(monaco_pkg):
    rmtree(monaco_pkg)

