import argparse
import glob
import re
from lxml import etree
# You might need to install lxml manually, for example trough Python 3 installed by Git tools - Tools\python\tools\Scripts has pip exe, that you need to run trough command console

def sortkey(child, nsmap):
	# 'Tag' will sort GameText files, 'Type' will sort Infos files after some tweaks, or temporarily comment out outermost tags in infotypes
	tag = child.find('Tag',nsmap)
	if(tag is not None):
		return tag.text
	return ''

def namespace(element):
	m = re.match(r'\{(.*)\}', element.tag)
	return m.group(1) if m else ''

def sort_xmls(files_to_sort):
	for filename in files_to_sort:
		print('Sorting %s' % filename)
		root = etree.parse(filename).getroot()
		nsmap = {'': namespace(root)}
		try:
			root[:] = sorted(root, key=lambda child: sortkey(child, nsmap))
		finally:
			etree.ElementTree(root).write(
				filename, encoding="utf-8", xml_declaration=True, pretty_print=True)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description='Sort xml text file by Tag or Type.')
	parser.add_argument('glob_patterns', metavar='glob', type=str,
						nargs='+', help='globs describing what files to process (e.g. "*_CIV4GameText.xml")')
	args = parser.parse_args()

	expanded_files = [glob.glob(g) for g in args.glob_patterns]
	flattened_files = [f for files in expanded_files for f in files]
	unique_files = list(set(flattened_files))
	if not unique_files:
		print('No files found matching the provided globs!')
	else:
		if len(unique_files) > 1:
			print('Processing %d files...' % len(unique_files))

		sort_xmls(unique_files)
		print('... Complete!')
