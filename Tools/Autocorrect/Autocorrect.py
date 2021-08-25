import argparse
from lxml import etree
# import xml.etree.ElementTree as ET
import re
import os
import time
import msvcrt
import glob
import random
import requests
import pickle
import hashlib
# from pylanguagetool import api
import logging
import colorama
from colorama import Fore, Back, Style
from spellchecker import SpellChecker
from googlesp import GoogleSp
from random import randint
from time import sleep
from langtool import LanguageTool
import urllib

script_dir = os.path.dirname(os.path.realpath(__file__))

google_cache = {}

class Mode:
    DETECT = 0
    AUTOMATIC = 1
    INTERACTIVE = 2

def load_string_list(filename):
    with open(os.path.join(script_dir, filename)) as f:
        return [line.strip() for line in f]

def save_string_list(filename, strings):
    with open(os.path.join(script_dir, filename), 'w') as f:
        for item in strings:
            f.write('%s\n' % item)

def save_obj(obj, name):
    with open(os.path.join(script_dir, name + '.pkl'), 'wb') as f:
        pickle.dump(obj, f, pickle.HIGHEST_PROTOCOL)

def load_obj(name):
    with open(os.path.join(script_dir, name + '.pkl'), 'rb') as f:
        return pickle.load(f)

def namespace(element):
    m = re.match(r'\{(.*)\}', element.tag)
    return m.group(1) if m else ''

class ExitEarly(Exception):
   pass

def autocorrect(files, mode, args):
    ignore_words = load_string_list('ignore_word_list.txt')
    ignore_tags = load_string_list('ignore_tag_list.txt')
    ignore_rules = load_string_list('ignore_rules_list.txt') + ['__ignore']
    ignore_instances = set(load_string_list('ignore_instances_list.txt'))

    global google_cache

    try:
        google_cache = load_obj('google_cache')
    except:
        google_cache = {}

    langtool = LanguageTool(api_url='http://localhost:8081/v2/', lang='en-US')
    spell = SpellChecker(distance=1, local_dictionary=args.spell_dict)
    google = GoogleSp()

    fancy = args.fancy

    try:
        for filename in files:
            print(filename)
            indent = '  '
            print(Fore.GREEN + indent + u'Searching', end='')
            root = etree.parse(filename).getroot()
            nsmap = {'': namespace(root)}
            try:
                for text_elem in root.findall('TEXT', nsmap):
                    tag_elem = text_elem.find('Tag', nsmap)
                    if tag_elem is not None and tag_elem.text not in ignore_tags:
                        eng_elem = text_elem.find('English', nsmap)
                        if eng_elem is not None:
                            autocorrect_element(eng_elem, tag_elem.text, ignore_words, ignore_tags, ignore_rules, ignore_instances, mode, indent, fancy, langtool, spell, google)
            finally:
                if mode != Mode.DETECT:
                    etree.ElementTree(root).write(filename, encoding="utf-8", xml_declaration=True, pretty_print=True)
    except ExitEarly:
        print('\n\nExited early, progress has been saved')
    finally:
        # We can't have changed the lists unless we were in interactive mode
        if mode == Mode.INTERACTIVE:
            print('\n\nSaving ignore lists and caches')
            save_string_list('ignore_word_list.txt', ignore_words)
            save_string_list('ignore_tag_list.txt', ignore_tags)
            ignore_rules.remove('__ignore')
            save_string_list('ignore_rules_list.txt', ignore_rules)
            save_string_list('ignore_instances_list.txt', ignore_instances)
            save_obj(google_cache, 'google_cache')

spell_check_replacement_rules = ['MORFOLOGIK_RULE_EN_US']

def get_spellchecker_candidates(word, spell):
    candidates = spell.candidates(word.lower())
    # If spellcheck couldn't find the word then it returns the word itself, so remove it!
    if word.lower() in candidates:
        candidates.remove(word.lower())
    # Correct capitalization
    if word == word.capitalize():
        candidates = set([c.capitalize() for c in candidates])
    return list(candidates)

last_googled = 0

def print_progress(color = Style.RESET_ALL):
    if color == 0:
        print(random.choice([Fore.BLUE, Fore.CYAN, Fore.GREEN, Fore.MAGENTA, Fore.RED, Fore.WHITE, Fore.YELLOW]) +'.' + Style.RESET_ALL, end='')
    else:
        print(color +'.' + Style.RESET_ALL, end='')

