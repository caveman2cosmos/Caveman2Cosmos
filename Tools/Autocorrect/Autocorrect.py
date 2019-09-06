import argparse
import xml.etree.ElementTree as ET
import re
import msvcrt
import glob
import requests
from pylanguagetool import api

class Mode:
    DETECT = 0
    AUTOMATIC = 1
    INTERACTIVE = 2

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


def autocorrect(files, mode):
    ignore_words = load_string_list('ignore_word_list.txt')
    ignore_tags = load_string_list('ignore_tag_list.txt')

    for filename in files:
        print(filename)
        # text_file = ET.parse(filename)
        root = ET.parse(filename).getroot()
        nspace = namespace(root)
        ET.register_namespace('', nspace)
        for text_elem in root.findall('{%s}TEXT' % nspace):
            tag_elem = text_elem.find('{%s}Tag' % nspace)
            if tag_elem is not None and tag_elem.text not in ignore_tags:
                eng_elem = text_elem.find('{%s}English' % nspace)
                if eng_elem is not None:
                    autocorrect_element(
                        eng_elem, tag_elem.text, ignore_words, ignore_tags, mode, '  ')

        if mode != Mode.DETECT:
            with open(filename, 'w') as fileh:
                fileh.write(ET.tostring(root, encoding='utf8', method='xml'))

    # We can't have changed the lists unless we were in interactive mode
    if mode == 'interactive':
        save_string_list('ignore_word_list.txt', ignore_words)
        save_string_list('ignore_tag_list.txt', ignore_tags)


def autocorrect_element(eng_elem, tag, ignore_words, ignore_tags, mode, indent):
    results = api.check(eng_elem.text,
                        api_url='http://localhost:8081/v2/', lang='en-US')
    if results and 'matches' in results and len(results['matches']) > 0:
        matches = [r for r in results['matches'] if eng_elem.text[r['offset']
            :r['offset']+r['length']] not in ignore_words]
        if len(matches) > 0:
            # Make sure its sorted by offset (it should be already)
            matches.sort(key=lambda x: x['offset'])

            print(indent + '%d errors found in text "%s":' %
                  (len(matches), eng_elem.text))
            indent = indent + '    '
            # We need to keep track of changes introduced by corrections to
            # correctly apply consecutive ones
            if mode == Mode.AUTOMATIC or mode == Mode.INTERACTIVE:
                corrected_text, corrected_text_with_markers = apply_corrections(
                    eng_elem.text, matches)
                if mode == Mode.INTERACTIVE:
                    done = False
                    while not done:
                        print(indent + 'Corrected text: %s' %
                              (corrected_text_with_markers))
                        print(indent + 'Accept all (return), Reject all (backspace), Interactive (space), Ignore %s (i)?' % tag),
                        key = msvcrt.getch()
                        print('')
                        if key == '\r':
                            eng_elem.text = corrected_text
                            print(indent + 'Applied all suggested changes')
                            done = True
                        elif key == '\x08':
                            print(indent + 'Rejected all suggested changes')
                            done = True
                        elif key == ' ':
                            print(indent + 'Entering interactive mode')
                            corrected_text, corrected_text_with_markers = apply_corrections_interactive(
                                eng_elem.text, matches, ignore_words, indent + '  ')
                        elif key == 'i':
                            ignore_tags.append(tag)
                            print(indent + 'Added %s to the global ignore list' % tag)
                            done = True
                        else:
                            print(indent + 'Please select one of the options to continue')
                else:
                    print(indent + 'Corrected text: %s' %
                          (corrected_text_with_markers))
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
            corrected_text = corrected_text[:offset] + \
                replacement + corrected_text[offset+match['length']:]
            offset_adjust = offset_adjust + \
                (len(corrected_text) - orig_text_len)

            offset = match['offset'] + offset_adjust_with_markers
            orig_text_len = len(corrected_text_with_markers)
            corrected_text_with_markers = corrected_text_with_markers[:offset] + marker + \
                replacement + marker + \
                corrected_text_with_markers[offset+match['length']:]
            offset_adjust_with_markers = offset_adjust_with_markers + \
                (len(corrected_text_with_markers) - orig_text_len)
    return corrected_text, corrected_text_with_markers

