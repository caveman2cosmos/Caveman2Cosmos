import argparse
from lxml import etree
# import xml.etree.ElementTree as ET
import re
import msvcrt
import glob
import requests
from pylanguagetool import api
import logging

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

# define Python user-defined exceptions
class ExitEarly(Exception):
   pass

def autocorrect(files, mode):
    ignore_words = load_string_list('ignore_word_list.txt')
    ignore_tags = load_string_list('ignore_tag_list.txt')
    ignore_rules = load_string_list('ignore_rules_list.txt')

    for filename in files:
        print(filename)
        # text_file = ET.parse(filename)
        root = etree.parse(filename).getroot()
        #nspace = namespace(root)
        nsmap = {'': namespace(root)}

        try:
            # .register_namespace('', nspace)
            for text_elem in root.findall('TEXT', nsmap):
                tag_elem = text_elem.find('Tag', nsmap)
                if tag_elem is not None and tag_elem.text not in ignore_tags:
                    eng_elem = text_elem.find('English', nsmap)
                    if eng_elem is not None:
                        autocorrect_element(
                            eng_elem, tag_elem.text, ignore_words, ignore_tags, ignore_rules, mode, '  ')
        except ExitEarly:
            pass

        if mode != Mode.DETECT:
            etree.ElementTree(root).write(filename, encoding="utf-8", xml_declaration=True, pretty_print=True)

    # We can't have changed the lists unless we were in interactive mode
    if mode == Mode.INTERACTIVE:
        save_string_list('ignore_word_list.txt', ignore_words)
        save_string_list('ignore_tag_list.txt', ignore_tags)
        save_string_list('ignore_rules_list.txt', ignore_rules)


def autocorrect_element(eng_elem, tag, ignore_words, ignore_tags, ignore_rules, mode, indent):
    results = api.check(eng_elem.text,
                        api_url='http://localhost:8081/v2/', lang='en-US', disabled_rules=ignore_rules, pwl=ignore_words)
    if results and 'matches' in results and len(results['matches']) > 0:
        matches = [r for r in results['matches'] if eng_elem.text[r['offset']
            :r['offset']+r['length']] not in ignore_words and r['rule']['id'] not in ignore_rules]
        if len(matches) > 0:
            # Make sure its sorted by offset (it should be already)
            matches.sort(key=lambda x: x['offset'])

            print(indent + u'{0} errors found in text "{1}":'.format(len(matches), eng_elem.text))
            indent = indent + '    '

            # We need to keep track of changes introduced by corrections to
            # correctly apply consecutive ones
            corrected_text, corrected_text_with_markers = apply_corrections(
                eng_elem.text, matches)

            if eng_elem.text != corrected_text:
                print(indent + u'Corrected text: {0}'.format(corrected_text_with_markers))

            if mode == Mode.INTERACTIVE:
                if eng_elem.text != corrected_text:
                    print(indent + 'Accept all (return), Skip (s), Interactive (space), Ignore %s (x), Exit (esc)?' % tag),
                else:
                    print(indent + 'Skip (s), Interactive (space), Ignore %s (x), Exit (esc)?' % tag),

                key = msvcrt.getch()
                print('')
                if key == b'\r':
                    eng_elem.text = corrected_text
                    print(indent + 'Applied all suggested changes')
                elif key == b' ':
                    print(indent + 'Entering interactive mode')
                    corrected_text, corrected_text_with_markers = apply_corrections_interactive(
                        eng_elem.text, matches, ignore_words, ignore_rules, indent + '  ')
                    print(indent + u'Corrected text: {0}'.format(corrected_text_with_markers))
                    eng_elem.text = corrected_text
                elif key == b'x':
                    ignore_tags.append(tag)
                    print(indent + 'Added %s to the global ignore list' % tag)
                elif key == b's':
                    print(indent + 'Skipping %s' % tag)
                else:
                    raise ExitEarly
            elif mode == Mode.AUTOMATIC:
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

def create_error_underlines(context):
    offset = context['offset']
    length = context['length']
    return ' ' * offset + '^' * length