def apply_spellcheck(matches, ignore_words, spell, google):
    global last_googled
    global google_cache

    spelling_matches = [m for m in matches if m['rule']['id'] in spell_check_replacement_rules]
    for match in spelling_matches:
        context = match['context']
        text = context['text']
        word = text[context['offset']:context['offset']+context['length']]
        if word in ignore_words:
            matches.remove(match)
            continue

        print_progress(Fore.YELLOW)
        candidates = get_spellchecker_candidates(word, spell)

        if not candidates or len(candidates) == 0:
            matches.remove(match)
            continue
        try:
            if word in google_cache:
                google_candidate = google_cache[word]
            else:
                # don't spam google too hard...
                sleep(max(0.01, 1 - (time.time() - last_googled)))
                print_progress(Fore.RED)
                google_candidate = google.correct(word)
                last_googled = time.time()
                google_cache[word] = google_candidate

            if not google_candidate or google_candidate == word:
                matches.remove(match)
                continue
            candidates = [google_candidate] + candidates
        except urllib.error.HTTPError as ex:
            print('**** ERROR: google spell check request failed: ' + ex.reason)
            print('**** Falling back to default spellcheck')

        candidates = candidates + [r['value'] for r in match['replacements']]
        unique_candidates = []
        [unique_candidates.append(x) for x in candidates if x not in unique_candidates]
        if len(candidates) == 0:
            matches.remove(match)
        else:
            match['replacements'] = [{'value': v} for v in unique_candidates]

def get_instance_key(tag, match):
    hash_str = match['rule']['id'] + str(match['context'])
    return tag + ' ' + hashlib.md5(hash_str.encode()).hexdigest()

def matches_summary(matches, indent):
    messages = [m['message'] for m in matches]
    for m in set(messages):
        freq = messages.count(m)
        print (Fore.YELLOW + indent + str(freq) + 'x ' + m)

def autocorrect_element(eng_elem, tag, ignore_words, ignore_tags, ignore_rules, ignore_instances, mode, indent, fancy, langtool, spell, google):
    print_progress(Fore.BLUE)
    try:
        results = langtool.check(eng_elem.text)
    except ValueError as ex:
        print ("Exception checking " + str(eng_elem) + " from " + str(tag) + ":")
        print (str(ex))
        return

    if results and 'matches' in results and len(results['matches']) > 0:
        matches = [m for m in results['matches'] if (m['rule']['id'] not in ignore_rules and get_instance_key(tag, m) not in ignore_instances)]

        # remove_ignore_matches(result.matches, ignore_tags, ignore_rules, ignore_instances)
        apply_spellcheck(matches, ignore_words, spell, google)

        if len(matches) > 0:
            print()
            # Make sure its sorted by offset (it should be already)
            matches.sort(key=lambda x: x['offset'])

            # We need to keep track of changes introduced by corrections to
            # correctly apply consecutive ones
            corrected_text, corrected_text_with_markers, orig_text_with_markers = apply_corrections(
                eng_elem.text, matches, fancy)

            print(Fore.YELLOW + indent + str(len(matches)) + ' possible errors found in ' + Fore.CYAN + tag + Fore.YELLOW + ': ')
            matches_summary(matches, indent + '- ')
            print(Fore.WHITE + indent + '    ' + orig_text_with_markers)

            if eng_elem.text != corrected_text:
                print(Fore.GREEN + indent + u'Suggested text:')
                print(Fore.WHITE + indent + '    ' + corrected_text_with_markers)

            if mode == Mode.INTERACTIVE:
                if eng_elem.text != corrected_text:
                    print(Fore.BLUE + indent + '> Accept all (return), Skip (s), Interactive (space), Ignore all (i), Ignore ' + Fore.CYAN + tag + Fore.BLUE + ' (x), Exit (esc)?', end = '')
                else:
                    print(Fore.BLUE + indent + '> Skip (s), Interactive (space), Ignore all (i), Ignore ' + Fore.CYAN + tag + Fore.BLUE + ' (x), Exit (esc)?', end = '')
                key = msvcrt.getch()
                print('')
                if key == b'\r':
                    eng_elem.text = corrected_text
                    print(Fore.GREEN + indent + 'Applied all suggested changes')
                elif key == b' ':
                    print(Fore.GREEN + indent + 'Entering interactive mode')
                    corrected_text, corrected_text_with_markers = apply_corrections_interactive(
                        tag, eng_elem.text, matches, ignore_words, ignore_rules, ignore_instances, indent + '    ', fancy)
                    print(Fore.GREEN + indent + u'Corrected text:')
                    print(Fore.WHITE + indent + '    ' + corrected_text_with_markers)
                    eng_elem.text = corrected_text
                elif key == b'i':
                    for m in matches:
                        ignore_instances.add(get_instance_key(tag, m))
                    print(Fore.GREEN + indent + 'Added all errors to the ignore list')
                elif key == b'x':
                    ignore_tags.append(tag)
                    print(Fore.GREEN + indent + 'Added ' + Fore.CYAN + tag + Fore.GREEN + ' to the global ignore list')
                elif key == b'\x1b':
                    print(Fore.GREEN + indent + 'Exiting...')
                    raise ExitEarly
                else: #key == b's':
                    print(Fore.GREEN + indent + 'Skipping ' + Fore.CYAN + tag)
            elif mode == Mode.AUTOMATIC:
                eng_elem.text = corrected_text
            print('\n' + Fore.GREEN + indent + u'Continuing search', end='')

