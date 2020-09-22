"""
Testing "error escape" feature on various levels of representation
(user-facade functionality and underlying logical format)
"""

import pytest
import os
import numpy as np

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

def test_unescapable_notdlis(assert_error):
    path = 'data/chap2/nondlis.txt'
    with pytest.raises(RuntimeError) as excinfo:
        _ = dlisio.load(path)
    assert "could not find visible record envelope" in str(excinfo.value)

def test_truncated_in_data(assert_error):
    path = 'data/chap2/truncated-in-second-lr.dlis'
    with dlisio.load(path) as (f, *_):
        assert_error("findoffsets: file truncated")
        assert len(f.channels) == 1

def test_truncated_in_lrsh(assert_error):
    path = 'data/chap2/truncated-in-lrsh.dlis'
    with dlisio.load(path) as (f, *_):
        assert_error("unexpected EOF when reading record")
        assert len(f.channels) == 1

def test_truncated_on_lrs_vr_over(assert_error):
    path = 'data/chap2/truncated-on-lrs-vr-over.dlis'
    with dlisio.load(path) as (f, *_):
        assert_error("last logical record segment expects successor")
        assert len(f.channels) == 0

def test_zeroed_before_lrs(assert_error):
    path = 'data/chap2/zeroed-in-1st-lr.dlis'
    with dlisio.load(path) as (f, *_):
        assert_error("Too short logical record")
        assert len(f.channels) == 0

def test_zeroed_before_vr(assert_error):
    path = 'data/chap2/zeroed-in-2nd-lr.dlis'
    with dlisio.load(path) as (f, *_):
        assert_error("Incorrect format version")
        assert len(f.channels) == 1

def test_extract_broken_padbytes(assert_error):
    path = 'data/chap2/padbytes-bad.dlis'
    with dlisio.load(path) as (f, *_):
        assert_error("bad segment trim")
        valid_obj = f.object("VALID-SET", "VALID-OBJ", 10, 0)
        assert valid_obj

def test_findfdata_bad_obname(assert_error):
    path = 'data/chap3/implicit/fdata-broken-obname.dlis'
    with dlisio.load(path) as (f, *_):
        assert_error("Error on reading fdata obname")
        assert "T.FRAME-I.DLIS-FRAME-O.3-C.1" in f.fdata_index

def test_curves_broken_fmt(assert_error):
    path = 'data/chap4-7/iflr/broken-fmt.dlis'
    with dlisio.load(path) as (f, *_):
        frame = f.object('FRAME', 'FRAME-REPRCODE', 10, 0)
        curves = frame.curves()
        assert_error("fmtstr would read past end")
        assert np.array_equal(curves['FRAMENO'], np.array([1, 3]))

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

