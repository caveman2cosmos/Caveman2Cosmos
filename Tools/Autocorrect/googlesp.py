# Replaces the word under the cursor or selection with Google Search's recommended spelling
# Hosted at http://github.com/noahcoad/google-spell-check

import urllib
import re
from html.parser import HTMLParser

class GoogleSp:
    def correct(self, text):
        # grab html
        html = self.get_page('http://www.google.com/search?hl=en&q=' + urllib.parse.quote(text) + "&meta=&gws_rd=ssl")
        html_parser = HTMLParser()

        # save html for debugging
        # open('page.html', 'w').write(html)

        # pull pieces out
        match = re.search(r'(?:Showing results for|Did you mean|Including results for).*?<a.*?>(.*?)</a>', html)
        if match is None:
            fix = text
        else:
            fix = match.group(1)
            fix = re.sub(r'<.*?>', '', fix)
            fix = html_parser.unescape(fix)

        # return result
        return fix

    def get_page(self, url):
        # the type of header affects the type of response google returns
        # for example, using the commented out header below google does not
        # include "Including results for" results and gives back a different set of results
        # than using the updated user_agent yanked from chrome's headers
        # user_agent = 'Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.0.7) Gecko/2009021910 Firefox/3.0.7'
        user_agent = 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/27.0.1453.116 Safari/537.36'
        headers = {'User-Agent':user_agent,}
        req = urllib.request.Request(url, None, headers)
        with urllib.request.urlopen(req) as page:
            html = str(page.read().decode())
        return html

# p.s. Yes, I'm using hard tabs for indentation.  bite me
# set tabs to whatever level of indentation you like in your editor
# for crying out loud, at least they're consistent here, and use
# the ST2 command "Indentation: Convert to Spaces", which will convert
# to spaces if you really need to be part of the 'soft tabs only' crowd =)