#!python3.12

from os import *
from os.path import *

monaco_editor_tgz = "monaco-editor.tgz"

system(f"curl https://registry.npmjs.org/monaco-editor/-/monaco-editor-0.52.0.tgz > {monaco_editor_tgz}")
system(f"tar -xf {monaco_editor_tgz} -C Prompt/monaco-editor")

remove(monaco_editor_tgz)

