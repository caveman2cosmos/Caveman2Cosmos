import argparse
import xml.etree.ElementTree as ET
import re
from pylanguagetool import api


def namespace(element):
    m = re.match(r'\{.*\}', element.tag)
    return m.group(0) if m else ''


def autocorrect(files, apply_changes):
    for filename in files:
        root = ET.parse(filename).getroot()
        ns = namespace(root)
        for type_tag in root.findall(ns + 'TEXT' + '/' + ns + 'English'):
            print(type_tag.text)
            results = api.check(type_tag.text,
                            api_url='http://localhost:8081/v2/', lang='en-US')
            if results:
                for match in results['matches']:
                    print(match['message'] + ': ' + match['context']['text'])
                    if 'replacements' in match:
                        print('  suggestions: ')
                        print('    ' + ', '.join([v['value'] for v in match['replacements']]))
            else:
                print('query failed!')

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Detect grammar and spelling mistakes in an XML file using Language Tool 4.6, and optionally correct them.')
    parser.add_argument('files', metavar='file', type=str,
                        nargs='+', help='files to process')
    parser.add_argument('--apply-changes', dest='apply_changes', action='store_true',
                        help='apply the proposed changed (default just displays them)')

    args = parser.parse_args()

    autocorrect(args.files, args.apply_changes)
