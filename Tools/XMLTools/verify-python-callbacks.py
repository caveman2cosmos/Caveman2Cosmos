import ast
import glob
import re
from lxml import etree
import os
import fnmatch

def find_all_functions(py_file):
    with open(py_file) as f:
        code = f.read()
        a = ast.parse(code, py_file)
        #return [n.name for n in ast.walk(a) if type(n) == ast.FunctionDef]
        return [n.name for n in ast.iter_child_nodes(a) if type(n) == ast.FunctionDef]


def namespace(element):
    m = re.match(r'\{(.*)\}', element.tag)
    return m.group(1) if m else ''

callback_nodes = [
    'PythonCallback',
    'PythonHelp',
    'PythonExpireCheck',
    'PythonCanDo',
    'PythonCanDoCity',
    'PythonCanDoUnit',
    'Python'
]
def verify_callbacks(xml_files, py_files):
    fn_regex = re.compile('(?:[_a-zA-Z][_a-zA-Z0-9]{0,30})\\Z')
    all_functions = {}
    for py_file in py_files:
        all_functions.update([(fn, py_file) for fn in find_all_functions(py_file)])

    # print 'Found ' + str(len(all_functions)) + ' functions defined in '  + str(len(py_files)) + ' Python files'

    for filename in xml_files:
        tree = etree.parse(filename)
        root = tree.getroot()
        nsmap = {'': namespace(root)}
        modified = False
        for node in callback_nodes:
            for text_elem in root.findall('.//' + node, nsmap):
                fn = text_elem.text
                if fn and (fn_regex.match(fn) and fn not in all_functions):
                    print(','.join([str(fn), filename, tree.getpath(text_elem) + '/' + node]))
                    text_elem.text = None
                    # text_elem.getparent().remove(text_elem)
                    modified = True
                #else:
                #    print(fn + ',' + filename + ',' + all_functions[fn])
        if modified:
            etree.ElementTree(root).write(filename, encoding="utf-8", xml_declaration=True, pretty_print=True)

def get_files(root, file_pattern):
    matches = []
    for directory_path, _, filenames in os.walk(root):
        for filename in fnmatch.filter(filenames, file_pattern):
            matches.append(os.path.join(directory_path, filename))
    return matches

CIV4_ASSETS = r'C:\Program Files (x86)\Civilization IV Complete\Civ4\Assets\Python'
BTS_ASSETS = r'C:\Program Files (x86)\Civilization IV Complete\Civ4\Beyond the Sword\Assets\Python'
if __name__ == "__main__":
    xml_files = get_files('Assets', '*.xml')
    py_files = get_files('Assets', '*.py') + get_files(CIV4_ASSETS, '*.py') + get_files(BTS_ASSETS, '*.py')
    verify_callbacks(xml_files, py_files)