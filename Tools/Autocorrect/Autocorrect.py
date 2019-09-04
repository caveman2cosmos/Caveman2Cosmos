import argparse
import xml.etree.ElementTree as ET
import re
from pylanguagetool import api

def load_string_list(filename):
    try:
        return [line.strip() for line in open(filename)]
    except:
        return []

def save_string_list(filenamem, strings):
    with open(filenamem, 'w') as f:
        for item in strings:
            f.write('%s\n' % item)

def namespace(element):
    m = re.match(r'\{(.*)\}', element.tag)
    return m.group(1) if m else ''

def autocorrect(files, apply_changes):
    ignore_words = load_string_list('ignore_word_list.txt')
    ignore_tags = load_string_list('ignore_tag_list.txt')

    for filename in files:
        # text_file = ET.parse(filename)
        root = ET.parse(filename).getroot()
        ns = namespace(root)
        ET.register_namespace('', ns)
        for text_elem in root.findall('{%s}TEXT' % ns):
            tag_elem = text_elem.find('{%s}Tag' % ns)
            if tag_elem is not None and tag_elem.text not in ignore_tags:
                eng_elem = text_elem.find('{%s}English' % ns)
                if eng_elem is not None :
                    autocorrect_element(eng_elem, ignore_words, apply_changes)

        if apply_changes:
            with open(filename, 'w') as f:
                f.write(ET.tostring(root, encoding='utf8', method='xml'))

    save_string_list('ignore_word_list.txt', ignore_words)
    save_string_list('ignore_tag_list.txt', ignore_tags)

def autocorrect_element(eng_elem, ignore_words, apply_changes):
    results = api.check(eng_elem.text,
                        api_url='http://localhost:8081/v2/', lang='en-US')
    if results and 'matches' in results and len(results['matches']) > 0:
        not_ignored = [r for r in results['matches'] if eng_elem.text[r['offset']:r['offset']+r['length']] not in ignore_words]
        if len(not_ignored) > 0:
            print('Errors found in text "%s":' % eng_elem.text)
            for match in not_ignored:
                print('    %s: %s' % (match['message'], match['context']['text']))
                if 'replacements' in match:
                    print('    - Suggestions: %s' %
                        ', '.join([v['value'] for v in match['replacements']]))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Detect grammar and spelling mistakes in an XML file using Language Tool 4.6, and optionally correct them.')
    parser.add_argument('files', metavar='file', type=str,
                        nargs='+', help='files to process')
    parser.add_argument('--apply-changes', dest='apply_changes', action='store_true',
                        help='apply the proposed changed (default just displays them)')

    args = parser.parse_args()

    autocorrect(args.files, args.apply_changes)