def apply_corrections(text, matches, fancy):
    # We need to keep track of changes introduced by corrections to
    # correctly apply consecutive ones
    offset_adjust = 0
    corrected_text = text

    start_marker = '#'
    end_marker = '#'
    start_marker_corrected = '#'
    end_marker_corrected = '#'
    if fancy:
        start_marker = Fore.RED
        end_marker = Style.RESET_ALL
        start_marker_corrected = Fore.GREEN
        end_marker_corrected = Style.RESET_ALL
    offset_adjust_with_markers = 0
    corrected_text_with_markers = text

    offset_adjust_orig = 0
    orig_with_markers = text
    for match in matches:
        if 'replacements' in match and len(match['replacements']) > 0:
            replacement = match['replacements'][0]['value']

            offset = match['offset'] + offset_adjust
            orig_text_len = len(corrected_text)
            corrected_text = corrected_text[:offset] + replacement + corrected_text[offset+match['length']:]
            offset_adjust = offset_adjust + (len(corrected_text) - orig_text_len)

            offset = match['offset'] + offset_adjust_with_markers
            orig_text_len = len(corrected_text_with_markers)
            corrected_text_with_markers = corrected_text_with_markers[:offset] + start_marker_corrected + replacement + end_marker_corrected + corrected_text_with_markers[offset+match['length']:]
            offset_adjust_with_markers = offset_adjust_with_markers + (len(corrected_text_with_markers) - orig_text_len)

            offset = match['offset'] + offset_adjust_orig
            orig_text_len = len(orig_with_markers)
            orig_with_markers = orig_with_markers[:offset] + start_marker + orig_with_markers[offset:offset+match['length']] + end_marker + orig_with_markers[offset+match['length']:]
            offset_adjust_orig = offset_adjust_orig + (len(orig_with_markers) - orig_text_len)
    return corrected_text, corrected_text_with_markers, orig_with_markers

def create_error_underlines(context):
    offset = context['offset']
    length = context['length']
    return ' ' * offset + '^' * length

def replace(replacement, match, offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks, fancy):
    if fancy:
        start_marker_corrected = Fore.GREEN
        end_marker_corrected = Style.RESET_ALL
    offset = match['offset'] + offs_adj
    length = match['length']
    orig_text_len = len(corrected_text)
    corrected_text = corrected_text[:offset] + replacement + corrected_text[offset+length:]
    offs_adj = offs_adj + (len(corrected_text) - orig_text_len)

    offset = match['offset'] + offs_adj_mrks
    orig_text_len = len(corrected_text_mrks)
    corrected_text_mrks = corrected_text_mrks[:offset] + start_marker_corrected + replacement + end_marker_corrected + corrected_text_mrks[offset+length:]
    offs_adj_mrks = offs_adj_mrks + (len(corrected_text_mrks) - orig_text_len)

    return offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks

def apply_corrections_interactive(tag, text, matches, ignore_words, ignore_rules, ignore_instances, indent, fancy):
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

            context = match['context']
            context_text = context['text']
            print(Fore.YELLOW + indent + match['message'] + Style.DIM + '(%s [%s])' % (match['rule']['issueType'], match['rule']['id']) + Style.NORMAL + ': ')

            if not fancy:
                error_underline = create_error_underlines(context)
                print(Fore.WHITE + indent + '    ' + context_text)
                print(Fore.RED + indent + '    ' + error_underline)
            else:
                offset = context['offset']
                length = context['length']
                context_text_with_markers = Fore.WHITE + context_text[:offset] + Fore.RED + context_text[offset:offset+length] + Fore.WHITE + context_text[offset+length:]
                print(Fore.WHITE + indent + '    ' + context_text_with_markers)

            can_fix = 'replacements' in match and len(match['replacements']) > 0
            if can_fix:
                print(Fore.GREEN + indent + u'Suggestions: ' + u', '.join([Fore.GREEN + '[' + str(idx) + '] ' + Fore.WHITE + v['value'] for idx, v in enumerate(match['replacements'][:10])]))
                print(Fore.BLUE + indent + '> Accept best (return), Add to dictionary (a), Select suggestion (0-9), Ignore (i), Skip (s), Custom entry (c), Exit(esc)?', end = '')
            else:
                print(Fore.BLUE + indent + '> Skip (s), Ignore (i), Exit(esc)?', end = '')

            key = msvcrt.getch()
            print('')

            if can_fix and (key == b'\r' or (key >= b'0' and key <= b'9')):
                index = 0
                if key >= b'0' and key <= b'9':
                    index = ord(key) - ord(b'0')
                replacement = match['replacements'][index]['value']
                print(Fore.GREEN + indent + 'Applied suggestion ' + Fore.WHITE + replacement)
                offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks = replace(replacement, match, offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks, fancy)
            elif can_fix and key == b'a':
                print(Fore.GREEN + indent + 'Added word ' + Fore.RED + to_replace + Fore.GREEN + ' to the global ignore list')
                ignore_words.append(to_replace)
            elif key == b'i':
                print(Fore.GREEN + indent + 'Added instance to the ignore list')
                ignore_instances.add(get_instance_key(tag, match))
            # elif key == b'e':
            #     print(Fore.GREEN + indent + 'Added rule ' + Fore.WHITE + match['rule']['id'] + Fore.GREEN + ' to the global ignore list')
            #     ignore_rules.append(match['rule']['id'])
            elif can_fix and key == b'c':
                print(Fore.BLUE + indent + 'Enter text to replace ' + Fore.RED + to_replace + Fore.BLUE + ' > ' + Fore.WHITE, end = '')
                offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks = replace(input(), match, offs_adj, corrected_text, offs_adj_mrks, corrected_text_mrks, fancy)
            elif key == b'\x1b':
                print(Fore.GREEN + indent + 'Exiting...')
                raise ExitEarly
            else: # key == b's':
                print(Fore.GREEN + indent + 'Skipping this suggestion')

    return corrected_text, corrected_text_mrks

if __name__ == "__main__":
    colorama.init(autoreset=True)

    print(Style.BRIGHT + Fore.BLUE + 'C2C Autocorrect' + Style.RESET_ALL + ' v0.1 by billw\n')

    parser = argparse.ArgumentParser(description='Detect grammar and spelling mistakes in an XML file using Language Tool 4.6, and optionally correct them.')
    parser.add_argument('glob_patterns', metavar='glob', type=str,
                        nargs='+', help='globs describing what files to process (e.g. "*_CIV4GameText.xml")')
    parser.add_argument('--log', dest='log_file', action='store', help='override log file name', default='Autocorrect.log')
    parser.add_argument('--non-fancy', dest='fancy', action='store_false',
                            help="don't use colors for markup", default=True)
    parser.add_argument('--dict', dest='spell_dict', help="use specified spellcheck dictionary", default=None)
    mode_group = parser.add_mutually_exclusive_group(required=False)
    mode_group.add_argument('--automatic', dest='automatic', action='store_true',
                            help='apply the proposed changes automatically')
    mode_group.add_argument('--interactive', dest='interactive', action='store_true',
                            help='present proposed changes for user to select from')
    args = parser.parse_args()

    mode = Mode.DETECT

    if args.automatic:
        mode = Mode.AUTOMATIC
        print(Fore.GREEN + 'Automatic fix mode')
    elif args.interactive:
        mode = Mode.INTERACTIVE
        print(Fore.GREEN + 'Interactive mode')
    else:
        mode = Mode.DETECT
        print(Fore.GREEN + 'Detect mode')

    logging.basicConfig(filename=args.log_file, level=logging.DEBUG)

    expanded_files = [glob.glob(g) for g in args.glob_patterns]
    flattened_files = [f for files in expanded_files for f in files]
    unique_files = list(set(flattened_files))
    if len(unique_files) == 0:
        print(Fore.YELLOW + 'No files found matching the provided globs!')
    else:
        if len(unique_files) > 1:
            print('Processing %d files...' % len(unique_files))

        try:
            autocorrect(unique_files, mode, args)

            print('... Complete!')
        except requests.exceptions.ConnectionError as ex:
            print(Fore.RED + "ERROR: Can't connect to LanguageTool server, did you forget to start it?")