def highlight_error_in_context(context, marker):
    context_text = context['text']
    offset = context['offset']
    length = context['length']
    prefix = context_text[:offset]
    error = context_text[offset:offset + length]
    postfix = context_text[offset + length:]
    return prefix + marker + error + marker + postfix

def apply_corrections_interactive(text, matches, ignore_words, indent, marker='#'):
    # We need to keep track of changes introduced by corrections to
    # correctly apply consecutive ones
    offs_adj = 0
    corrected_text = text

    offs_adj_mrks = 0
    corrected_text_mrks = text
    for match in matches:
        context_text_marked = highlight_error_in_context(match['context'], marker)
        print(indent + '%s: %s' % (match['message'], context_text_marked))
        if 'replacements' in match and len(match['replacements']) > 0:
            print(indent + 'Suggestions: %s' %
                  ', '.join(['[%d] %s' % (idx, v['value']) for idx, v in enumerate(match['replacements'][:10])]))
            print(indent + '> Accept best (return), Add to ignore (i), Select suggestion (0-9), Reject all (any other key)?'),
            key = msvcrt.getch()
            print('\n')

            offset = match['offset'] + offs_adj
            length = match['length']
            if key == '\r' or (key >= '0' and key <= '9'):
                index = 0
                if key >= '0' and key <= '9':
                    index = ord(key) - ord('0')
                replacement = match['replacements'][index]['value']
                orig_text_len = len(corrected_text)
                corrected_text = corrected_text[:offset] + replacement + corrected_text[offset+length:]
                offs_adj = offs_adj + (len(corrected_text) - orig_text_len)

                offset = match['offset'] + offs_adj_mrks
                orig_text_len = len(corrected_text_mrks)
                corrected_text_mrks = corrected_text_mrks[:offset] + replacement + corrected_text_mrks[offset+length:]
                offs_adj_mrks = offs_adj_mrks + (len(corrected_text_mrks) - orig_text_len)
            elif key == 'i':
                ignore_words.append(corrected_text[offset:offset+length])
            else: #key == '\x08'
                pass

    return corrected_text, corrected_text_mrks


if __name__ == "__main__":
    print('C2C Autocorrect v0.1 by billw\n')

    parser = argparse.ArgumentParser(
        description='Detect grammar and spelling mistakes in an XML file using Language Tool 4.6, and optionally correct them.')
    parser.add_argument('glob_patterns', metavar='glob', type=str,
                        nargs='+', help='globs describing what files to process (e.g. "*_CIV4GameText.xml")')
    mode_group = parser.add_mutually_exclusive_group(required=False)
    mode_group.add_argument('--automatic', dest='automatic', action='store_true',
                            help='apply the proposed changes automatically')
    mode_group.add_argument('--interactive', dest='interactive', action='store_true',
                            help='present proposed changes for user to select from')

    args = parser.parse_args()

    mode = Mode.DETECT

    if args.automatic:
        mode = Mode.AUTOMATIC
        print('Automatic fix mode')
    elif args.interactive:
        mode = Mode.INTERACTIVE
        print('Interactive mode')
    else:
        mode = Mode.DETECT
        print('Detect mode')

    expanded_files = [glob.glob(g) for g in args.glob_patterns]
    flattened_files = [f for files in expanded_files for f in files]
    unique_files = list(set(flattened_files))
    if len(unique_files) == 0:
        print('No files found matching the provided blobs!')
    else:
        print('Processing %d files...' % len(unique_files))
        try:
            autocorrect(unique_files, mode)
        except requests.exceptions.ConnectionError as ex:
            print("ERROR: Can't connect to LanguageTool server, did you forget to run it?")
