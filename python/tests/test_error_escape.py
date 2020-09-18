"""
Testing "error escape" feature on various levels of representation
(user-facade functionality and underlying logical format)
"""

import pytest
import os

import dlisio

@pytest.fixture(autouse=True)
def setup_error_escape():
    # autouse fixture. Level will be set to error
    # tests that do not like this escape level must set their own explicitly
    default_escape_level = dlisio.get_escape_level()
    dlisio.set_escape_level("error")
    yield
    dlisio.set_escape_level(default_escape_level)

def test_set_invalid_escape_level():
    with pytest.raises(KeyError) as excinfo:
        dlisio.set_escape_level("invalid")
    assert "Invalid severity argument 'invalid'" in str(excinfo.value)

# TODO: test_errors_explicit (let users examine the errors regardless of escape
# level set)

# TODO: test mix (do not fail on truncation, but fail on bad attribute access)

# TODO: test_truncated

# TODO: test_extract

# TODO: test_parse_exeptions

def test_parse_error_escaped(tmpdir, merge_files_oneLR,
                                         assert_error):
    path = os.path.join(str(tmpdir), 'error-on-attribute-access.dlis')
    content = [
        'data/chap3/start.dlis.part',
        'data/chap3/template/invalid-repcode-no-value.dlis.part',
        'data/chap3/object/object.dlis.part',
        'data/chap3/objattr/empty.dlis.part',
    ]
    merge_files_oneLR(path, content)

    with dlisio.load(path) as (f, *_):
        obj = f.object('VERY_MUCH_TESTY_SET', 'OBJECT', 1, 1)
        # value is unclear and shouldn't be trusted
        _ = obj.attic['INVALID']
        assert_error("representation code is unknown")

def test_parse_warning_errored(tmpdir, merge_files_oneLR):
    path = os.path.join(str(tmpdir), 'replacement-set.dlis')
    content = [
        'data/chap3/sul.dlis.part',
        'data/chap3/set/replacement.dlis.part',
        'data/chap3/template/default.dlis.part',
        'data/chap3/object/object.dlis.part'
    ]
    merge_files_oneLR(path, content)

    dlisio.set_escape_level("info")
    with dlisio.load(path) as (f, *_):
        with pytest.raises(RuntimeError) as excinfo:
            _ = f.object('REPLACEMENT', 'OBJECT', 1, 1)
        assert "Replacement sets are not supported" in str(excinfo.value)

def test_parse_info_errored(tmpdir, merge_files_oneLR):
    path = os.path.join(str(tmpdir), 'redundant-set.dlis')
    content = [
        'data/chap3/sul.dlis.part',
        'data/chap3/set/redundant.dlis.part',
        'data/chap3/template/default.dlis.part',
        'data/chap3/object/object.dlis.part'
    ]
    merge_files_oneLR(path, content)

    dlisio.set_escape_level("debug")
    with dlisio.load(path) as (f, *_):
        with pytest.raises(RuntimeError) as excinfo:
            _ = f.object('REDUNDANT', 'OBJECT', 1, 1)
        assert "Redundant sets are not supported" in str(excinfo.value)

