import argparse
import xml.etree.ElementTree as ET
import re
import msvcrt
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

def autocorrect(files, apply_changes, interactive):
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
                    autocorrect_element(eng_elem, tag_elem.text, ignore_words, ignore_tags, apply_changes, interactive)

        if apply_changes:
            with open(filename, 'w') as f:
                f.write(ET.tostring(root, encoding='utf8', method='xml'))

    # We can't have changed the lists unless we were in interactive mode
    if interactive:
        save_string_list('ignore_word_list.txt', ignore_words)
        save_string_list('ignore_tag_list.txt', ignore_tags)

def autocorrect_element(eng_elem, tag, ignore_words, ignore_tags, apply_changes, interactive):
    results = api.check(eng_elem.text,
                        api_url='http://localhost:8081/v2/', lang='en-US')
    if results and 'matches' in results and len(results['matches']) > 0:
        matches = [r for r in results['matches'] if eng_elem.text[r['offset']:r['offset']+r['length']] not in ignore_words]
        if len(matches) > 0:
            # Make sure its sorted by offset (it should be already)
            matches.sort(key = lambda x: x['offset'])

            print('%d errors found in text "%s":' % (len(matches), eng_elem.text))

            # We need to keep track of changes introduced by corrections to 
            # correctly apply consecutive ones
            if apply_changes:
                corrected_text, corrected_text_with_markers = apply_corrections(eng_elem.text, matches)
                if interactive:
                    done = False
                    while not done:
                        print('  Corrected text: %s' % (corrected_text_with_markers))
                        print('  Accept all (return), Reject all (backspace), Interactive (space), Ignore %s (i)?' % tag)
                        print('  >')
                        key = msvcrt.getch()
                        if key == '\r':
                            eng_elem.text = corrected_text
                            print('  Applied all suggested changes')
                            done = True
                        elif key == '\x08':
                            print('  Rejected all suggested changes')
                            done = True
                        elif key == ' ':
                            print('  Entering interactive mode')
                            corrected_text, corrected_text_with_markers = apply_corrections_interactive(eng_elem.text, matches, ignore_words)
                        elif key == 'i':
                            ignore_tags.append(tag)
                            print('  Added %s to the global ignore list' % tag)
                            done = True
                        else:
                            print('  Please select one of the options to continue')
                else:
                    print('  Corrected text: %s' % (corrected_text_with_markers))
                    eng_elem.text = corrected_text

def apply_corrections(text, matches, marker='#'):
    # We need to keep track of changes introduced by corrections to 
    # correctly apply consecutive ones
    offset_adjust = 0
    corrected_text = text

    offset_adjust_with_markers = 0
    corrected_text_with_markers = text
    for match in matches:
        # print('    %s: %s' % (match['message'], match['context']['text']))
        if 'replacements' in match and len(match['replacements']) > 0:
            # print('    - Suggestions: %s' %
            #     ', '.join([v['value'] for v in match['replacements']]))
            replacement = match['replacements'][0]['value']
            offset = match['offset'] + offset_adjust
            orig_text_len = len(corrected_text)
            corrected_text = corrected_text[:offset] + replacement + corrected_text[offset+match['length']:]
            offset_adjust = offset_adjust + (len(corrected_text) - orig_text_len)

            offset = match['offset'] + offset_adjust_with_markers
            orig_text_len = len(corrected_text_with_markers)
            corrected_text_with_markers = corrected_text_with_markers[:offset] + marker + replacement + marker + corrected_text_with_markers[offset+match['length']:]
            offset_adjust_with_markers = offset_adjust_with_markers + (len(corrected_text_with_markers) - orig_text_len)
    return corrected_text, corrected_text_with_markers

def apply_corrections_interactive(text, matches, ignore_words, marker='#'):
    # We need to keep track of changes introduced by corrections to 
    # correctly apply consecutive ones
    offset_adjust = 0
    corrected_text = text

    offset_adjust_with_markers = 0
    corrected_text_with_markers = text
    for match in matches:
        print('    %s: %s' % (match['message'], match['context']['text']))
        if 'replacements' in match and len(match['replacements']) > 0:
            print('    - Suggestions: %s' %
                ', '.join([v['value'] for v in match['replacements']]))
            print('    Accept best (return), Reject (any key), Add to ignore (i)?')
            print('    >')
            key = msvcrt.getch()

            offset = match['offset'] + offset_adjust
            length = match['length']
            if key == '\r':
                replacement = match['replacements'][0]['value']
                orig_text_len = len(corrected_text)
                corrected_text = corrected_text[:offset] + replacement + corrected_text[offset+length:]
                offset_adjust = offset_adjust + (len(corrected_text) - orig_text_len)

                offset = match['offset'] + offset_adjust_with_markers
                orig_text_len = len(corrected_text_with_markers)
                corrected_text_with_markers = corrected_text_with_markers[:offset] + replacement + corrected_text_with_markers[offset+length:]
                offset_adjust_with_markers = offset_adjust_with_markers + (len(corrected_text_with_markers) - orig_text_len)
            elif key == 'i':
                ignore_words.append(corrected_text[offset:offset+length])
            else: # if key == '\x08':
                pass

    return corrected_text, corrected_text_with_markers

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Detect grammar and spelling mistakes in an XML file using Language Tool 4.6, and optionally correct them.')
    parser.add_argument('files', metavar='file', type=str,
                        nargs='+', help='files to process')
    parser.add_argument('--apply-changes', dest='apply_changes', action='store_true',
                        help='apply the proposed changed (default just displays them)')
    parser.add_argument('--interactive', dest='interactive', action='store_true',
                        help='user interactive fix mode')

    args = parser.parse_args()

    autocorrect(args.files, args.apply_changes, args.interactive)