def replace(replacement, match, offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks):
    # nonlocal offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks

    offset = match['offset'] + offs_adj
    length = match['length']
    orig_text_len = len(corrected_text)
    corrected_text = corrected_text[:offset] + replacement + corrected_text[offset+length:]
    offs_adj = offs_adj + (len(corrected_text) - orig_text_len)

    offset = match['offset'] + offs_adj_mrks
    orig_text_len = len(corrected_text_mrks)
    corrected_text_mrks = corrected_text_mrks[:offset] + replacement + corrected_text_mrks[offset+length:]
    offs_adj_mrks = offs_adj_mrks + (len(corrected_text_mrks) - orig_text_len)

    return offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks

def apply_corrections_interactive(text, matches, ignore_words, ignore_rules, indent, marker='#'):
    # We need to keep track of changes introduced by corrections to
    # correctly apply consecutive ones
    offs_adj = 0
    corrected_text = text

    offs_adj_mrks = 0
    corrected_text_mrks = text

    for match in matches:
        offset = match['offset'] + offs_adj
        length = match['length']
        to_replace = corrected_text[offset:offset+length]
        if to_replace not in ignore_words and match['rule']['id'] not in ignore_rules:
            # context_text_marked = highlight_error_in_context(match['context'], marker)
            context = match['context']
            error_underline = create_error_underlines(context)
            prefix = indent + '%s (%s [%s]): ' % (match['message'], match['rule']['issueType'], match['rule']['id'])
            print(u'{0}{1}'.format(prefix, context['text']))
            prefix_space = ' ' * len(prefix)
            print(u'{0}{1}'.format(prefix_space, error_underline))

            can_fix = 'replacements' in match and len(match['replacements']) > 0
            if can_fix:
                print(indent + u'Suggestions: {0}'.format(
                    u', '.join([u'[{0}] {1}'.format(idx, v['value']) for idx, v in enumerate(match['replacements'][:10])])))
                print(indent + '> Accept best (return), Add to ignore (x), Select suggestion (0-9), Skip (s), Ignore rule (e), Custom entry (c), Exit(esc)?'),
            else:
                print(indent + '> Skip (s), Ignore rule (e), Exit(esc)?'),

            key = msvcrt.getch()
            print('\n')

            if can_fix and (key == b'\r' or (key >= b'0' and key <= b'9')):
                index = 0
                if key >= b'0' and key <= b'9':
                    index = ord(key) - ord(b'0')
                # offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks = replace(match['replacements'][index]['value'], match, offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks)
                offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks = replace(match['replacements'][index]['value'], match, offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks)
                # replacement = match['replacements'][index]['value']
                # orig_text_len = len(corrected_text)
                # corrected_text = corrected_text[:offset] + replacement + corrected_text[offset+length:]
                # offs_adj = offs_adj + (len(corrected_text) - orig_text_len)

                # offset = match['offset'] + offs_adj_mrks
                # orig_text_len = len(corrected_text_mrks)
                # corrected_text_mrks = corrected_text_mrks[:offset] + replacement + corrected_text_mrks[offset+length:]
                # offs_adj_mrks = offs_adj_mrks + (len(corrected_text_mrks) - orig_text_len)
            elif can_fix and key == b'x':
                ignore_words.append(to_replace)
            elif key == b'e':
                ignore_rules.append(match['rule']['id'])
            elif can_fix and key == b'c':
                #offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks = replace(input('Enter text to replace %s > ' % to_replace), match, offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks)
                offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks = replace(input(u'Enter text to replace {0} > '.format(to_replace)), match, offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks)
            elif key == b's':
                pass
            else:
                raise ExitEarly

    return corrected_text, corrected_text_mrks


if __name__ == "__main__":
    print('C2C Autocorrect v0.1 by billw\n')

    parser = argparse.ArgumentParser(
        description='Detect grammar and spelling mistakes in an XML file using Language Tool 4.6, and optionally correct them.')
    parser.add_argument('glob_patterns', metavar='glob', type=str,
                        nargs='+', help='globs describing what files to process (e.g. "*_CIV4GameText.xml")')
    parser.add_argument('--log', dest='log_file', action='store', help='override log file name', default='Autocorrect.log')

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

    logging.basicConfig(filename=args.log_file, level=logging.DEBUG)

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
            print("ERROR: Can't connect to LanguageTool server, did you forget to start it?")
