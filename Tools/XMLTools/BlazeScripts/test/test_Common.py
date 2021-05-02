from pathlib import Path
from tempfile import NamedTemporaryFile

from ..Common import load_tree


def test_load_tree() -> None:
    with NamedTemporaryFile() as temp_file_str:
        temp_file = Path(temp_file_str.name)
        raw_xml = """<?xml version="1.0" encoding="UTF-8"?>
<note>
  <dummy1>Value1</dummy1>
  <dummy2>Value2</dummy2>
  <heading>Some-Header</heading>
  <body>Some content here</body>
</note>"""
        temp_file.write_text(raw_xml)
        tree, root, schema = load_tree(str(temp_file))
        assert isinstance(schema, str)
        assert schema == ""
        assert root.text == "\n  "
        assert tree is not None
