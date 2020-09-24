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

def test_parse_unparsable_record(tmpdir, merge_files_oneLR, assert_error):
    path = os.path.join(str(tmpdir), 'unparsable.dlis')
    content = [
        'data/chap3/start.dlis.part',
        'data/chap3/template/invalid-repcode-no-value.dlis.part',
        'data/chap3/object/object.dlis.part',
        'data/chap3/objattr/empty.dlis.part',
        'data/chap3/object/object2.dlis.part',
        # here must go anything that will be considered unrecoverable
        'data/chap3/objattr/reprcode-invalid-value.dlis.part'
    ]
    merge_files_oneLR(path, content)

    with dlisio.load(path) as (f, *_):
        obj = f.object('VERY_MUCH_TESTY_SET', 'OBJECT', 1, 1)
        assert_error("parse interrupted")

        # value is unclear and shouldn't be trusted
        _ = obj.attic['INVALID']
        assert_error("representation code is unknown")

        with pytest.raises(ValueError) as excinfo:
            _ = f.object('VERY_MUCH_TESTY_SET', 'OBJECT2', 1, 1)
        assert "not found" in str(excinfo.value)

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

@pytest.fixture
def create_very_broken_file(tmpdir, merge_files_oneLR, merge_files_manyLR):
    valid = os.path.join(str(tmpdir), 'valid.dlis')
    content = [
        'data/chap3/start.dlis.part',
        'data/chap3/template/invalid-repcode-no-value.dlis.part',
        'data/chap3/object/object.dlis.part',
        # will cause issues on attribute access
        'data/chap3/objattr/empty.dlis.part',
        'data/chap3/object/object2.dlis.part',
        # will cause issues on parsing
        'data/chap3/objattr/reprcode-invalid-value.dlis.part'
    ]
    merge_files_oneLR(valid, content)

    content = [
        valid,
        'data/chap3/sul.dlis.part', # will cause issues on load
    ]

    def create_file(path):
        merge_files_manyLR(path, content)
    return create_file


def test_complex(create_very_broken_file, tmpdir):

    path = os.path.join(str(tmpdir), 'complex.dlis')
    create_very_broken_file(path)

    dlisio.set_escape_level("warning")
    with pytest.raises(RuntimeError):
        with dlisio.load(path) as (f, *_):
            pass

    # escape errors on load
    dlisio.set_escape_level("error")
    with dlisio.load(path) as (f, *_):

        # fail again on parsing objects not parsed on load
        dlisio.set_escape_level("warning")
        with pytest.raises(RuntimeError) as excinfo:
            _ = f.object('VERY_MUCH_TESTY_SET', 'OBJECT', 1, 1)
        assert "parse: error on parsing" in str(excinfo.value)

        # escape errors on parsing
        dlisio.set_escape_level("error") # bypass errors on parsing
        obj = f.object('VERY_MUCH_TESTY_SET', 'OBJECT', 1, 1)

        dlisio.set_escape_level("warning")
        # set is parsed, but user should get error anyway from other source
        with pytest.raises(RuntimeError) as excinfo:
            _ = f.object('VERY_MUCH_TESTY_SET', 'OBJECT', 1, 1)
        assert "Message from object set" in str(excinfo.value)

        # fail on attribute access
        dlisio.set_escape_level("warning")
        with pytest.raises(RuntimeError):
            _ = obj.attic['INVALID']

        # retrieve whatever value from errored attribute
        dlisio.set_escape_level("error")
        _ = obj.attic['INVALID']
