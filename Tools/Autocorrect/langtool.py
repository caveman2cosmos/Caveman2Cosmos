# -*- coding: utf-8 -*-
r"""
Python API wrapper for the languagetool REST API.

Simple usage::

    >>> from pylanguagetool import api
    >>> api.check(
    ...     'This is a example',
    ...     api_url='https://languagetool.org/api/v2/',
    ...     lang='en-US',
    ... )
    {'software': {'name': 'LanguageTool', 'version': '4.6-SNAPSHOT', 'buildDate': '2019-05-15 19:25', 'apiVersion': 1, 'premium': False, 'premiumHint': 'You might be missing errors only the Premium version can find. Contact us at support<at>languagetoolplus.com.', 'status': ''}, 'warnings': {'incompleteResults': False}, 'language': {'name': 'English (US)', 'code': 'en-US', 'detectedLanguage': {'name': 'English (US)', 'code': 'en-US', 'confidence': 0.561}}, 'matches': [{'message': 'Use "an" instead of \'a\' if the following word starts with a vowel sound, e.g. \'an article\', \'an hour\'', 'shortMessage': 'Wrong article', 'replacements': [{'value': 'an'}], 'offset': 8, 'length': 1, 'context': {'text': 'This is a example', 'offset': 8, 'length': 1}, 'sentence': 'This is a example', 'type': {'typeName': 'Other'}, 'rule': {'id': 'EN_A_VS_AN', 'description': "Use of 'a' vs. 'an'", 'issueType': 'misspelling', 'category': {'id': 'MISC', 'name': 'Miscellaneous'}}, 'ignoreForIncompleteSentence': False, 'contextForSureMatch': 1}]}

"""
import requests

class LanguageTool():
    def __init__(self, api_url, lang, mother_tongue=None, preferred_variants=None,
            enabled_rules=None, disabled_rules=None,
            enabled_categories=None, disabled_categories=None,
            enabled_only=False, verbose=False,
            pwl=None):
        """
        Args:
            api_url (str):
                API base url, e.g. ``https://languagetool.org/api/v2/``

            lang: Language of the given text as `RFC 3066`__ language code.
                For example ``en-GB`` or ``de-AT``. ``auto`` is a valid value too
                and will cause the language to be detected automatically.

                __ https://www.ietf.org/rfc/rfc3066.txt

            mother_tongue: Native language of the author as `RFC 3066`__ language
                code.

                __ https://www.ietf.org/rfc/rfc3066.txt

            preferred_variants (str):
                Comma-separated list of preferred language variants. The language
                detector used with ``language=auto`` can detect e.g. English, but
                it cannot decide whether British English or American English is
                used. Therefore, this parameter can be used to specify the
                preferred variants like ``en-GB`` and ``de-AT``. Only available
                with ``language=auto``.

            enabled_rules (str):
                Comma-separated list of IDs of rules to be enabled

            disabled_rules (str):
                Comma-separated list of IDs of rules to be disabled.

            enabled_categories (str):
                Comma-separated list of IDs of categories to be enabled.

            disabled_categories (str):
                Comma-separated list of IDs of categories to be disabled.

            enabled_only (bool):
                If ``True``, only the rules and categories whose IDs are specified
                with ``enabledRules`` or ``enabledCategories`` are enabled.
                Defaults to ``False``.

            verbose (bool):
                If ``True``, a more verbose output will be printed. Defaults to
                ``False``.

            pwl (List[str]):
                Personal world list. A custom dictionary of words that should be
                excluded from spell checking errors.
        """
        self.api_url = api_url
        self.lang = lang
        self.mother_tongue = mother_tongue
        self.preferred_variants = preferred_variants
        self.enabled_rules = enabled_rules
        self.disabled_rules = disabled_rules
        self.enabled_categories = enabled_categories
        self.disabled_categories = disabled_categories
        self.enabled_only = enabled_only
        self.verbose = verbose
        self.pwl = pwl

        self.session = requests.Session()
        self.session.trust_env = False

    def get_languages(self):
        """
        Return supported languages as a list of dictionaries.

        Args:
            api_url (str): API base url.

        Returns:
            List[dict]:
                Supported languages as a list of dictionaries.

                Each dictionary contains three keys, ``name``, ``code`` and
                ``longCode``::

                    {
                        "name":"English (GB)",
                        "code":"en",
                        "longCode":"en-GB"
                    }

        """
        r = self.session.get(self.api_url + "languages")
        return r.json()

    def _is_in_pwl(self, match):
        start = match['context']['offset']
        end = start + match['context']['length']
        word = match['context']['text'][start:end]
        return word in self.pwl

    def check(self, input_text):
        """
        Check given text and return API response as a dictionary.

        Args:
            input_text (str):
                Plain text that will be checked for spelling mistakes.

        Returns:
            dict:
                A dictionary representation of the JSON API response.
                The most notable key is ``matches``, which contains a list of all
                spelling mistakes that have been found.

                E.g.::

                    {'language': {'code': 'en-US',
                                'detectedLanguage': {'code': 'en-US',
                                                    'confidence': 0.561,
                                                    'name': 'English (US)'},
                                'name': 'English (US)'},
                    'matches': [{'context': {'length': 1,
                                            'offset': 8,
                                            'text': 'This is a example'},
                                'contextForSureMatch': 1,
                                'ignoreForIncompleteSentence': False,
                                'length': 1,
                                'message': 'Use "an" instead of \'a\' if the following word '
                                            "starts with a vowel sound, e.g. 'an article', 'an "
                                            "hour'",
                                'offset': 8,
                                'replacements': [{'value': 'an'}],
                                'rule': {'category': {'id': 'MISC', 'name': 'Miscellaneous'},
                                        'description': "Use of 'a' vs. 'an'",
                                        'id': 'EN_A_VS_AN',
                                        'issueType': 'misspelling'},
                                'sentence': 'This is a example',
                                'shortMessage': 'Wrong article',
                                'type': {'typeName': 'Other'}}],
                    'software': {'apiVersion': 1,
                                'buildDate': '2019-05-15 19:25',
                                'name': 'LanguageTool',
                                'premium': False,
                                'premiumHint': 'You might be missing errors only the Premium '
                                                'version can find. Contact us at '
                                                'support<at>languagetoolplus.com.',
                                'status': '',
                                'version': '4.6-SNAPSHOT'},
                    'warnings': {'incompleteResults': False}}

        """
        post_parameters = {
            "text": input_text,
            "language": self.lang,
        }

        if self.mother_tongue:
            post_parameters["motherTongue"] = self.mother_tongue
        if self.preferred_variants:
            post_parameters["preferredVariants"] = self.preferred_variants
        if self.enabled_rules:
            post_parameters["enabledRules"] = self.enabled_rules
        if self.disabled_rules:
            post_parameters["disabledRules"] = self.disabled_rules
        if self.enabled_categories:
            post_parameters["enabledCategories"] = self.enabled_categories
        if self.disabled_categories:
            post_parameters["disabledCategories"] = self.disabled_categories
        if self.enabled_only:
            post_parameters["enabledOnly"] = True

        r = self.session.post(self.api_url + "check", data=post_parameters)

        if r.status_code != 200:
            raise ValueError(r.text)
        if self.verbose:
            print(post_parameters)
            print(r.json())
        data = r.json()
        if self.pwl:
            matches = data.pop('matches', [])
            data['matches'] = [
                match for match in matches
                if not self._is_in_pwl(match)
            ]
        return data
